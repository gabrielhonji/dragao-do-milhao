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

    Dificuldade determina_dificuldade(const Partida *partidas); //determina o nível de dificuldade das perguntas de acordo com a questão atual

    int pergunta_ja_sorteada(const Partida *partida, int id_pergunta); //verifica se aquela pergunta com aquele ID já foi sorteada

    /**
     * @brief Conta as perguntas elegíveis para o próximo sorteio.
     * @param banco Banco consultado.
     * @param partida Tentativa cujo histórico e progresso serão consultados.
     * @return Quantidade de candidatas, ou zero quando não há sorteio possível.
     */
    int conta_perguntas_disponiveis(const Banco_perguntas *banco, const Partida *partida);

    /**
     * @brief Sorteia uma pergunta, registra seu ID e avança o número da questão.
     * @param banco Banco de onde a pergunta será escolhida.
     * @param partida Tentativa que receberá o ID sorteado.
     * @return Endereço da pergunta no banco, ou NULL se não houver candidata.
     */
    const Pergunta *sorteia_pergunta(const Banco_perguntas *banco, Partida *partida);

 #endif /*QUIZ_H*/
