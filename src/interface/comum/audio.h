/**
 * @file audio.h
 * @brief Declara os controles das trilhas e dos efeitos da interface.
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

/** @brief Identifica o som curto associado a uma ação do jogador. */
typedef enum
{
    SOM_BOTAO,
    SOM_ALTERNATIVA,
    SOM_ACERTO,
    SOM_ERRO,
    SOM_CARTA,
    SOM_ASA,
    SOM_SOPRO,
    TOTAL_EFEITOS
} EfeitoSonoro;

/**
 * @brief Reproduz um efeito uma vez quando o som está ligado.
 * @param efeito Som da ação que a tela acabou de aceitar.
 */
void tocar_efeito(EfeitoSonoro efeito);

/** @brief Abre o dispositivo e carrega as trilhas e os efeitos do jogo. */
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

/** @brief Libera as trilhas e os efeitos antes de fechar o dispositivo de áudio. */
void encerrar_audio(void);

#endif
