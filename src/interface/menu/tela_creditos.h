/**
 * @file tela_creditos.h
 * @brief Desenha os créditos e o painel da equipe.
 */

#ifndef TELA_CREDITOS_H
#define TELA_CREDITOS_H

#include <raylib.h>

/** @brief Desenha a arte, os nomes da equipe e o botão Voltar. */
void desenhar_creditos(Font fonte, Font fonte_titulo, Rectangle mapa, Rectangle painel_equipe, Rectangle voltar);

#endif
