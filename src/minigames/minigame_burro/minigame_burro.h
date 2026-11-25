/**
 * @file minigame_burro.h
 * @author Enzo Andreassa Pizza
 * @brief Declara o estado e as operações da rodada do Trote Certeiro.
 */

#ifndef MINIGAME_BURRO_H
#define MINIGAME_BURRO_H

typedef struct{
    int cartas[6]; // Símbolo de cada carta.
    int encontradas[6]; // Identifica as cartas que já formaram um par.
    int primeira_carta, segunda_carta; // Índices das cartas viradas ou -1 para escolha vazia.
    int pares_encontrados; // Quantidade de pares encontrados.
    int chances; // Quantidade de erros que restam na rodada.
    float tempo_revelacao; // Segundos acumulados desde que a segunda carta foi virada.
    int resultado; // 0 em andamento, 1 na vitória e -1 na derrota.
} MinigameBurro;

/**
 * @brief Embaralha as seis cartas e inicia a rodada com três chances.
 * @param jogo Ponteiro para o estado da rodada.
 */
void iniciarMinigameBurro(MinigameBurro *jogo);

/**
 * @brief Registra um clique válido como primeira ou segunda escolha da jogada.
 *        O segundo clique inicia o tempo de revelação do par.
 * @param jogo Ponteiro para o estado da rodada.
 * @param posicao Índice da carta clicada, de 0 a 5.
 */
void selecionar_Carta_Burro(MinigameBurro *jogo, int posicao);

/**
 * @brief Mantém as duas cartas reveladas por 0,8 segundo e resolve a jogada.
 *        Marca um par igual como encontrado ou desconta uma chance em caso de erro.
 *        A rodada termina ao encontrar três pares ou esgotar as chances.
 * @param jogo Ponteiro para o estado da rodada.
 * @param tempo_decorrido Duração do quadro em segundos.
 */
void atualizarMinigameBurro(MinigameBurro *jogo, float tempo_decorrido);

#endif
