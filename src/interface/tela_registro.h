/**
 * @file tela_registro.h
 * @brief Edita o nome e grava o resultado da tentativa.
 */

#ifndef TELA_REGISTRO_H
#define TELA_REGISTRO_H

#include <raylib.h>
#include "../quiz/quiz.h"

typedef struct
{
    char nome[41]; // Até 40 bytes de nome e o '\0'.
    bool selecionar_nome;
    const char *erro;
} TelaRegistro;

/**
 * @brief Atualiza o campo de nome e processa Salvar.
 * @param tela Formulário que recebe o nome e o aviso de erro.
 * @param partida Tentativa já encerrada.
 * @param salvar Área do botão Salvar.
 * @param janela_em_foco Indica se o campo pode ler o teclado.
 * @param confirmou Indica se Enter foi pressionado neste quadro.
 * @return Verdadeiro quando o resultado foi gravado e a gruta pode abrir.
 */
bool atualizar_registro(TelaRegistro *tela, const Partida *partida, Rectangle salvar,
                       bool janela_em_foco, bool confirmou);

/** @brief Desenha o campo de nome, a mensagem e o botão Salvar. */
void desenhar_registro(const TelaRegistro *tela, Font fonte, Font fonte_titulo, Rectangle salvar);

#endif
