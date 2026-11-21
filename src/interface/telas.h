/**
 * @file telas.h
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Declara o ponto de entrada do modo visual de teste.
 * @version 0.1
 * @date 2026-11-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef TELAS_H
#define TELAS_H
#include "../perguntas/banco_perguntas.h"

/**
 * @brief Abre a janela de teste usando o banco carregado.
 * @param banco Banco de perguntas mantido pela main.
 */
void executar_tela_teste(const Banco_perguntas *banco);

#endif
