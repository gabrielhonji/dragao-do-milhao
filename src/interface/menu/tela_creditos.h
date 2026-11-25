/**
 * @file tela_creditos.h
 * @brief Desenha os créditos e o painel da equipe.
 */

#ifndef TELA_CREDITOS_H
#define TELA_CREDITOS_H

#include <raylib.h>

/**
 * @brief Desenha a arte, os nomes da equipe e o botão Voltar.
 * @param fonte Fonte dos nomes e dos textos de identificação.
 * @param fonte_titulo Fonte dos títulos.
 * @param mapa Área reservada à cena com o dragão.
 * @param painel_equipe Área reservada aos nomes da equipe.
 * @param voltar Área do botão Voltar.
 */
void desenhar_creditos(Font fonte, Font fonte_titulo, Rectangle mapa, Rectangle painel_equipe, Rectangle voltar);

#endif
