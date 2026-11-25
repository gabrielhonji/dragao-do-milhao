/**
 * @file minigame_sopro.h
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief Declara o estado e as operações do Sopro Ancestral.
 * @version 0.1
 * @date 2026-11-23
 *
 * @copyright Copyright (c) 2026
 */
#ifndef MINIGAME_SOPRO_H
#define MINIGAME_SOPRO_H

/** @brief Prazo da rodada em segundos, usado pela lógica e pela interface. */
enum {SOPRO_TEMPO_LIMITE = 5};

typedef struct
{
    float tempo;   // Segundos desde o início da rodada.
    float carga;   // Carga atual da barra, de 0 a 100.
    int resultado; // 0 em andamento, 1 na vitória e -1 quando o prazo acaba.
} MinigameSopro;

/**
 * @brief Inicializa a rodada com tempo e carga zerados e resultado em andamento.
 * @param jogo Ponteiro para o estado da rodada.
 */
void iniciarMinigameSopro(MinigameSopro *jogo);

/**
 * @brief Atualiza o tempo e a carga de uma rodada em andamento.
 *        A rodada termina ao atingir a carga máxima ou ao acabar o prazo.
 * @param jogo Ponteiro para o estado da rodada.
 * @param tempo_decorrido Duração do quadro em segundos.
 * @param apertou_espaco 1 se houve um novo aperto de Espaço neste quadro e 0 caso contrário.
 */
void atualizarMinigame(MinigameSopro *jogo, float tempo_decorrido, int apertou_espaco);

#endif
