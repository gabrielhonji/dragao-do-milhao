/**
 * @file tela_burro.h
 * @brief Atualiza e desenha a tela do Trote Certeiro.
 */

#ifndef TELA_BURRO_H
#define TELA_BURRO_H

#include <raylib.h>
#include "../../minigames/minigame_burro/minigame_burro.h"

/**
 * @brief Atualiza as cartas ou lê o botão de voltar após o resultado.
 * @param burro Estado da rodada atual.
 * @return Verdadeiro quando o jogador pede para voltar à pergunta.
 */
bool atualizar_tela_burro(MinigameBurro *burro);

/**
 * @brief Desenha o tabuleiro, os contadores e o resultado da rodada.
 * @param burro Estado das cartas, chances e resultado da rodada.
 * @param fonte Fonte das instruções e dos contadores.
 * @param fonte_titulo Fonte do título.
 * @param tamanho_corpo Tamanho lógico do texto comum.
 * @param tamanho_titulo Tamanho lógico do título.
 */
void desenhar_burro(const MinigameBurro *burro, Font fonte, Font fonte_titulo, int tamanho_corpo, int tamanho_titulo);

#endif
