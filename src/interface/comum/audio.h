/**
 * @file audio.h
 * @brief Declara os controles das trilhas da interface.
 */

#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

/** @brief Indica qual trilha deve acompanhar a tela atual. */
typedef enum
{
    TRILHA_SILENCIO, ///< Estado inicial, antes de tocar uma faixa.
    TRILHA_JORNADA,  ///< Faixa compartilhada pelas telas fora da Batalha.
    TRILHA_BATALHA,  ///< Faixa da batalha contra o príncipe.
    TOTAL_TRILHAS,
} TrilhaAudio;

/** @brief Abre o dispositivo de áudio e prepara as duas faixas para reprodução. */
void iniciar_audio(void);

/**
 * @brief Troca a faixa quando necessário e alimenta a reprodução a cada quadro.
 * @param trilha Faixa que a tela atual pede. Use TRILHA_SILENCIO para parar a música.
 */
void atualizar_audio(TrilhaAudio trilha);

/** @brief Alterna o volume geral do jogo, mantendo a trilha na posição atual. */
void alternar_som(void);

/** @return Verdadeiro quando o jogador deixou o som ligado nesta execução. */
bool som_esta_ligado(void);

/** @brief Libera as faixas carregadas e fecha o dispositivo de áudio. */
void encerrar_audio(void);

#endif
