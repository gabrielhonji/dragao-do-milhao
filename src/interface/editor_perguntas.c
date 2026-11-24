/**
 * @file editor_perguntas.c
 * @brief Cadastro, edição e exclusão de perguntas na janela do jogo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor_perguntas.h"
#include "componentes.h"
#include "cores.h"
#include "../perguntas/arquivos_perguntas.h"

enum { TOTAL_CAMPOS_TEXTO = 5 };

static const Rectangle cancelar = {24, 464, 200, 52};
static const Rectangle confirmar = {736, 464, 200, 52};
static const Rectangle escolher_resposta = {220, 372, 260, 44};
static const Rectangle escolher_dificuldade = {630, 372, 266, 44};
static const char *nomes_dificuldade[] = {"", "Muito fácil", "Fácil", "Média", "Difícil", "Muito difícil"};

void abrir_editor_perguntas(EditorPerguntas *editor, ModoEditorPerguntas modo, const Pergunta *pergunta)
{
    *editor = (EditorPerguntas){.modo = modo};
    if (pergunta != NULL) {
        editor->pergunta = *pergunta;
        if (editor->pergunta.dificuldade < muito_facil || editor->pergunta.dificuldade > muito_dificil)
            editor->pergunta.dificuldade = muito_facil;
        if (editor->pergunta.resposta < 'A' || editor->pergunta.resposta > 'D')
            editor->pergunta.resposta = 'A';
    }
    else {
        editor->pergunta.resposta = 'A';
        editor->pergunta.dificuldade = muito_facil;
    }
}

/** @brief Calcula a área de texto de uma linha do formulário. */
static Rectangle caixa_texto(int campo)
{
    return (Rectangle){220, 78 + campo * 55, 676, 44}; // Um campo a cada 55 px abaixo do título.
}

/**
 * @brief Lê caracteres para o campo ativo, mantendo sequências UTF-8 completas.
 * @param selecionar_tudo Indica se a próxima edição substitui todo o conteúdo.
 */
static void editar_texto(char *texto, size_t capacidade, bool *selecionar_tudo)
{
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_A))
        *selecionar_tudo = true;

    size_t tamanho = strlen(texto);
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        if (*selecionar_tudo) {
            tamanho = 0;
            *selecionar_tudo = false;
        }
        else if (tamanho > 0) {
            // Um caractere acentuado pode ocupar mais de um byte.
            do {
                tamanho--;
            } while (tamanho > 0 && ((unsigned char)texto[tamanho] & 0xc0) == 0x80);
        }
        texto[tamanho] = '\0';
    }

    for (int codigo = GetCharPressed(); codigo != 0; codigo = GetCharPressed()) {
        // A fonte da interface cobre ASCII e Latin-1, incluindo os acentos do português.
        if (codigo < 32 || (codigo > 126 && codigo < 160) || codigo > 255)
            continue;

        int bytes;
        const char *caractere = CodepointToUTF8(codigo, &bytes);
        if (*selecionar_tudo) {
            tamanho = 0;
            *selecionar_tudo = false;
        }
        if (tamanho + (size_t)bytes >= capacidade)
            continue;

        memcpy(texto + tamanho, caractere, (size_t)bytes);
        tamanho += (size_t)bytes;
        texto[tamanho] = '\0';
    }
}

/**
 * @brief Aplica uma operação a uma cópia do banco e só a adota depois de gravá-la.
 * @return Texto do erro ou NULL em caso de sucesso.
 */
static const char *gravar_mudanca(Banco_perguntas *banco, EditorPerguntas *editor)
{
    Banco_perguntas copia = *banco;
    // Copia os vetores, pois a API pode realocar os dados ao inserir ou excluir.
    copia.capacidade_reservada = banco->quant_perguntas;
    size_t bytes_perguntas = (size_t)banco->quant_perguntas * sizeof *copia.perguntas;
    size_t bytes_ids = (size_t)banco->quant_ids_disponiveis * sizeof *copia.ids_disponiveis;
    copia.perguntas = bytes_perguntas ? malloc(bytes_perguntas) : NULL;
    copia.ids_disponiveis = bytes_ids ? malloc(bytes_ids) : NULL;

    const char *erro = "Memória insuficiente para salvar a alteração.";
    if ((!bytes_perguntas || copia.perguntas) && (!bytes_ids || copia.ids_disponiveis)) {
        if (bytes_perguntas)
            memcpy(copia.perguntas, banco->perguntas, bytes_perguntas);
        if (bytes_ids)
            memcpy(copia.ids_disponiveis, banco->ids_disponiveis, bytes_ids);

        int id = editor->pergunta.identificador;
        int resultado;
        if (editor->modo == EDITOR_NOVA)
            resultado = inserir_pergunta(&copia, &editor->pergunta);
        else if (editor->modo == EDITOR_ALTERAR)
            resultado = alterar_pergunta(&copia, id, &editor->pergunta);
        else
            resultado = excluir_pergunta(&copia, id);

        erro = "Não foi possível aplicar a alteração.";
        if (resultado == 0) {
            FILE *arquivo = NULL;
            erro = "Não foi possível gravar o banco de perguntas.";
            if (salvar_binario(&copia, &arquivo) == 0) {
                free(banco->perguntas);
                free(banco->ids_disponiveis);
                *banco = copia;
                return NULL;
            }
        }
    }

    free(copia.perguntas);
    free(copia.ids_disponiveis);
    return erro;
}

