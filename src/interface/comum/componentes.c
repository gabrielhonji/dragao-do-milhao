/**
 * @file componentes.c
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Implementa operações reutilizáveis para os componentes da interface gráfica.
 * @version 0.1
 * @date 2026-11-18
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <string.h>
#include "componentes.h"
#include "cores.h"

/** @brief Verifica se o cursor está dentro do botão e se o jogo está em foco. */
static bool mouse_sobre_botao(Rectangle area)
{
    return IsWindowFocused() && CheckCollisionPointRec(GetMousePosition(), area);
}

bool botao_clicado(Rectangle area)
{
    return mouse_sobre_botao(area) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/**
 * @brief Sobrepõe duas faixas para deixar um pequeno degrau em cada canto.
 * @param area Limites externos da forma.
 * @param cor Cor de preenchimento.
 */
static void preencher_face(Rectangle area, Color cor)
{
    const float borda_arredondada = 3; // Tamanho do quadradinho que fica vazio em cada canto.

    DrawRectangleRec((Rectangle){area.x + borda_arredondada, area.y, area.width - 2 * borda_arredondada, area.height}, cor);
    DrawRectangleRec((Rectangle){area.x, area.y + borda_arredondada, area.width, area.height - 2 * borda_arredondada}, cor);
}

void desenhar_painel(Rectangle area, Color fundo)
{
    const float espessura = 2; // Largura da moldura em pixels da tela lógica.
    // O interior começa uma espessura para dentro e perde essa margem em cada lado.
    Rectangle interior = {area.x + espessura, area.y + espessura, area.width - 2 * espessura, area.height - 2 * espessura};
    preencher_face(area, COR_MOLDURA_MAPA);
    preencher_face(interior, fundo);
}

/**
 * @brief Quebra no último espaço que cabe, ou entre caracteres em palavras muito longas.
 * @param linhas Destino com espaço para o dobro dos bytes do texto mais o terminador.
 */
static void quebrar_texto(Font fonte, const char *texto, char *linhas, int tamanho, float largura)
{
    char *fim = linhas;
    char *inicio = linhas;
    char *espaco = NULL;

    while (*texto != '\0') {
        int bytes;
        int codigo = GetCodepointNext(texto, &bytes);
        if (codigo == '\r' || codigo == '\n') {
            *fim++ = '\n';
            inicio = fim;
            espaco = NULL;
            texto += bytes;
            if (codigo == '\r' && *texto == '\n')
                texto++;
            continue;
        }

        char *posicao = fim;
        memcpy(fim, texto, (size_t)bytes);
        texto += bytes;
        fim += bytes;
        *fim = '\0';
        if (codigo == ' ')
            espaco = posicao;
        if (posicao == inicio || MeasureTextEx(fonte, inicio, tamanho, 1).x <= largura)
            continue;

        if (espaco != NULL && espaco >= inicio) {
            *espaco = '\n';
            inicio = espaco + 1;
            espaco = NULL;
        }
        // Se a palavra após o último espaço também ultrapassa a largura, quebra antes da letra atual.
        if (posicao > inicio && MeasureTextEx(fonte, inicio, tamanho, 1).x > largura) {
            memmove(posicao + 1, posicao, (size_t)bytes + 1);
            *posicao = '\n';
            inicio = posicao + 1;
            fim++;
        }
    }
    *fim = '\0';
}

void desenhar_texto_ajustado(Font fonte, Rectangle area, const char *texto, int tamanho, Color cor, bool centralizar_verticalmente)
{
    char linhas[2 * strlen(texto) + 1]; // Cada caractere pode acrescentar no máximo uma quebra.
    const char *visivel = texto;
    Vector2 medida = MeasureTextEx(fonte, texto, tamanho, 1);

    if (medida.x > area.width || medida.y > area.height || strchr(texto, '\r') != NULL) {
        visivel = linhas;
        for (;;) {
            quebrar_texto(fonte, texto, linhas, tamanho, area.width);
            medida = MeasureTextEx(fonte, linhas, tamanho, 1);
            if ((medida.x <= area.width && medida.y <= area.height) || tamanho <= 1)
                break;
            tamanho--;
        }
    }

    float y = centralizar_verticalmente ? area.y + (area.height - medida.y) / 2 : area.y;
    DrawTextEx(fonte, visivel, (Vector2){area.x, y}, tamanho, 1, cor);
}

/* Desenhar botões */

/** @brief Desenha as camadas comuns aos botões de ação e às alternativas do quiz. */
static void desenhar_corpo_botao(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo,
                                bool habilitado, bool alinhar_esquerda, float reserva_direita)
{
    Color cor_face = COR_FACE_BOTAO;     // Superfície onde aparece o texto.
    Color cor_aresta = COR_ARESTA_BOTAO; // Faixa inferior que dá espessura ao botão.
    Color cor_borda = COR_BORDA_BOTAO;   // Contorno externo, visível em volta da face.
    Color cor_texto = COR_TEXTO_CLARO;   // Cor das letras sobre a face.

    // Resultados e o fogo permanecem visíveis mesmo em opções que não recebem cliques.
    if (estilo == BOTAO_ACERTO || estilo == BOTAO_ERRO) {
        cor_face = estilo == BOTAO_ACERTO ? COR_FACE_ACERTO : COR_FACE_ERRO;
        cor_borda = estilo == BOTAO_ACERTO ? COR_BORDA_ACERTO : COR_BORDA_ERRO;
        cor_aresta = cor_face;
    }
    else if (estilo == BOTAO_SELECIONADO) {
        cor_face = COR_FACE_SELECIONADA;
        cor_borda = COR_BORDA_SELECIONADA;
    }
    else if (estilo == BOTAO_SAIDA) {
        cor_face = habilitado ? COR_FACE_SAIDA : COR_FACE_BLOQUEADA;
        cor_borda = habilitado ? COR_BORDA_SAIDA : COR_BORDA_SAIDA_INATIVA;
        cor_aresta = COR_FACE_ERRO;
        if (!habilitado)
            cor_texto = COR_TEXTO_BLOQUEADO;
    }
    else if (estilo == BOTAO_DICA_FOGO) {
        cor_face = COR_FACE_FOGO;
        cor_borda = COR_ARESTA_PRIMARIA;
        cor_texto = COR_TEXTO_BLOQUEADO;
        cor_aresta = cor_face;
    }
    else if (estilo == BOTAO_DICA_BURRO || estilo == BOTAO_DICA_CARTA) {
        cor_face = estilo == BOTAO_DICA_BURRO ? COR_FACE_BURRO : COR_FACE_CARTA;
        cor_borda = estilo == BOTAO_DICA_BURRO ? COR_DICA_BURRO : COR_DICA_CARTA;
        cor_aresta = cor_face;
    }
    else if (!habilitado) {
        cor_face = COR_FACE_BLOQUEADA;
        cor_texto = COR_TEXTO_BLOQUEADO;
    }
    else if (estilo == BOTAO_PRIMARIO) {
        cor_face = COR_FACE_PRIMARIA;
        cor_borda = COR_BORDA_PRIMARIA;
        cor_texto = COR_ESCURO;
        cor_aresta = COR_ARESTA_PRIMARIA;
    }

    // O hover dura só enquanto o cursor está sobre o botão. A escolha mantém a face escura.
    if (habilitado && mouse_sobre_botao(area)) {
        cor_borda = COR_TEXTO_CLARO;
        if (estilo != BOTAO_SELECIONADO)
            cor_face = ColorBrightness(cor_face, 0.2f);
    }

    const float espessura = 2; // Largura do contorno e altura da aresta, em pixels do jogo.
    // X cresce para a direita e Y para baixo. Entramos pela esquerda e pelo topo;
    // tiramos duas espessuras de cada dimensão para sobrar borda também à direita e embaixo.
    Rectangle interior = {area.x + espessura, area.y + espessura, area.width - 2 * espessura, area.height - 2 * espessura};
    preencher_face(area, cor_borda);
    preencher_face(interior, cor_face);

    const float recuo_aresta = 4; // Encurta a faixa nas duas pontas para poupar os cantos recortados.
    // O limite inferior é interior.y + interior.height. Subir uma espessura mantém a faixa dentro da face.
    DrawRectangleRec((Rectangle){area.x + recuo_aresta, interior.y + interior.height - espessura, area.width - 2 * recuo_aresta, espessura}, cor_aresta);

    const int tamanho = 20; // Altura de fonte escolhida para os rótulos dos botões.
    if (alinhar_esquerda) {
        // Reserva 12 px de cada lado e 4 px acima e abaixo do texto.
        Rectangle texto = {area.x + 12, area.y + 4, area.width - 24 - reserva_direita, area.height - 8};
        desenhar_texto_ajustado(fonte, texto, rotulo, tamanho, cor_texto, true);
        return;
    }

    Vector2 medida = MeasureTextEx(fonte, rotulo, tamanho, 1);
    Vector2 posicao = {area.x + (area.width - medida.x) / 2, area.y + (area.height - medida.y) / 2};
    DrawTextEx(fonte, rotulo, posicao, tamanho, 1, cor_texto);
}

void desenhar_botao_com_estado(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo, bool habilitado)
{
    desenhar_corpo_botao(fonte, area, rotulo, estilo, habilitado, false, 0);
}

void desenhar_alternativa(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo, bool habilitada, float reserva_direita)
{
    desenhar_corpo_botao(fonte, area, rotulo, estilo, habilitada, true, reserva_direita);
}

void desenhar_botao(Font fonte, Rectangle area, const char *rotulo)
{
    desenhar_botao_com_estado(fonte, area, rotulo, BOTAO_SECUNDARIO, true);
}

/* Campos de texto */
void editar_texto(char *texto, size_t capacidade, bool *selecionar_tudo)
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
            // Volta pelos bytes de continuação (10xxxxxx) até o começo do caractere.
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
        // Reserva espaço para todos os bytes da letra e para o '\0'.
        if (tamanho + (size_t)bytes >= capacidade)
            continue;

        memcpy(texto + tamanho, caractere, (size_t)bytes);
        tamanho += (size_t)bytes;
        texto[tamanho] = '\0';
    }
}

