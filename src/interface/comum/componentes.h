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

#include <stddef.h> // Declara size_t, usado para a capacidade do campo.
#include <raylib.h>

/**
 * @brief Define a aparência do botão. A disponibilidade ainda é controlada pelo parâmetro 'habilitado'.
 */
typedef enum
{
    BOTAO_SECUNDARIO,
    BOTAO_SELECIONADO,
    BOTAO_PRIMARIO,
    BOTAO_ACERTO,
    BOTAO_ERRO,
    BOTAO_SAIDA,
    BOTAO_DICA_BURRO,
    BOTAO_DICA_FOGO,
    BOTAO_DICA_CARTA,
} EstiloBotao;

/**
 * @brief Detecta o início de um clique dentro da área enquanto a janela está em foco.
 * @param area Retângulo do botão na tela lógica de 960 × 540.
 * @return True se o clique começou dentro da área neste quadro.
 */
bool botao_clicado(Rectangle area);

/**
 * @brief Desenha um painel com o contorno em degraus usado no mapa do menu.
 * @param area Limites externos do painel na tela.
 * @param fundo Cor que preenche o interior da moldura.
 */
void desenhar_painel(Rectangle area, Color fundo);

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
 * @param estilo Aparência adequada à ação ou ao estado do botão.
 * @param habilitado Controla o tom do botão e o destaque ao passar o mouse.
 */
void desenhar_botao_com_estado(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo, bool habilitado);

/**
 * @brief Desenha uma opção de resposta com o texto alinhado à esquerda.
 * @param fonte Fonte usada para a resposta.
 * @param area Retângulo da alternativa na tela lógica.
 * @param rotulo Texto da resposta.
 * @param estilo Aparência da opção conforme a seleção e o resultado.
 * @param habilitada Permite o destaque ao passar o mouse.
 * @param reserva_direita Largura extra para o selo de uma ajuda, ou zero sem selo.
 */
void desenhar_alternativa(Font fonte, Rectangle area, const char *rotulo, EstiloBotao estilo, bool habilitada, float reserva_direita);

/**
 * @brief Quebra linhas e reduz a fonte quando necessário para caber no retângulo.
 * @param fonte Fonte usada na medição e no desenho.
 * @param area Limite de largura e altura do texto.
 * @param texto Texto UTF-8 terminado em '\0'.
 * @param tamanho Tamanho de fonte preferido.
 * @param cor Cor do texto.
 * @param centralizar_verticalmente Centraliza na altura disponível quando verdadeiro.
 */
void desenhar_texto_ajustado(Font fonte, Rectangle area, const char *texto, int tamanho, Color cor, bool centralizar_verticalmente);

/**
 * @brief Processa a digitação, o Backspace e o Ctrl+A do campo em foco.
 * @param texto Vetor com o texto atual, terminado em '\0', que será editado.
 * @param capacidade Tamanho do vetor em bytes, incluindo o espaço para '\0'.
 * @param selecionar_tudo Estado da seleção, atualizado pela função. Se verdadeiro,
 *        a próxima edição substitui ou apaga todo o conteúdo.
 */
void editar_texto(char *texto, size_t capacidade, bool *selecionar_tudo);

/**
 * @brief Desenha um campo de texto de uma linha e mostra o fim se o texto for comprido.
 * @param fonte Fonte usada no texto.
 * @param area Limites do campo na tela lógica.
 * @param texto Conteúdo exibido, terminado em '\0'.
 * @param ativo Indica se o campo tem foco e deve ganhar contorno.
 * @param selecionado Indica se deve destacar a seleção de todo o texto.
 */
void desenhar_campo(Font fonte, Rectangle area, const char *texto, bool ativo, bool selecionado);

/**
 * @brief Desenha uma linha e abrevia o texto quando passa da largura.
 * @param fonte Fonte usada para medir e desenhar.
 * @param texto Texto terminado em '\0'.
 * @param posicao Canto superior esquerdo do texto.
 * @param largura Largura disponível até a próxima coluna.
 */
void desenhar_previa_texto(Font fonte, const char *texto, Vector2 posicao, float largura);

#endif
