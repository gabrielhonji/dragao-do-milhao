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

/** @brief Lê a mesma área usada no desenho e aceita Enter para avançar. */
AcaoTransicao ler_acao_transicao(TransicaoPartida transicao, bool confirmou);

/** @brief Exibe a carta da princesa ou a chegada ao pátio do castelo. */
void desenhar_transicao(TransicaoPartida transicao, Font fonte, Font fonte_titulo, Rectangle mapa, Rectangle painel);

#endif
