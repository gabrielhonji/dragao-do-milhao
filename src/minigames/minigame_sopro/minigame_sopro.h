/**
 * @file minigame_sopro.h
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 2026-11-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MINIGAME_SOPRO_H
#define MINIGAME_SOPRO_H

typedef struct{
    float tempo;
    float posicao_barra;
    float velocidade_barra;
    int zona_inicio;
    int zona_final;
    int resultado;
}MinigameSopro;

void iniciarMinigameSopro(void);
void atualizarMinigame(float tempo_decorrido, int apertou_espaco);
int obterResultadoMinigame(void);

#endif