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
} BotoesMenu;

typedef enum
{
    MENU_SEM_ACAO,
    MENU_PREVIA_GRUTA,
    MENU_CREDITOS,
    MENU_RANKING,
    MENU_GERENCIAR,
    MENU_SAIR,
    MENU_JOGAR,
} AcaoMenu;

/** @brief Calcula a posição dos botões usados no desenho e nos cliques. */
BotoesMenu montar_botoes_menu(void);

/** @brief Lê a ação escolhida no menu neste quadro. */
AcaoMenu ler_acao_menu(const BotoesMenu *botoes, bool janela_em_foco, bool confirmou);

/**
 * @brief Desenha o menu e o aviso de falta de perguntas.
 * @param botoes Posições dos botões do menu.
 * @param fonte Fonte das mensagens e dos botões.
 * @param fonte_titulo Fonte do título.
 * @param tamanho_titulo Altura lógica do título.
 * @param dificuldade_em_falta Nome da dificuldade que precisa de perguntas, ou NULL.
 * @param faltam_perguntas Indica que a primeira pergunta não pôde ser sorteada.
 */
void desenhar_menu(const BotoesMenu *botoes, Font fonte, Font fonte_titulo,
                  int tamanho_titulo, const char *dificuldade_em_falta, bool faltam_perguntas);

#endif
