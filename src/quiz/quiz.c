/**
 * @file quiz.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Implementação das funções definidas no header quiz.h
 * @version 0.1
 * @date 2026-11-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quiz.h"

/**
  * @brief Função usada para inicializar a estrutura partida
  * @param partidas recebe o ponteiro para partida que será inicializado
  */
void inicializa_partida(Partida *partidas)
{
    partidas->nome_jogador[0] = '\0'; //zerando a minha string do nome colocando o caracter finalizador na primeira posição
    partidas->questao_atual = 0;      //ainda não estou em nenhuma questao != de ID
    partidas->acertos = 0;            //zero os acertos
    partidas->situacao = andamento;
    for (int i = 0; i < 15; i++) {
        partidas->ids_sorteados[i] = -1; //um a um coloco os ids em -1
    }
    for (int i = 0; i < 3; i++) {
        partidas->minigames_usados[i] = 0; //um a um indico que os minigames ainda não foram usados
    }
} //INICIALIZA PARTIDA

/**
  * @brief Função que recebe qual vai ser a próxima questão e retorna o nível de dificuldade dessa
  * @param partidas Tentativa cuja a seleção de dificuldade irá se basear.
  * @return Dificuldade da questão a ser escolhida.
  */
Dificuldade determina_dificuldade(const Partida *partidas)
{

    if ((partidas->questao_atual + 1) < 1 || (partidas->questao_atual + 1) > 15) {
        return 0; //caso as próximas questões são abaixo de 1 ou acima de 15
    }
    else if ((partidas->questao_atual + 1) <= 2) {
        return muito_facil;
    }
    else if ((partidas->questao_atual + 1) <= 4) {
        return facil;
    }
    else if ((partidas->questao_atual + 1) <= 8) {
        return medio;
    }
    else if ((partidas->questao_atual + 1) <= 12) {
        return dificil;
    }
    else {
        return muito_dificil;
    }
} //DETERMINA DIFICULDADE

/**
 * @brief Procura no meu vetor de ids já sorteados o id atual, para ver se pode apresentar essa pergunta ou se ela já foi apresentada
 * @param partida struct partida sendo passado por referência
 * @param id_pergunta id da pergunta atual que o programa irá verificar se já foi sorteada
 * @return Zero para a pergunta não sorteada e 1 para a que ja foi sorteada.
 */
int pergunta_ja_sorteada(const Partida *partida, int id_pergunta)
{

    for (int i = 0; i < partida->questao_atual; i++) {
        if (partida->ids_sorteados[i] == id_pergunta) {
            return 1;
        }
    }
    return 0;
} // PERGUNTA JA SORTEADA

/**
 * @brief Verifica se uma pergunta atende aos critérios do sorteio.
 * @param pergunta Candidata do banco.
 * @param partida Tentativa cujo histórico será consultado.
 * @param dificuldade Dificuldade exigida no próximo sorteio.
 * @return 1 se a pergunta for elegível, 0 se não for.
 */
static int pergunta_disponivel(const Pergunta *pergunta, const Partida *partida, Dificuldade dificuldade)
{
    return pergunta->dificuldade == dificuldade && !pergunta_ja_sorteada(partida, pergunta->identificador);
}

int conta_perguntas_disponiveis(const Banco_perguntas *banco, const Partida *partida)
{
    Dificuldade dificuldade = determina_dificuldade(partida);
    if (dificuldade == 0 || partida->situacao != andamento) {
        return 0;
    }

    int total = 0;
    for (int i = 0; i < banco->quant_perguntas; i++) {
        if (pergunta_disponivel(&banco->perguntas[i], partida, dificuldade)) {
            total++;
        }
    }
    return total;
}

const Pergunta *sorteia_pergunta(const Banco_perguntas *banco, Partida *partida)
{
    int total = conta_perguntas_disponiveis(banco, partida);
    if (total == 0) {
        return NULL;
    }

    Dificuldade dificuldade = determina_dificuldade(partida);
    int candidatas_a_pular = rand() % total;

    for (int i = 0; i < banco->quant_perguntas; i++) {
        const Pergunta *pergunta = &banco->perguntas[i];
        if (!pergunta_disponivel(pergunta, partida, dificuldade)) {
            continue;
        }

        if (candidatas_a_pular == 0) {
            // O histórico guarda o ID.
            partida->ids_sorteados[partida->questao_atual] = pergunta->identificador;
            partida->questao_atual++;
            return pergunta;
        }
        candidatas_a_pular--;
    }
    return NULL;
}
