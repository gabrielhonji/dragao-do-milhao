/**
 * @file minigame_sopro.c
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief Controla a carga, o tempo e o resultado do Sopro Ancestral.
 * @version 0.1
 * @date 2026-11-23
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>
#include "minigame_sopro.h"

void iniciarMinigameSopro(MinigameSopro *jogo)
{
    jogo->carga = 0;
    jogo->resultado = 0;
    jogo->tempo = 0;
}

void atualizarMinigame(MinigameSopro *jogo, float tempo_decorrido, int apertou_espaco)
{
    if (jogo->resultado != 0)
        return; // Mantém o estado da rodada encerrada para a tela de resultado.

    jogo->tempo += tempo_decorrido;

    if (jogo->tempo >= SOPRO_TEMPO_LIMITE) { // Verifica o prazo antes de aplicar o toque.
        jogo->resultado = -1;
        return;
    }

    jogo->carga -= 22.0f * tempo_decorrido; // Calcula a queda pela duração do quadro.
    if (jogo->carga <= 0.0f)
        jogo->carga = 0.0f;

    if (apertou_espaco == 1) // Aplica o ganho apenas no quadro do novo aperto.
        jogo->carga += 7.0f;

    if (jogo->carga >= 100.0f) { // Limita a barra à carga máxima ao vencer.
        jogo->carga = 100.0f;
        jogo->resultado = 1;
    }
}
