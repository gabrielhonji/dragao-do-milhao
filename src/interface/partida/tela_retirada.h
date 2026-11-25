/**
 * @file tela_retirada.h
 * @brief Exibe e lê a confirmação de retirada tática.
 */

#ifndef TELA_RETIRADA_H
#define TELA_RETIRADA_H

#include <raylib.h>

typedef enum
{
    RETIRADA_SEM_ACAO,
    RETIRADA_CANCELAR,
    RETIRADA_CONFIRMAR,
} AcaoRetirada;

/**
 * @brief Lê o clique de cancelar ou de confirmar a retirada.
 * @return RETIRADA_CANCELAR, RETIRADA_CONFIRMAR ou RETIRADA_SEM_ACAO conforme os botões acionados.
 */
AcaoRetirada ler_acao_retirada(void);

/**
 * @brief Mostra o aviso de encerramento e os botões Cancelar e Retirar.
 * @param fonte Fonte da mensagem e dos botões.
 * @param fonte_titulo Fonte do título.
 * @param tamanho_corpo Tamanho lógico do texto da mensagem.
 */
void desenhar_retirada(Font fonte, Font fonte_titulo, int tamanho_corpo);

#endif
