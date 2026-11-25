/**
 * @file tela_retirada.c
 * @brief Exibe e lê a confirmação de retirada tática.
 */

#include "tela_retirada.h"
#include "componentes.h"

AcaoRetirada ler_acao_retirada(Rectangle cancelar, Rectangle confirmar)
{
    if (botao_clicado(cancelar))
        return RETIRADA_CANCELAR;
    if (botao_clicado(confirmar))
        return RETIRADA_CONFIRMAR;
    return RETIRADA_SEM_ACAO;
}

void desenhar_retirada(Font fonte, Font fonte_titulo, int tamanho_corpo, int tamanho_titulo,
                      Rectangle cancelar, Rectangle confirmar)
{
    DrawTextEx(fonte_titulo, "Retirada Tática", (Vector2){48, 96}, tamanho_titulo, 1, BLACK);
    DrawTextEx(fonte, "Encerrar a tentativa e voltar à gruta?", (Vector2){48, 184}, tamanho_corpo, 1, BLACK);
    desenhar_botao(fonte, cancelar, "Cancelar");
    desenhar_botao(fonte, confirmar, "Retirar");
}
