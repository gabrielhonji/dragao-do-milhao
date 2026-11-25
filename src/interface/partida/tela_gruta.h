/**
 * @file tela_gruta.h
 * @brief Organiza a prévia e o resultado mostrados na gruta.
 */

#ifndef TELA_GRUTA_H
#define TELA_GRUTA_H

#include "../comum/arte_cenas.h"
#include "../../quiz/quiz.h"

typedef struct
{
    EstadoGruta estado;
    bool previa;
} TelaGruta;

/**
 * @brief Prepara a gruta aberta pelo atalho F2 do menu.
 * @param gruta Estado que será inicializado em modo de prévia.
 */
void abrir_previa_gruta(TelaGruta *gruta);

/**
 * @brief Permite escolher com 1 a 4 a composição da prévia.
 * @param gruta Estado da composição que pode mudar neste quadro.
 * @param janela_em_foco Indica se a janela pode receber o atalho.
 */
void atualizar_previa_gruta(TelaGruta *gruta, bool janela_em_foco);

/**
 * @brief Escolhe a pilha de ouro da retirada pelo último marco.
 * @param gruta Estado que exibirá o resultado.
 * @param marco Último marco da partida, com valores 0, 5 ou 10.
 */
void preparar_gruta_retirada(TelaGruta *gruta, int marco);

/**
 * @brief Define a composição de vitória ou de derrota.
 * @param gruta Estado que exibirá o resultado.
 * @param situacao Desfecho da partida.
 */
void preparar_gruta_final(TelaGruta *gruta, Situacao_partida situacao);

/**
 * @brief Desenha a composição, a mensagem de resultado e o botão Voltar.
 * @param gruta Estado da composição.
 * @param partida Tentativa encerrada. Os dados são consultados só fora da prévia.
 * @param fonte Fonte dos textos.
 * @param voltar Área do botão Voltar.
 * @param x_mensagem Posição horizontal da mensagem no rodapé.
 * @param tamanho_corpo Tamanho da mensagem em pixels lógicos.
 */
void desenhar_tela_gruta(const TelaGruta *gruta, const Partida *partida, Font fonte,
                        Rectangle voltar, float x_mensagem, int tamanho_corpo);

#endif
