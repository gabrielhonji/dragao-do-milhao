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

/** @brief Lê o clique de cancelar ou de confirmar a retirada. */
AcaoRetirada ler_acao_retirada(void);

/** @brief Mostra o aviso de encerramento e os botões Cancelar e Retirar. */
void desenhar_retirada(Font fonte, Font fonte_titulo, int tamanho_corpo);

#endif
