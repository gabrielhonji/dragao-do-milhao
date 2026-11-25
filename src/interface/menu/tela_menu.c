/**
 * @file tela_menu.c
 * @brief Organiza os botões e o desenho do menu principal.
 */

#include <stddef.h>
#include "tela_menu.h"
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

BotoesMenu montar_botoes_menu(void)
{
    const Rectangle jogar = {586, 205, 318, 44}; // Primeira linha da coluna.
    const float passo = jogar.height + 12;       // A altura do botão mais 12 px de espaço até o próximo.
    const Rectangle ranking = {jogar.x, jogar.y + passo, jogar.width, jogar.height};
    const Rectangle gerenciar = {jogar.x, ranking.y + passo, jogar.width, jogar.height};
    const Rectangle creditos = {jogar.x, gerenciar.y + passo, 151, jogar.height};
    const Rectangle sair = {creditos.x + creditos.width + 9, creditos.y, 158, jogar.height}; // Intervalo de 9 px entre os botões.
    const Rectangle som = {jogar.x, creditos.y + passo, jogar.width, jogar.height};

    return (BotoesMenu){jogar, ranking, gerenciar, creditos, sair, som};
}

AcaoMenu ler_acao_menu(const BotoesMenu *botoes, bool janela_em_foco, bool confirmou)
{
    if (janela_em_foco && IsKeyPressed(KEY_F2))
        return MENU_PREVIA_GRUTA;
    if (botao_clicado(botoes->creditos))
        return MENU_CREDITOS;
    if (botao_clicado(botoes->ranking))
        return MENU_RANKING;
    if (botao_clicado(botoes->gerenciar))
        return MENU_GERENCIAR;
    if (botao_clicado(botoes->som))
        return MENU_ALTERNAR_SOM;
    if (botao_clicado(botoes->sair))
        return MENU_SAIR;
    if (confirmou || botao_clicado(botoes->jogar))
        return MENU_JOGAR;
    return MENU_SEM_ACAO;
}

void desenhar_menu(const BotoesMenu *botoes, Font fonte, Font fonte_titulo,
                  int tamanho_titulo, const char *dificuldade_em_falta, bool faltam_perguntas, bool som_ligado)
{
    desenhar_painel((Rectangle){24, 24, 524, 492}, COR_ESCURO);

    desenhar_arte_menu();

    const char *linhas[] = {"Dragão do", "Milhão"};
    const int passo_titulo = 70; // Distância entre o topo da primeira linha e o da segunda.
    const int sombra = 2;        // Desenha a cópia escura 2 pixels à direita e abaixo na base 960 × 540 (4 em escala 2×).

    for (int i = 0; i < 2; i++) {
        Vector2 posicao = {582, 39 + i * passo_titulo}; // A primeira linha começa em (582, 39) no menu.
        Vector2 posicao_sombra = {posicao.x + sombra, posicao.y + sombra};

        DrawTextEx(fonte_titulo, linhas[i], posicao_sombra, tamanho_titulo, 1, BLACK);
        DrawTextEx(fonte_titulo, linhas[i], posicao, tamanho_titulo, 1, COR_TEXTO_CLARO);
    }

    desenhar_botao_com_estado(fonte, botoes->jogar, "Jogar", BOTAO_PRIMARIO, true);
    desenhar_botao(fonte, botoes->ranking, "Ranking");
    desenhar_botao(fonte, botoes->gerenciar, "Gerenciar perguntas");
    desenhar_botao(fonte, botoes->creditos, "Créditos");
    desenhar_botao_com_estado(fonte, botoes->sair, "Sair", BOTAO_SAIDA, true);
    desenhar_botao_com_estado(fonte, botoes->som, som_ligado ? "Som ligado" : "Som desligado",
                             som_ligado ? BOTAO_SECUNDARIO : BOTAO_SAIDA, true);

    const Vector2 posicao_aviso = {botoes->jogar.x, botoes->som.y + botoes->som.height + 18}; // Espaço de 18 px abaixo do botão de som.
    if (dificuldade_em_falta != NULL) {
        DrawTextEx(fonte, "Faltam perguntas no banco.", posicao_aviso, 20, 1, COR_AVISO);
        const char *nivel = TextFormat("Nível: %s", dificuldade_em_falta);
        DrawTextEx(fonte, nivel, (Vector2){posicao_aviso.x, posicao_aviso.y + 24}, 20, 1, COR_AVISO); // Distância à linha anterior.
    }
    else if (faltam_perguntas)
        DrawTextEx(fonte, "Sem perguntas para iniciar.", posicao_aviso, 20, 1, COR_AVISO);
}
