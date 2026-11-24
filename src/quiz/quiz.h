/**
 * @file quiz.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Estrturação do quiz via terminal para futura implementação na interface gráfica
 * @version 0.1
 * @date 2026-11-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef QUIZ_H
#define QUIZ_H
#include "../perguntas/banco_perguntas.h"

typedef enum
{
    andamento = 1,
    vitoria,
    derrota,
    retirada
} Situacao_partida; //enum para definir qual a situação da partida

typedef enum
{
    resposta_acerto = 1,
    resposta_marco,
    resposta_renascimento,
    resposta_vitoria,
    resposta_derrota
} Resultado_resposta; //enum para definir quais são as diferentes situações possíveis para as respostas

enum {PV_MAX_PRINCIPE = 1000};

typedef struct
{
    char nome_jogador[100];
    int questao_atual; //indica qual a questão atual
    int acertos;       //contabiliza os acertos
    Situacao_partida situacao;
    int ids_sorteados[25];      //Histórico dos IDs exibidos durante a tentativa.
    int contador_ids_sorteados; //Quantidade de IDs registrados no histórico.
    int minigames_usados[3];    //indicará quais minigames já foram utilizados - valor 0 se ainda não foi usado e valor 1 se já foi usado
    int ovos_restantes;
    int checkpoint;
} Partida;

void inicializa_partida(Partida *partidas);

Dificuldade determina_dificuldade(const Partida *partidas); //determina o nível de dificuldade das perguntas de acordo com a questão atual

int pergunta_ja_sorteada(const Partida *partida, int id_pergunta); //verifica se aquela pergunta com aquele ID já foi sorteada

/**
     * @brief Conta as perguntas elegíveis para o próximo sorteio.
     * @param banco Banco consultado.
     * @param partida Tentativa cujo histórico e progresso serão consultados.
     * @return Quantidade de candidatas do próximo nível. Retorna 0 se o nível for inválido
     *         ou se a partida estiver fora do estado de andamento.
     */
int conta_perguntas_disponiveis(const Banco_perguntas *banco, const Partida *partida);

/**
     * @brief Sorteia uma pergunta, registra seu ID no histórico e avança o número da questão.
     * @param banco Banco de onde a pergunta será escolhida.
     * @param partida Tentativa que receberá o ID sorteado.
     * @return Endereço da pergunta no banco. Retorna NULL se não houver candidata
     *         ou se o histórico já tiver 25 IDs.
     */
const Pergunta *sorteia_pergunta(const Banco_perguntas *banco, Partida *partida);

Dificuldade verifica_banco_para_partida(const Banco_perguntas *banco);

Resultado_resposta processa_resposta(Partida *partida, const Pergunta *pergunta_atual, char resposta_escolhida);

int confirma_retirada(Partida *partida);

int calcula_pv_principe(const Partida *partida);

#endif /*QUIZ_H*/
