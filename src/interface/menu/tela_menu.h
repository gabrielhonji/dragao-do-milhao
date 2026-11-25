/**
 * @file tela_menu.h
 * @brief Organiza os botões e o desenho do menu principal.
 */

#ifndef TELA_MENU_H
#define TELA_MENU_H

#include <raylib.h>

typedef struct
{
    Rectangle jogar;
    Rectangle ranking;
    Rectangle gerenciar;
    Rectangle creditos;
    Rectangle sair;
    Rectangle som;
    Rectangle tela_cheia;
} BotoesMenu;

typedef enum
{
    MENU_SEM_ACAO,
    MENU_PREVIA_GRUTA,
    MENU_CREDITOS,
    MENU_RANKING,
    MENU_GERENCIAR,
    MENU_ALTERNAR_SOM,
    MENU_ALTERNAR_TELA,
    MENU_SAIR,
    MENU_JOGAR,
} AcaoMenu;

/**
 * @brief Calcula a posição dos botões usados no desenho e nos cliques.
 * @return Retângulos dos botões em coordenadas da tela lógica.
 */
BotoesMenu montar_botoes_menu(void);

/**
 * @brief Lê a ação escolhida no menu neste quadro.
 * @param botoes Áreas dos botões do menu.
 * @param janela_em_foco Indica se a janela pode receber atalhos de teclado.
 * @param confirmou Indica se Enter foi pressionado para iniciar a partida.
 * @return Ação selecionada ou MENU_SEM_ACAO quando nenhuma entrada corresponde a uma ação.
 */
AcaoMenu ler_acao_menu(const BotoesMenu *botoes, bool janela_em_foco, bool confirmou);

/**
 * @brief Desenha o menu e o aviso de falta de perguntas.
 * @param botoes Posições dos botões do menu.
 * @param fonte Fonte das mensagens e dos botões.
 * @param fonte_titulo Fonte do título.
 * @param tamanho_titulo Altura lógica do título.
 * @param dificuldade_em_falta Nome da dificuldade que precisa de perguntas, ou NULL.
 * @param faltam_perguntas Indica que o quiz não conseguiu sortear uma pergunta.
 * @param som_ligado Estado do som mostrado no botão.
 */
void desenhar_menu(const BotoesMenu *botoes, Font fonte, Font fonte_titulo,
                  int tamanho_titulo, const char *dificuldade_em_falta, bool faltam_perguntas, bool som_ligado);

#endif
