/**
 * @file tela_eco.h
 * @brief Lê os controles e exibe a rodada do Eco da Torre.
 */

#ifndef TELA_ECO_H
#define TELA_ECO_H

#include <raylib.h>
#include "../../minigames/minigame_flappybird/minigame_flappybird.h"

/**
 * @brief Atualiza o voo ou lê o clique de retorno após o resultado.
 * @param jogo Estado da rodada.
 * @param janela_em_foco Permite avançar a simulação e receber Espaço enquanto a janela está em foco.
 * @return Verdadeiro quando o jogador pede para voltar à pergunta.
 */
bool atualizar_tela_eco(FlappyBird *jogo, bool janela_em_foco);

/**
 * @brief Desenha a pomba, o percurso e o resultado.
 * @param jogo Estado usado para posicionar os elementos.
 * @param fonte Fonte do placar e das instruções.
 * @param fonte_titulo Fonte usada no título.
 * @param tamanho_corpo Tamanho lógico do texto comum.
 * @param tamanho_titulo Tamanho lógico do título.
 */
void desenhar_eco(const FlappyBird *jogo, Font fonte, Font fonte_titulo, int tamanho_corpo, int tamanho_titulo);

#endif
