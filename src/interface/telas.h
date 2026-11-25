/**
 * @file telas.h
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Declara o ponto de entrada da interface gráfica do jogo.
 * @version 0.1
 * @date 2026-11-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef TELAS_H
#define TELAS_H
#include <stdbool.h>
#include "../perguntas/banco_perguntas.h"

/**
 * @brief Abre a interface gráfica e mantém a navegação até fechar a janela.
 * @param banco Banco inicializado pela main, consultado durante o quiz e atualizado pelo gerenciador.
 * @param iniciar_em_tela_cheia Abre no monitor inteiro quando solicitado na linha de comando.
 */
void executar_tela_teste(Banco_perguntas *banco, bool iniciar_em_tela_cheia);

#endif