void desenhar_campo(Font fonte, Rectangle area, const char *texto, bool ativo, bool selecionado)
{
    desenhar_painel(area, ativo ? COR_FACE_SELECIONADA : COR_ESCURO);
    if (ativo)
        DrawRectangleLinesEx(area, 2, COR_BORDA_SELECIONADA);

    const float recuo = 12;
    const float tamanho = 20;
    const char *visivel = texto;
    // Avança só o início do trecho visível, para manter o fim da digitação dentro da caixa.
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

void desenhar_previa_texto(Font fonte, const char *texto, Vector2 posicao, float largura)
{
    const float tamanho = 18;
    const float espacamento = 1;
    // strpbrk procura em texto o primeiro '\r' ou '\n' e retorna seu endereço, ou NULL se não encontrar.
    // '\r' representa o retorno ao início da linha e aparece antes de '\n' em quebras CRLF.
    if (strpbrk(texto, "\r\n") == NULL && MeasureTextEx(fonte, texto, tamanho, espacamento).x <= largura) {
        DrawTextEx(fonte, texto, posicao, tamanho, espacamento, COR_TEXTO_CLARO);
        return;
    }

    const char *fim_previa = texto;

    // Como na quebra de linha do quiz, só mede nos separadores entre palavras.
    for (const char *fim_palavra = texto; *fim_palavra != '\0'; fim_palavra++) {
        char separador = *fim_palavra;
        bool quebra_linha = separador == '\n' || separador == '\r';
        if (separador != ' ' && !quebra_linha)
            continue;

        // Em %.*s, % abre a conversão, . indica um limite, * lê esse limite de um int e s indica uma string.
        // A diferença entre os ponteiros dá o limite em bytes. Os três pontos finais são texto literal.
        const char *trecho = TextFormat("%.*s...", (int)(fim_palavra - texto), texto);
        if (MeasureTextEx(fonte, trecho, tamanho, espacamento).x > largura)
            break;

        fim_previa = fim_palavra;
        if (quebra_linha)
            break; // A prévia termina antes da quebra já presente no enunciado.
    }

    const char *previa = TextFormat("%.*s...", (int)(fim_previa - texto), texto);
    DrawTextEx(fonte, previa, posicao, tamanho, espacamento, COR_TEXTO_CLARO);
}
