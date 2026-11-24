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
    partidas->contador_ids_sorteados = 0;
    partidas->ovos_restantes = 0;
    partidas->checkpoint = 0;
    for (int i = 0; i < 25; i++)
        partidas->ids_sorteados[i] = -1; //um a um coloco os ids em -1
    for (int i = 0; i < 3; i++)
        partidas->minigames_usados[i] = 0; //um a um indico que os minigames ainda não foram usados
} //INICIALIZA PARTIDA

/**
  * @brief Função que recebe qual vai ser a próxima questão e retorna o nível de dificuldade dessa
  * @param partidas Tentativa cuja a seleção de dificuldade irá se basear.
  * @return Dificuldade da questão a ser escolhida.
  */
Dificuldade determina_dificuldade(const Partida *partidas)
{

    if ((partidas->questao_atual + 1) < 1 || (partidas->questao_atual + 1) > 15)
        return 0; //caso as próximas questões são abaixo de 1 ou acima de 15
    else if ((partidas->questao_atual + 1) <= 2)
        return muito_facil;
    else if ((partidas->questao_atual + 1) <= 4)
        return facil;
    else if ((partidas->questao_atual + 1) <= 8)
        return medio;
    else if ((partidas->questao_atual + 1) <= 12)
        return dificil;
    else
        return muito_dificil;
} //DETERMINA DIFICULDADE

/**
 * @brief Procura no meu vetor de ids já sorteados o id atual, para ver se pode apresentar essa pergunta ou se ela já foi apresentada
 * @param partida struct partida sendo passado por referência
 * @param id_pergunta id da pergunta atual que o programa irá verificar se já foi sorteada
 * @return Zero para a pergunta não sorteada e 1 para a que ja foi sorteada.
 */
int pergunta_ja_sorteada(const Partida *partida, int id_pergunta)
{

    for (int i = 0; i < partida->contador_ids_sorteados; i++)
        if (partida->ids_sorteados[i] == id_pergunta)
            return 1;
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
    if (dificuldade == 0 || partida->situacao != andamento)
        return 0;

    int total = 0;
    for (int i = 0; i < banco->quant_perguntas; i++)
        if (pergunta_disponivel(&banco->perguntas[i], partida, dificuldade))
            total++;
    return total;
}

const Pergunta *sorteia_pergunta(const Banco_perguntas *banco, Partida *partida)
{
    int total = conta_perguntas_disponiveis(banco, partida);
    if (total == 0 || partida->contador_ids_sorteados == 25)
        return NULL;

    Dificuldade dificuldade = determina_dificuldade(partida);
    int candidatas_a_pular = rand() % total;

    for (int i = 0; i < banco->quant_perguntas; i++) {
        const Pergunta *pergunta = &banco->perguntas[i];
        if (!pergunta_disponivel(pergunta, partida, dificuldade))
            continue;

        if (candidatas_a_pular == 0) {
            // O histórico guarda o ID.
            partida->ids_sorteados[partida->contador_ids_sorteados] = pergunta->identificador;
            partida->contador_ids_sorteados++;
            partida->questao_atual++;
            return pergunta;
        }
        candidatas_a_pular--;
    }
    return NULL;
}

/**
 * @brief Função que confere a quantidade mínima de perguntas por dificuldade antes de iniciar a partida.
 *        São necessárias 2 muito fáceis, 2 fáceis, 7 médias, 8 difíceis e 9 muito difíceis.
 *
 * @param banco Ponteiro para o banco de perguntas carregado.
 * @return Dificuldade Retorna o primeiro nível abaixo do mínimo, da muito fácil à muito difícil.
 *         Retorna 0 quando o banco atende aos mínimos de todos os níveis.
 */
Dificuldade verifica_banco_para_partida(const Banco_perguntas *banco)
{

    int contador[6];
    for (int i = 0; i < 6; i++) {
        contador[i] = 0;
    } //zerando o contador

    for (int i = 0; i < banco->quant_perguntas; i++) {
        if (banco->perguntas[i].dificuldade >= 1 && banco->perguntas[i].dificuldade <= 5) {
            switch (banco->perguntas[i].dificuldade) {
                case 1:
                    contador[1]++;
                    break;

                case 2:
                    contador[2]++;
                    break;

                case 3:
                    contador[3]++;
                    break;

                case 4:
                    contador[4]++;
                    break;

                case 5:
                    contador[5]++;
                    break;
            } //switch
        } //if
    } //for que contabiliza no contador

    //verificando se a quantidade é necessária
    if (contador[1] < 2)
        return muito_facil;
    if (contador[2] < 2)
        return facil;
    if (contador[3] < 7)
        return medio;
    if (contador[4] < 8)
        return dificil;
    if (contador[5] < 9)
        return muito_dificil;

    return 0;
}

