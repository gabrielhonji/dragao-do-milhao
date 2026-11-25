/**
 * @file tela_transicao.h
 * @brief Declara as cenas que apresentam a Jornada e a Batalha.
 */

#ifndef TELA_TRANSICAO_H
#define TELA_TRANSICAO_H

#include <raylib.h>

typedef enum
{
    TRANSICAO_ABERTURA,
    TRANSICAO_CHEGADA,
} TransicaoPartida;

typedef enum
{
    TRANSICAO_AGUARDAR,
    TRANSICAO_AVANCAR,
    TRANSICAO_VOLTAR,
} AcaoTransicao;

/**
 * @brief Lê a mesma área usada no desenho e aceita Enter para avançar.
 * @param transicao Cena atual. O botão Voltar fica disponível na abertura.
 * @param confirmou Indica se Enter foi pressionado neste quadro.
 * @return Ação escolhida ou TRANSICAO_AGUARDAR quando nenhuma ação foi acionada.
 */
AcaoTransicao ler_acao_transicao(TransicaoPartida transicao, bool confirmou);

/**
 * @brief Exibe a carta da princesa ou a chegada ao pátio do castelo.
 * @param transicao Cena que define a arte e a mensagem.
 * @param fonte Fonte da mensagem, da orientação e dos botões.
 * @param fonte_titulo Fonte do título.
 * @param mapa Área reservada à arte da cena.
 * @param painel Área reservada à mensagem e à orientação da etapa.
 */
void desenhar_transicao(TransicaoPartida transicao, Font fonte, Font fonte_titulo, Rectangle mapa, Rectangle painel);

#endif
