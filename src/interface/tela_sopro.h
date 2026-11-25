/**
 * @file tela_sopro.h
 * @brief Atualiza e desenha o desafio do Sopro Ancestral.
 */

#ifndef TELA_SOPRO_H
#define TELA_SOPRO_H

#include <raylib.h>
#include "../minigames/minigame_sopro/minigame_sopro.h"

/**
 * @brief Avança o desafio ou lê o botão de voltar quando a rodada termina.
 * @param sopro Estado do desafio atual.
 * @param janela_em_foco Indica se a partida pode receber entrada do teclado.
 * @return Verdadeiro quando o jogador pede para voltar à pergunta.
 */
bool atualizar_tela_sopro(MinigameSopro *sopro, bool janela_em_foco);

/** @brief Desenha a cena, o tempo, a carga do sopro e o resultado na área lógica do jogo. */
void desenhar_sopro(const MinigameSopro *sopro, Font fonte, Font fonte_titulo,
                    int tamanho_corpo, int tamanho_titulo, int largura, int altura);

#endif
