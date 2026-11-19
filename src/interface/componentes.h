/**
 * @file componentes.h
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Declara as operações de entrada e desenho dos componentes da interface gráfica.
 * @version 0.1
 * @date 2026-11-18
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef COMPONENTES_H
#define COMPONENTES_H

#include <raylib.h>

bool botao_clicado(Rectangle area);
void desenhar_botao(Font fonte, Rectangle area, const char *rotulo);

#endif
