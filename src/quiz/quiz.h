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
    #include "../perguntas/pergunta.h"

    typedef enum {andamento = 1, vitoria, derrota, retirada} Situacao_partida; //enum para definir qual a situação da partida

    typedef struct{
        char nome_jogador[100];
        int questao_atual; //indica qual a questão atual
        int acertos; //contabiliza os acertos
        Situacao_partida situacao;
        int ids_sorteados[15];
        int minigames_usados[3]; //indicará quais minigames já foram utilizados - valor 0 se ainda não foi usado e valor 1 se já foi usado
    } Partida;

    void inicializa_partida(Partida *partidas);

    Dificuldade determina_dificuldade(Partida *partidas); //determina o nível de dificuldade das perguntas de acordo com a questão atual

    int pergunta_ja_sorteada(Partida *partida, int id_pergunta); //verifica se aquela pergunta com aquele ID já foi sorteada

 #endif /*QUIZ_H*/