bool atualizar_editor_perguntas(EditorPerguntas *editor, Banco_perguntas *banco)
{
    if (!IsWindowFocused())
        return false;

    if (botao_clicado(cancelar)) {
        editor->modo = EDITOR_FECHADO;
        return false;
    }

    if (editor->modo != EDITOR_EXCLUIR) {
        Pergunta *pergunta = &editor->pergunta;
        char *textos[] = {pergunta->enunciado, pergunta->alternativa.A, pergunta->alternativa.B,
                          pergunta->alternativa.C, pergunta->alternativa.D};
        for (int i = 0; i < TOTAL_CAMPOS_TEXTO; i++) {
            if (botao_clicado(caixa_texto(i))) {
                editor->campo_ativo = i;
                editor->selecionar_texto = false;
            }
        }
        if (IsKeyPressed(KEY_TAB)) {
            editor->campo_ativo = (editor->campo_ativo + 1) % TOTAL_CAMPOS_TEXTO;
            editor->selecionar_texto = false;
        }

        int campo = editor->campo_ativo;
        size_t capacidade = campo == 0 ? sizeof pergunta->enunciado : sizeof pergunta->alternativa.A;
        editar_texto(textos[campo], capacidade, &editor->selecionar_texto);

        if (botao_clicado(escolher_resposta))
            pergunta->resposta = pergunta->resposta == 'D' ? 'A' : pergunta->resposta + 1;
        if (botao_clicado(escolher_dificuldade))
            pergunta->dificuldade = pergunta->dificuldade == muito_dificil ? muito_facil : pergunta->dificuldade + 1;

        if (!botao_clicado(confirmar))
            return false;
        for (int i = 0; i < TOTAL_CAMPOS_TEXTO; i++) {
            if (textos[i][strspn(textos[i], " \t\r\n")] == '\0') {
                editor->erro = "Preencha o enunciado e as quatro alternativas.";
                editor->campo_ativo = i;
                editor->selecionar_texto = false;
                return false;
            }
        }
    }
    else if (!botao_clicado(confirmar)) {
        return false;
    }

    editor->erro = gravar_mudanca(banco, editor);
    if (editor->erro != NULL)
        return false;

    editor->modo = EDITOR_FECHADO;
    return true;
}

/** @brief Mostra o final do texto quando ele fica maior que o campo. */
static void desenhar_campo(Font fonte, Rectangle area, const char *texto, bool ativo, bool selecionado)
{
    desenhar_painel(area, ativo ? COR_FACE_SELECIONADA : COR_ESCURO);
    if (ativo)
        DrawRectangleLinesEx(area, 2, COR_BORDA_SELECIONADA);

    const float recuo = 12;
    const float tamanho = 20;
    const char *visivel = texto;
    while (*visivel && MeasureTextEx(fonte, visivel, tamanho, 1).x > area.width - 2 * recuo) {
        int bytes;
        GetCodepointNext(visivel, &bytes);
        visivel += bytes;
    }
    const float y = area.y + (area.height - tamanho) / 2;
    if (selecionado) {
        float largura = MeasureTextEx(fonte, visivel, tamanho, 1).x;
        DrawRectangleRec((Rectangle){area.x + recuo, y, largura, tamanho}, COR_BORDA_SELECIONADA);
    }
    DrawTextEx(fonte, visivel, (Vector2){area.x + recuo, y}, tamanho, 1, selecionado ? COR_ESCURO : COR_TEXTO_CLARO);
}

void desenhar_editor_perguntas(const EditorPerguntas *editor, Font fonte, Font titulo)
{
    const Pergunta *pergunta = &editor->pergunta;
    const char *nome = editor->modo == EDITOR_NOVA ? "Nova pergunta" : "Editar pergunta";
    if (editor->modo == EDITOR_EXCLUIR)
        nome = "Excluir pergunta";
    DrawTextEx(titulo, nome, (Vector2){24, 16}, 34, 1, COR_TEXTO_CLARO);

    if (editor->modo == EDITOR_EXCLUIR) {
        desenhar_painel((Rectangle){24, 78, 912, 106}, COR_FACE_BOTAO);
        DrawTextEx(fonte, TextFormat("Confirma excluir a pergunta %03d?", pergunta->identificador),
                   (Vector2){48, 99}, 24, 1, COR_TEXTO_CLARO);
    }
    else {
        const char *rotulos[] = {"Enunciado", "Alternativa A", "Alternativa B", "Alternativa C", "Alternativa D"};
        const char *textos[] = {pergunta->enunciado, pergunta->alternativa.A, pergunta->alternativa.B,
                                pergunta->alternativa.C, pergunta->alternativa.D};
        for (int i = 0; i < TOTAL_CAMPOS_TEXTO; i++) {
            Rectangle area = caixa_texto(i);
            bool ativo = editor->campo_ativo == i;
            DrawTextEx(fonte, rotulos[i], (Vector2){44, area.y + 12}, 20, 1, COR_TEXTO_CLARO);
            desenhar_campo(fonte, area, textos[i], ativo, ativo && editor->selecionar_texto);
        }
        DrawTextEx(fonte, "Resposta certa", (Vector2){44, 384}, 20, 1, COR_TEXTO_CLARO);
        DrawTextEx(fonte, "Dificuldade", (Vector2){490, 384}, 20, 1, COR_TEXTO_CLARO);
        desenhar_botao(fonte, escolher_resposta, TextFormat("Alternativa %c", pergunta->resposta));
        desenhar_botao(fonte, escolher_dificuldade, nomes_dificuldade[pergunta->dificuldade]);
    }

    if (editor->erro != NULL)
        DrawTextEx(fonte, editor->erro, (Vector2){24, 425}, 20, 1, COR_TEXTO_CLARO);
    desenhar_botao(fonte, cancelar, "Cancelar");
    desenhar_botao_com_estado(fonte, confirmar, editor->modo == EDITOR_EXCLUIR ? "Excluir" : "Salvar",
                             BOTAO_PRIMARIO, true);
}