/**
 * @brief Função que compara a letra confirmada com a resposta correta e atualiza a partida.
 *        O primeiro acerto nas questões 5 e 10 registra o marco e concede um ovo.
 *        Ao errar com um ovo disponível, a função gasta um ovo e volta a questão e os acertos
 *        ao último marco. O próximo sorteio abre a questão 6 ou 11, preservando o histórico
 *        de perguntas e os usos das ajudas.
 *
 * @param partida Ponteiro para a partida em andamento que receberá o resultado.
 * @param pergunta_atual Ponteiro para a pergunta exibida, da qual é lida a resposta correta.
 * @param resposta_escolhida Letra de A a D confirmada pelo jogador.
 * @return Resultado_resposta Retorna resposta_acerto para um acerto comum, resposta_marco
 *         ao conquistar um novo marco, resposta_renascimento ao gastar um ovo, resposta_vitoria
 *         ao acertar a questão 15 e resposta_derrota ao errar sem ovos disponíveis.
 */
Resultado_resposta processa_resposta(Partida *partida, const Pergunta *pergunta_atual, char resposta_escolhida)
{
    if (resposta_escolhida == pergunta_atual->resposta) {

        partida->acertos++; //contabilizo que acertei

        if (partida->questao_atual == 5 && partida->checkpoint < 5) {
            partida->checkpoint = 5;
            partida->ovos_restantes++;
            return resposta_marco;
        }
        else if (partida->questao_atual == 10 && partida->checkpoint < 10) {
            partida->checkpoint = 10;
            partida->ovos_restantes++;
            return resposta_marco;
        }
        else if (partida->questao_atual == 15) {
            partida->situacao = vitoria;
            return resposta_vitoria;
        }
        else {
            return resposta_acerto;
        }
    }
    else { //Resposta incorreta. Os ovos restantes determinam se a partida continua.
        if (partida->ovos_restantes > 0) {
            partida->ovos_restantes--;
            partida->acertos = partida->checkpoint;
            partida->questao_atual = partida->checkpoint;
            return resposta_renascimento;
        }
        else {
            partida->situacao = derrota;
            return resposta_derrota;
        } //caso em que já se acabaram os ovos
    }
} //PROCESSA RESPOSTA

/**
 * @brief Função que confirma a Retirada Tática quando a partida está em andamento.
 *        Registra a situação como retirada e entrega o último marco à interface.
 *
 * @param partida Ponteiro para a partida que o jogador confirmou encerrar.
 * @return int Retorna 0 na retirada antes do primeiro marco, 5 ou 10 conforme o último
 *         marco concluído e -1 quando a partida já está encerrada.
 */
int confirma_retirada(Partida *partida)
{
    if (partida->situacao == andamento) {
        partida->situacao = retirada;
        return partida->checkpoint;
    }
    return -1;
} //CONFIRMA RETIRADA

/**
 * @brief Função que calcula os pontos de vida restantes do príncipe a partir dos acertos.
 *        Com 8 acertos, a batalha começa com PV_MAX_PRINCIPE. Os acertos das questões 9 a 15
 *        reduzem os PV até zerar no golpe final. Após um renascimento, a função considera
 *        os acertos restaurados ao último marco.
 *
 * @param partida Ponteiro para a partida cujo progresso será consultado.
 * @return int Retorna os PV restantes para 8 a 15 acertos, incluindo 0 quando o príncipe é derrotado.
 *         Retorna -1 se a quantidade de acertos estiver fora desse intervalo.
 */
int calcula_pv_principe(const Partida *partida)
{
    switch (partida->acertos) {
        case 8:
            return PV_MAX_PRINCIPE;
        case 9:
            return 900;
        case 10:
            return 800;
        case 11:
            return 700;
        case 12:
            return 600;
        case 13:
            return 400;
        case 14:
            return 100;
        case 15:
            return 0;
        default:
            return -1; //Quantidade de acertos fora do intervalo da batalha.
    }
} //CALCULA PV PRÍNCIPE
