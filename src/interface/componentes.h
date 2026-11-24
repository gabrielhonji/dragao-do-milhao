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
 * @brief Define a aparência do botão. A disponibilidade ainda é controlada pelo parâmetro 'habilitado'.
 */
typedef enum {
    BOTAO_SECUNDARIO,
    BOTAO_SELECIONADO,
    BOTAO_PRIMARIO,
} EstiloBotao;

/**
 * @brief Detecta o início de um clique dentro da área enquanto a janela está em foco.
 * @param area Retângulo do botão na tela lógica de 960 × 540.
 * @return True se o clique começou dentro da área neste quadro.
 */
bool botao_clicado(Rectangle area);

/**
 * @brief Desenha um botão de ação com rótulo centralizado e destaque ao passar o mouse.
 * @param fonte Fonte usada no rótulo.
 * @param area Retângulo do botão na tela lógica de 960 × 540.
 * @param rotulo Texto exibido no botão.
 */
void desenhar_botao(Font fonte, Rectangle area, const char *rotulo);

/**
 * @brief Desenha um botão de ação centralizado com o estilo e a disponibilidade indicados.
 *
 * @param fonte Fonte do rótulo.
 * @param area Retângulo do botão.
 * @param rotulo Texto exibido.
 * @param estilo Aparência secundária, selecionada ou primária.
 * @param habilitado Controla o tom do botão e o destaque ao passar o mouse.
 */
void desenhar_botao_com_estado(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo, bool habilitado);

/**
 * @brief Desenha uma opção de resposta com o texto alinhado à esquerda.
 * @param fonte Fonte usada para a resposta.
 * @param area Retângulo da alternativa na tela lógica.
 * @param rotulo Texto da resposta.
 * @param selecionada Mantém a opção marcada mesmo depois da confirmação.
 * @param habilitada Permite o destaque ao passar o mouse.
 */
void desenhar_alternativa(Font fonte, Rectangle area, const char *rotulo, bool selecionada, bool habilitada);

#endif
