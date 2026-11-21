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

/**
 * @brief Detecta o início de um clique com o botão esquerdo dentro da área.
 * @param area Retângulo do botão, em coordenadas da janela.
 * @return True se o clique começou dentro da área neste quadro.
 */
bool botao_clicado(Rectangle area);

/**
 * @brief Desenha um botão com rótulo centralizado e destaque ao passar o mouse.
 * @param fonte Fonte usada no rótulo.
 * @param area Retângulo do botão, em coordenadas da janela.
 * @param rotulo Texto exibido no botão.
 */
void desenhar_botao(Font fonte, Rectangle area, const char *rotulo);

/**
 * @brief Desenha um botão com destaque de seleção e aparência de bloqueio.
 *
 * @param fonte Fonte do rótulo.
 * @param area Retângulo do botão.
 * @param rotulo Texto exibido.
 * @param selecionado Mantém o botão destacado quando o mouse sai.
 * @param habilitado Controla o tom do botão e o destaque ao passar o mouse.
 */
void desenhar_botao_com_estado(Font fonte, Rectangle area, const char *rotulo, bool selecionado, bool habilitado);

#endif
