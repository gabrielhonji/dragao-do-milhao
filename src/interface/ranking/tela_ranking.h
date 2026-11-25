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

/**
 * @brief Carrega a tabela ordenada quando o jogador entra na tela.
 * @param ranking Estado com o vetor inicialmente vazio. Recebe os registros e a contagem ou o erro de leitura.
 */
void carregar_ranking(TelaRanking *ranking);

/**
 * @brief Libera o vetor que foi carregado para a tela.
 * @param ranking Estado que possui o vetor de registros.
 */
void liberar_ranking(TelaRanking *ranking);

/**
 * @brief Desenha a tabela, o aviso de leitura e o botão Voltar.
 * @param ranking Estado com a quantidade e os registros ordenados. A tela exibe até oito resultados.
 * @param fonte Fonte da tabela, do aviso e do botão.
 * @param fonte_titulo Fonte do título.
 * @param voltar Área do botão Voltar.
 */
void desenhar_ranking(const TelaRanking *ranking, Font fonte, Font fonte_titulo, Rectangle voltar);

#endif
