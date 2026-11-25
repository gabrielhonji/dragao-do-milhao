/**
 * @file tela_transicao.c
 * @brief Desenha as apresentações do resgate e do confronto no castelo.
 */

#include "tela_transicao.h"
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

static const Rectangle voltar = {544, 464, 164, 52};
static const Rectangle avancar = {720, 464, 216, 52};

AcaoTransicao ler_acao_transicao(TransicaoPartida transicao, bool confirmou)
{
    if (transicao == TRANSICAO_ABERTURA && botao_clicado(voltar))
        return TRANSICAO_VOLTAR;
    if (confirmou || botao_clicado(avancar))
        return TRANSICAO_AVANCAR;
    return TRANSICAO_AGUARDAR;
}

void desenhar_transicao(TransicaoPartida transicao, Font fonte, Font fonte_titulo, Rectangle mapa, Rectangle painel)
{
    bool abertura = transicao == TRANSICAO_ABERTURA;
    const char *titulo = abertura ? "Um pedido de resgate" : "Chegada ao castelo";
    const char *capitulo = abertura ? "Prólogo" : "Etapa 2 de 2";
    const char *texto = abertura
                            ? "Dragão, o príncipe me trancou nesta torre e espalhou que você era o monstro. Preciso da sua ajuda."
                            : "O dragão chegou até o pátio. O príncipe bloqueia o caminho para a torre.";
    const char *orientacao = abertura
                                 ? "Responda às perguntas para seguir pelo vale. Você pode pedir ajuda durante o caminho."
                                 : "Cada resposta certa enfraquece o príncipe. Acerte até zerar os PV e libertar a princesa.";

    DrawTextEx(fonte_titulo, titulo, (Vector2){mapa.x, 25}, 38, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, capitulo, (Vector2){painel.x + 16, 54}, 20, 1, COR_DICA_CARTA);
    desenhar_painel(mapa, COR_ESCURO);
    desenhar_arte_quiz(abertura ? CENA_ABERTURA : CENA_CHEGADA, 0);
    desenhar_painel(painel, COR_ESCURO);

    const float recuo = 16; // Margem interna compartilhada pelos textos e pela divisória.
    float x = painel.x + recuo;
    float largura = painel.width - 2 * recuo;
    DrawTextEx(fonte, abertura ? "Carta da princesa" : "Batalha", (Vector2){x, painel.y + 20}, 25, 1, COR_DICA_CARTA);
    desenhar_texto_ajustado(fonte, (Rectangle){x, painel.y + 65, largura, 108}, texto, 23, COR_TEXTO_CLARO, false);
    DrawRectangleRec((Rectangle){x, painel.y + 180, largura, 2}, COR_MOLDURA_MAPA);
    desenhar_texto_ajustado(fonte, (Rectangle){x, painel.y + 204, largura, 102}, orientacao, 20, COR_TEXTO_CLARO, false);

    if (abertura)
        desenhar_botao(fonte, voltar, "Voltar");
    desenhar_botao_com_estado(fonte, avancar, abertura ? "Iniciar jornada" : "Enfrentar príncipe", BOTAO_PRIMARIO, true);
}
