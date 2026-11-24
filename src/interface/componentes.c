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

/**
 * @brief Preenche um retângulo com um pequeno "degrau" em cada canto, uma maneira absoluta de fazer um border_radius.
 * @param area Limites externos do container.
 * @param cor Cor de preenchimento.
 */
static void preencher_face(Rectangle area, Color cor)
{
    const float borda_arredondada = 3; // Recorta 3px de border_radius.

    DrawRectangleRec((Rectangle){area.x + borda_arredondada, area.y, area.width - 2 * borda_arredondada, area.height}, cor);
    DrawRectangleRec((Rectangle){area.x, area.y + borda_arredondada, area.width, area.height - 2 * borda_arredondada}, cor);
}

void desenhar_botao_com_estado(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo, bool habilitado)
{
    // Importa as cores pelo prefixo "0x" que converte essas entradas para valores hexadecimais
    Color cor_face = GetColor(0x2936AFF); //
    Color cor_aresta = GetColor(0x364B46FF); //
    Color cor_borda = GetColor(0x51675DFF);
    Color cor_texto = GetColor(0xF5EBD8FF);

    // Mantem a escolha visível mesmo depois de confirmar a resposta.
    if (estilo == BOTAO_SELECIONADO) {
        cor_face = GetColor(0x142C2AFF);
        cor_borda = GetColor(0x9FCFBFFF);
    }
    else if (!habilitado) {
        cor_face = GetColor(0x252D30FF);
        cor_texto = GetColor(0x96A39AFF);
    }
    else if (estilo == BOTAO_PRIMARIO) {
        cor_face = GetColor(0xACD1ABFF);
        cor_borda = GetColor(0xC7DEC0FF);
        cor_texto = GetColor(0x10181CFF);
        cor_aresta = GetColor(0x74998AFF);
    }

   const float espessura = 2; // Para as bordas e arestas, com 2px de espessura.
   Rectangle interior = {area.x + espessura, area.y + espessura, area.width - 2 * espessura, area.height - 2 * espessura}; //

   preencher_face(area, cor_borda);
   preencher_face(interior, cor_face);

   const float recuo_aresta = 4; // A linha termina antes de chegar no border_radius.
   DrawRectangleRec((Rectangle){area.x + recuo_aresta, interior.y + interior.height - espessura, area.width - 2 * recuo_aresta, espessura}, cor_aresta); //

   const float tamanho = 20; //
   const float recuo_cor_texto = 12; // Distância do rótulo à borda esquerda.
   Vector2 medida = MeasureTextEx(fonte, rotulo, tamanho, 1);
   Vector2 posicao = {area.x + recuo_cor_texto, area.y + (area.height - medida.y) / 2};
   DrawTextEx(fonte, rotulo, posicao, tamanho, 1, cor_texto);
}

void desenhar_botao(Font fonte, Rectangle area, const char *rotulo)
{
    desenhar_botao_com_estado(fonte, area, rotulo, BOTAO_SECUNDARIO, true);
}
