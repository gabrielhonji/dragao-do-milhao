/**
 * @file quiz.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Define o estado e as regras do quiz usado pela interface gráfica.
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
} Situacao_partida; // Estado geral da partida.

typedef enum
{
    resposta_acerto = 1,
    resposta_marco,
    resposta_renascimento,
    resposta_vitoria,
    resposta_derrota
} Resultado_resposta; // Resultado de uma resposta confirmada.

enum {PV_MAX_PRINCIPE = 1000};

typedef struct
{
    char nome_jogador[100];      // Campo inicializado na partida. O formulário mantém o nome usado no ranking.
    int questao_atual; // Número da questão no progresso atual, sem relação com seu ID.
    int acertos;       // Acertos acumulados no progresso atual.
    Situacao_partida situacao;
    int ids_sorteados[25];      // Histórico dos IDs, preservado no renascimento.
    int contador_ids_sorteados; // Quantidade de IDs registrados no histórico.
    int minigames_usados[3];    // Campo inicializado na partida. A interface controla os usos em TelaQuiz.usos_ajuda.
    int ovos_restantes;
    int checkpoint;             // Último marco atingido, com valores 0, 5 ou 10.
    double tempo_ativo; // Tempo em segundos com uma pergunta aberta.
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
 * @return Endereço da pergunta no vetor do banco, válido enquanto ele mantiver a mesma alocação e ordem.
 *         Retorna NULL se não houver candidata ou se o histórico já tiver 25 IDs.
 */
const Pergunta *sorteia_pergunta(const Banco_perguntas *banco, Partida *partida);

Dificuldade verifica_banco_para_partida(const Banco_perguntas *banco);

Resultado_resposta processa_resposta(Partida *partida, const Pergunta *pergunta_atual, char resposta_escolhida);

int confirma_retirada(Partida *partida);

int calcula_pv_principe(const Partida *partida);

#endif /*QUIZ_H*/
