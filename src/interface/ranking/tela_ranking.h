/**
 * @file tela_ranking.h
 * @brief Carrega e apresenta os resultados do ranking.
 */

#ifndef TELA_RANKING_H
#define TELA_RANKING_H

#include <raylib.h>
#include "../../quiz/arquivo_partidas.h"

typedef struct
{
    Registro_partida *registros; // Vetor alocado ao abrir o ranking.
    int total;                  // Quantidade ou erro devolvido pela leitura.
} TelaRanking;

/** @brief Carrega a tabela ordenada quando o jogador entra na tela. */
void carregar_ranking(TelaRanking *ranking);

/** @brief Libera o vetor que foi carregado para a tela. */
void liberar_ranking(TelaRanking *ranking);

/** @brief Desenha a tabela, o aviso de leitura e o botão Voltar. */
void desenhar_ranking(const TelaRanking *ranking, Font fonte, Font fonte_titulo, Rectangle voltar);

#endif
