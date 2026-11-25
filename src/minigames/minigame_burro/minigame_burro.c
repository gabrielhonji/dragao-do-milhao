/**
 * @file minigame_burro.c
 * @author Enzo Andreassa Pizza
 * @brief Implementa o embaralhamento, a seleção e a resolução dos pares do Trote Certeiro.
 * @version 0.1
 * @date 2026-11-19
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include "minigame_burro.h"

/** @brief Executa o protótipo original do Trote Certeiro no terminal. */
int jogarMinigame_burro(void)
{
    enum
    {
        TOTAL_CARTAS = 6,
        TOTAL_PARES = 3
    };
    int cartas[TOTAL_CARTAS] = {0, 0, 1, 1, 2, 2};
    int chances = 2;
    int pares_encontrados = 0;

    // Troca cada carta com uma posição ainda não fixada.
    for (int i = TOTAL_CARTAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = cartas[i];
        cartas[i] = cartas[j];
        cartas[j] = temp;
    }

    while (chances > 0 && pares_encontrados < TOTAL_PARES) {
        int escolha_1, escolha_2;
        printf("Escolha duas cartas de 0 a %d:\n", TOTAL_CARTAS - 1);
        if (scanf("%3d %3d", &escolha_1, &escolha_2) != 2)
            return -1;

        // Valida os índices antes de acessar o vetor. -1 identifica um par já encontrado.
        if (escolha_1 < 0 || escolha_1 >= TOTAL_CARTAS ||
            escolha_2 < 0 || escolha_2 >= TOTAL_CARTAS ||
            escolha_1 == escolha_2 || cartas[escolha_1] == -1 || cartas[escolha_2] == -1) {
            puts("Escolha duas cartas diferentes que ainda estejam no jogo.");
            continue;
        }

        printf("Cartas reveladas: %d e %d.\n", cartas[escolha_1], cartas[escolha_2]);
        if (cartas[escolha_1] == cartas[escolha_2]) {
            cartas[escolha_1] = -1;
            cartas[escolha_2] = -1;
            pares_encontrados++;
        }
        else {
            chances--;
            printf("Chances restantes: %d.\n", chances);
        }
    }

    return pares_encontrados == TOTAL_PARES ? 1 : -1;
}

void iniciarMinigameBurro(MinigameBurro *jogo)
{
    int posicao;

    for(int i = 0; i<6; i++){
        jogo->cartas[i] = -1; //inicializo todos em -1
    }

    for (int i = 0; i < 6; i++){
        do {
            posicao = rand() % 6; //sorteio a posição entre 0 e 5
        } while (jogo->cartas[posicao] != -1); //garantindo que não seja repetido

        // Coloca exatamente duas cartas de cada símbolo.
        if (i < 2)
            jogo->cartas[posicao] = 0;
        else if (i < 4)
            jogo->cartas[posicao] = 1;
        else
            jogo->cartas[posicao] = 2;
    }

    for (int i = 0; i < 6; i++)
        jogo->encontradas[i] = 0;

    jogo->primeira_carta = -1;
    jogo->segunda_carta = -1;
    jogo->pares_encontrados = 0;
    jogo->chances = 3;
    jogo->tempo_revelacao = 0.0f;
    jogo->resultado = 0;
}//INICIAR MINIGAME BURRO


void selecionar_Carta_Burro(MinigameBurro *jogo, int posicao){

    if(posicao < 0 || posicao >=6){
        return;
    }

    if(jogo->resultado != 0 || jogo->segunda_carta != -1 || jogo->encontradas[posicao] != 0 || posicao == jogo->primeira_carta){
        return;
    }

    if(jogo->primeira_carta == -1){
        jogo->primeira_carta = posicao;
        return;
    }else{
        jogo->segunda_carta = posicao;
        jogo->tempo_revelacao = 0;
    }
} //SELECIONAR CARTA BURRO


void atualizarMinigameBurro(MinigameBurro *jogo, float tempo_decorrido){
    if (jogo->resultado != 0 || jogo->segunda_carta == -1) {
        return;
    }

    jogo->tempo_revelacao += tempo_decorrido;
    if (jogo->tempo_revelacao < 0.8f) {
        return;
    }

    int primeira = jogo->primeira_carta;
    int segunda = jogo->segunda_carta;

    if (jogo->cartas[primeira] == jogo->cartas[segunda]) {
        jogo->encontradas[primeira] = 1;
        jogo->encontradas[segunda] = 1;
        jogo->pares_encontrados++;

        if (jogo->pares_encontrados == 3)
            jogo->resultado = 1;
    }
    else {
        jogo->chances--;

        if (jogo->chances == 0)
            jogo->resultado = -1;
    }

    jogo->primeira_carta = -1;
    jogo->segunda_carta = -1;
    jogo->tempo_revelacao = 0.0f;
} //ATUALIZAR MINIGAME BURRO
