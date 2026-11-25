/**
 * @file tela_retirada.c
 * @brief Exibe e lê a confirmação de retirada tática.
 */

#include "tela_retirada.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

// O clique usa os mesmos retângulos que o desenho.
static const Rectangle cancelar = {270, 318, 200, 44};
static const Rectangle confirmar = {490, 318, 200, 44};

AcaoRetirada ler_acao_retirada(void)
{
    if (botao_clicado(cancelar))
        return RETIRADA_CANCELAR;
    if (botao_clicado(confirmar))
        return RETIRADA_CONFIRMAR;
    return RETIRADA_SEM_ACAO;
}

void desenhar_retirada(Font fonte, Font fonte_titulo, int tamanho_corpo)
{
    desenhar_painel((Rectangle){242, 152, 476, 240}, COR_ESCURO);
    DrawTextEx(fonte_titulo, "Retirada Tática", (Vector2){270, 182}, 36, 1, COR_TEXTO_CLARO);
    Rectangle mensagem = {270, 247, 420, 48}; // A pergunta ocupa o espaço acima dos dois botões.
    desenhar_texto_ajustado(fonte, mensagem, "Encerrar a tentativa e voltar à gruta?", tamanho_corpo, COR_TEXTO_CLARO, false);
    desenhar_botao(fonte, cancelar, "Cancelar");
    desenhar_botao_com_estado(fonte, confirmar, "Retirar", BOTAO_SAIDA, true);
}
