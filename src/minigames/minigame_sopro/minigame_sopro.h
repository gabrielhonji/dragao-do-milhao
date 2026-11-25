/**
 * @file minigame_sopro.h
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief Declara o estado e as operações do Sopro Ancestral.
 * @version 0.1
 * @date 2026-11-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MINIGAME_SOPRO_H
#define MINIGAME_SOPRO_H

typedef struct
{
    float tempo;             // Segundos desde o início da tentativa.
    float posicao_barra;     // Posição do marcador de 0 a 50.
    float velocidade_barra;  // Unidades por segundo. O sinal determina o sentido.
    int zona_inicio;         // Primeiro ponto em que o jogador pode acertar.
    int zona_final;          // Último ponto em que o jogador pode acertar.
    int resultado;           // 0 em andamento, 1 em caso de acerto e -1 em caso de falha.
} MinigameSopro;

/** @brief Reinicia o minigame e sorteia a zona de acerto. */
void iniciarMinigameSopro(void);

/**
 * @brief Avança o marcador e avalia o aperto de Espaço.
 * @param tempo_decorrido Duração do quadro em segundos.
 * @param apertou_espaco 1 se o jogador pressionou Espaço neste quadro e 0 caso contrário.
 */
void atualizarMinigame(float tempo_decorrido, int apertou_espaco);

/** @return 0 durante a tentativa, 1 no acerto e -1 no erro ou ao esgotar o tempo. */
int obterResultadoMinigame(void);

/** @return Endereço do estado para a interface desenhar a barra e a zona. */
const MinigameSopro *obterEstadoMinigameSopro(void);

#endif
