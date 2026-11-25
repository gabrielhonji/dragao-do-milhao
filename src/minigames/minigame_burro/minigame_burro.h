/**
 * @file minigame_burro.h
 * @author Enzo Andreassa Pizza
 * @brief Declara o protótipo de cartas do Trote Certeiro.
 */

#ifndef MINIGAME_BURRO_H
#define MINIGAME_BURRO_H

typedef struct{
    int cartas[6]; //guarda o símbolo de cada carta
    int encontradas[6]; //marca quais cartas já formaram par
    int primeira_carta, segunda_carta; //guada os indices das cartas viradas na jogada atual
    int pares_encontrados; //conta os pares resolvidos
    int chances; //conta quantos pares errados a rodada ainda permite
    float tempo_revelacao; //quanto tempo as duas cartas ficam visíveis
    int resultado; //indica andamento(0), vitoria(1) ou derrota(-1)
} MinigameBurro;

void iniciarMinigameBurro(MinigameBurro *jogo);

#endif
