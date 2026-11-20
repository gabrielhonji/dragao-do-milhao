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
    return CheckCollisionPointRec(GetMousePosition(), area) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void desenhar_botao(Font fonte, Rectangle area, const char *rotulo)
{
    const float tamanho = 24.0f;
    const float espacamento = 1.0f;
    bool mouse_sobre_botao = CheckCollisionPointRec(GetMousePosition(), area);

    Color fundo = mouse_sobre_botao ? GRAY : LIGHTGRAY;
    Vector2 medida_rotulo = MeasureTextEx(fonte, rotulo, tamanho, espacamento);

    // Divide o espaço livre entre os dois lados de cada eixo.
    Vector2 posicao_rotulo = {area.x + (area.width - medida_rotulo.x) / 2.0f,
                              area.y + (area.height - medida_rotulo.y) / 2.0f};

    DrawRectangleRec(area, fundo);
    DrawRectangleLinesEx(area, 2.0f, DARKGRAY); // O 'Ex' vem de 'extented', uma versão plus da função padrão, na fonte ele permite usar fontes externas, nesse do rec ele permite definir uma espessura para a borda
    DrawTextEx(fonte, rotulo, posicao_rotulo, tamanho, espacamento, BLACK);
}
