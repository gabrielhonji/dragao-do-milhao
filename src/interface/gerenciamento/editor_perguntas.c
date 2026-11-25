/**
 * @file editor_perguntas.c
 * @brief Cadastro, edição e exclusão de perguntas na janela do jogo.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor_perguntas.h"
#include "../comum/audio.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"
#include "../../perguntas/arquivos_perguntas.h"

enum
{
    TOTAL_CAMPOS_TEXTO = 5
};

static const Rectangle cancelar = {24, 464, 200, 52};
static const Rectangle confirmar = {736, 464, 200, 52};
static const Rectangle escolher_resposta = {220, 372, 260, 44};
static const Rectangle escolher_dificuldade = {630, 372, 266, 44};
static const char *nomes_dificuldade[] = {"", "Muito fácil", "Fácil", "Média", "Difícil", "Muito difícil"};

void abrir_editor_perguntas(EditorPerguntas *editor, ModoEditorPerguntas modo, const Pergunta *pergunta)
{
    // Reinicia foco, seleção e mensagem a cada abertura do editor.
    *editor = (EditorPerguntas){.modo = modo};
    if (pergunta != NULL) {
        // A cópia mantém o ID para localizar a mesma pergunta ao salvar ou excluir.
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
 * @brief Aplica uma operação a uma cópia do banco e só a adota depois de gravá-la.
 * @param banco Banco em uso na listagem e no jogo.
 * @param editor Operação escolhida e pergunta de trabalho.
 * @return Texto do erro ou NULL em caso de sucesso.
 */
static const char *gravar_mudanca(Banco_perguntas *banco, EditorPerguntas *editor)
{
    Banco_perguntas copia = *banco;
    // A atribuição acima copia os ponteiros. Alocamos os vetores da cópia para que
    // a API trabalhe em memória própria, inclusive quando precisa realocar.
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
            // Depois de salvar, o banco passa a ser dono dos vetores da cópia.
            if (salvar_binario(&copia, &arquivo) == 0) {
                free(banco->perguntas);
                free(banco->ids_disponiveis);
                *banco = copia;
                return NULL;
            }
        }
    }

    // Em caso de falha, libera a cópia e deixa o rascunho aberto para tentar novamente.
    free(copia.perguntas);
    free(copia.ids_disponiveis);
    return erro;
}

bool atualizar_editor_perguntas(EditorPerguntas *editor, Banco_perguntas *banco)
{
    if (!IsWindowFocused())
        return false;

    if (botao_clicado(cancelar)) {
        tocar_efeito(SOM_BOTAO);
        // A lista continua com os dados gravados porque a edição usa um rascunho.
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
            // O resto da divisão volta ao enunciado depois da alternativa D.
            editor->campo_ativo = (editor->campo_ativo + 1) % TOTAL_CAMPOS_TEXTO;
            editor->selecionar_texto = false;
        }

        int campo = editor->campo_ativo;
        size_t capacidade = campo == 0 ? sizeof pergunta->enunciado : sizeof pergunta->alternativa.A;
        editar_texto(textos[campo], capacidade, &editor->selecionar_texto);

        if (botao_clicado(escolher_resposta)) {
            pergunta->resposta = pergunta->resposta == 'D' ? 'A' : pergunta->resposta + 1;
            tocar_efeito(SOM_BOTAO);
        }
        if (botao_clicado(escolher_dificuldade)) {
            pergunta->dificuldade = pergunta->dificuldade == muito_dificil ? muito_facil : pergunta->dificuldade + 1;
            tocar_efeito(SOM_BOTAO);
        }

        if (!botao_clicado(confirmar))
            return false;
        for (int i = 0; i < TOTAL_CAMPOS_TEXTO; i++) {
            // strspn pula os espaços iniciais. Chegar ao '\0' aqui indica um campo em branco.
            if (textos[i][strspn(textos[i], " \t\r\n")] == '\0') {
                editor->erro = "Preencha o enunciado e as quatro alternativas.";
                editor->campo_ativo = i;
                editor->selecionar_texto = false;
                tocar_efeito(SOM_ERRO);
                return false;
            }
        }
    }
    // Na exclusão, aguarda o clique de confirmação antes de chamar o banco.
    else if (!botao_clicado(confirmar)) {
        return false;
    }

    editor->erro = gravar_mudanca(banco, editor);
    tocar_efeito(editor->erro ? SOM_ERRO : SOM_BOTAO);
    if (editor->erro != NULL)
        return false;

    editor->modo = EDITOR_FECHADO;
    return true;
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
    bool excluindo = editor->modo == EDITOR_EXCLUIR;
    desenhar_botao_com_estado(fonte, confirmar, excluindo ? "Excluir" : "Salvar",
                             excluindo ? BOTAO_SAIDA : BOTAO_PRIMARIO, true);
}
