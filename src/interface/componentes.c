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

#include "componentes.h"

bool botao_clicado(Rectangle area)
{
    return IsWindowFocused() && CheckCollisionPointRec(GetMousePosition(), area) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void desenhar_botao_com_estado(Font fonte, Rectangle area, const char *rotulo, bool selecionado, bool habilitado)
{
    const float tamanho = 24.0f;
    const float espacamento = 1.0f;
    bool mouse_sobre_botao = habilitado && IsWindowFocused() && CheckCollisionPointRec(GetMousePosition(), area);

    Color fundo = habilitado ? LIGHTGRAY : RAYWHITE;
    Color cor_rotulo = habilitado ? BLACK : GRAY;
    Color cor_borda = habilitado || selecionado ? DARKGRAY : GRAY;

    if (mouse_sobre_botao)
        fundo = GRAY;

    // Mantém o estilo da seleção mesmo quando o mouse sai ou a resposta é confirmada.
    if (selecionado) {
        fundo = DARKGRAY;
        cor_rotulo = RAYWHITE;
    }

    Vector2 medida_rotulo = MeasureTextEx(fonte, rotulo, tamanho, espacamento);
    // Divide o espaço livre entre os dois lados de cada eixo.
    Vector2 posicao_rotulo = {area.x + (area.width - medida_rotulo.x) / 2.0f,
                              area.y + (area.height - medida_rotulo.y) / 2.0f};

    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, 2.0f, cor_borda);
    DrawTextEx(fonte, rotulo, posicao_rotulo, tamanho, espacamento, cor_rotulo);
}

void desenhar_botao(Font fonte, Rectangle area, const char *rotulo)
{
    desenhar_botao_com_estado(fonte, area, rotulo, false, true);
}
