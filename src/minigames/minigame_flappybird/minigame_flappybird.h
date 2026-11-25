/**
 * @file minigame_flappybird.h
 * @brief Declara o estado e as regras do Eco da Torre.
 */

#ifndef MINIGAME_FLAPPYBIRD_H
#define MINIGAME_FLAPPYBIRD_H

#include <stdbool.h>

// O desenho usa as mesmas medidas que o cálculo de colisão.
enum
{
    ECO_LARGURA = 640,
    ECO_ALTURA = 336,
    ECO_RAIO_POMBA = 10,
    ECO_LARGURA_OBSTACULO = 46,
    ECO_TAMANHO_ABERTURA = 108,
    ECO_META = 7,
};

typedef struct
{
    float pos_x;
    float abertura_y;
    bool contou_ponto;
} Obstaculo;

typedef struct
{
    float posy_pombo;
    float posx_pombo;
    float velocidade_pombo;
    int pontuacao;
    int resultado; // 0 durante a rodada, 1 na vitória e -1 na derrota.
    bool iniciou;
    Obstaculo cano;
} FlappyBird;

/**
 * @brief Prepara uma rodada que começa quando o jogador aperta Espaço pela primeira vez.
 * @param jogo Estado da rodada a reiniciar.
 */
void iniciarMinigame_flappybird(FlappyBird *jogo);

/**
 * @brief Move a pomba e o obstáculo e verifica colisões e pontos.
 * @param jogo Estado da rodada atual.
 * @param tempo_quadro Duração do passo em segundos.
 * @param bateu_asas Indica um novo aperto de Espaço.
 */
void atualizaMinigame_flappybird(FlappyBird *jogo, float tempo_quadro, bool bateu_asas);

#endif
