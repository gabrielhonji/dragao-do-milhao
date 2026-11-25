/**
 * @file minigame_burro.c
 * @author Enzo Andreassa Pizza
 * @brief Executa o protótipo de cartas do Trote Certeiro pelo terminal.
 * @version 0.1
 * @date 2026-11-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "minigame_burro.h"

int jogarMinigame_burro(void)
{
    enum {TOTAL_CARTAS = 6, TOTAL_PARES = 3};
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
