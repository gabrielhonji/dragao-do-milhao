/**
 * @file arte_cenas.h
 * @brief Declara o carregamento e o desenho dos cenários do jogo.
 */

#ifndef ARTE_CENAS_H
#define ARTE_CENAS_H

#include <raylib.h>

typedef enum
{
    CENA_JORNADA,
    CENA_BATALHA,
    CENA_VITORIA,
    CENA_DERROTA,
    CENA_DERROTA_JORNADA,
} CenaQuiz;

typedef enum
{
    GRUTA_VAZIA,
    GRUTA_OURO_BAIXO,
    GRUTA_OURO_MEDIO,
    GRUTA_OURO_ALTO,
} EstadoGruta;

/** @brief Carrega as texturas depois de abrir a janela. */
void carregar_arte_cenas(void);

/** @brief Libera as texturas antes de fechar a janela. */
void descarregar_arte_cenas(void);

/** @brief Desenha o mapa, o véu e o dragão do menu. */
void desenhar_arte_menu(void);

/** @brief Desenha o mapa, o véu e o dragão dos créditos. */
void desenhar_arte_creditos(void);

/**
 * @brief Desenha o fundo e os personagens do trecho atual do quiz.
 * @param cena Composição correspondente ao progresso ou ao resultado da partida.
 */
void desenhar_arte_quiz(CenaQuiz cena);

/**
 * @brief Desenha o cenário da gruta conforme o resultado da partida.
 * @param estado Tamanho da pilha de ouro.
 * @param filhotes Quantidade de filhotes a exibir, limitada a dois.
 * @param mostrar_dragao Indica se o dragão voltou vivo para a gruta.
 */
void desenhar_arte_gruta(EstadoGruta estado, int filhotes, bool mostrar_dragao);

/** @brief Desenha o pátio e o dragão na tela do Sopro Ancestral. */
void desenhar_arte_sopro(void);

#endif
