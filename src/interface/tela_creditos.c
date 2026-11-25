/**
 * @file tela_creditos.c
 * @brief Desenha os créditos e o painel da equipe.
 */

#include "tela_creditos.h"
#include "arte_cenas.h"
#include "componentes.h"
#include "cores.h"

void desenhar_creditos(Font fonte, Font fonte_titulo, Rectangle mapa, Rectangle painel_equipe, Rectangle voltar)
{
    const float recuo = 18;      // Margem do texto dentro do painel da equipe.
    const float passo_nome = 30; // Distância entre o topo de dois nomes consecutivos.
    const float x_texto = painel_equipe.x + recuo;

    DrawTextEx(fonte_titulo, "Créditos", (Vector2){mapa.x, 13}, 34, 1, COR_TEXTO_CLARO);

    // Pinta o fundo e a borda antes do mapa. O véu escurece o mapa, e o dragão desenhado depois mantém suas cores.
    desenhar_painel(mapa, COR_ESCURO);
    desenhar_arte_creditos();

    // Painel da equipe.
    desenhar_painel(painel_equipe, COR_FACE_BOTAO);

    Vector2 posicao_equipe = {x_texto, painel_equipe.y + recuo};
    DrawTextEx(fonte_titulo, "Sete Copas", posicao_equipe, 32, 1, COR_TEXTO_CLARO);

    // A divisória fica 61 px abaixo do topo e respeita o recuo dos dois lados.
    Rectangle divisoria = {x_texto, painel_equipe.y + 61,
                           painel_equipe.width - 2 * recuo, 2};
    DrawRectangleRec(divisoria, COR_MOLDURA_MAPA);

    const char *membros[] = {
        "Enzo Andreassa Pizza",
        "Gabriel Yudi Honji",
        "Murilo Henrique Alves Martins",
    };

    // O primeiro nome começa 80 px abaixo do topo, depois da divisória.
    float y_nome = painel_equipe.y + 80;
    for (int i = 0; i < 3; i++) {
        DrawTextEx(fonte, membros[i], (Vector2){x_texto, y_nome}, 21, 1, COR_TEXTO_CLARO);
        y_nome += passo_nome;
    }

    const float tamanho_rodape = 19; // Altura da fonte usada nas duas linhas.
    const float passo_rodape = 24;   // Distância entre os topos das linhas, com 5 px de folga.
    // DrawTextEx recebe o topo do texto. Subir uma altura de fonte deixa a margem livre abaixo da disciplina.
    const float y_disciplina = painel_equipe.y + painel_equipe.height - recuo - tamanho_rodape;
    DrawTextEx(fonte, "UTFPR Apucarana", (Vector2){x_texto, y_disciplina - passo_rodape}, tamanho_rodape, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, "Fundamentos de Programação 2", (Vector2){x_texto, y_disciplina}, tamanho_rodape, 1, COR_TEXTO_CLARO);

    desenhar_botao(fonte, voltar, "Voltar");
}
