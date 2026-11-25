/**
 * @file arquivo_partidas.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Definição das funções usadas para obter o ranking
 * @version 0.1
 * @date 2026-11-24
 *
 * @copyright Copyright (c) 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include "arquivo_partidas.h"
#include <string.h>

/**
 * @brief Verifica se a partida pode ser registrada no ranking.
 *        Entram as vitórias e as retiradas a partir do primeiro marco.
 *
 * @param partida Partida cujo desfecho e último marco serão consultados.
 * @return int Retorna 1 se a partida entra no ranking e 0 nos demais casos.
 */
int partida_entra_no_ranking(const Partida *partida)
{
    if (partida->situacao == vitoria || (partida->situacao == retirada && partida->checkpoint >= 5))
        return 1;
    return 0;
} //PARTIDA ENTRA NO RANKING

/**
 * @brief Preenche o registro com o nome informado e os dados finais da partida.
 *
 * @param registro_partida Registro que recebe o desfecho, os acertos, os ovos e o tempo ativo.
 * @param partida Partida encerrada da qual os dados serão copiados.
 * @param nome_jogador Nome informado pelo jogador, com 1 a 99 bytes antes de '\0'.
 * @return int Retorna 0 no sucesso, -1 se a partida não entra no ranking e -2 se o nome é inválido.
 */
int preenche_registro_partida(Registro_partida *registro_partida, const Partida *partida, const char *nome_jogador)
{
    int retorno_partida_entra_no_ranking = partida_entra_no_ranking(partida);
    int tamanho_nome;

    if (retorno_partida_entra_no_ranking == 0) {
        return -1;
    } //se a partida não entrar no ranking não prossigo

    tamanho_nome = strlen(nome_jogador);
    if (tamanho_nome == 0 || tamanho_nome >= 100)
        return -2; //retorno esse valor para exibir a mensagem correta de que o nome informado é inválido

    strcpy(registro_partida->nome_jogador, nome_jogador); //copio o nome do jogador para o registro

    registro_partida->desfecho = partida->situacao;

    registro_partida->progresso = partida->acertos;

    registro_partida->ovos_restantes = partida->ovos_restantes;

    registro_partida->tempo_ativo = partida->tempo_ativo;

    return 0;
} //PREENCHA REGISTRO PARTIDA

/**
 * @brief Acrescenta um registro ao final do arquivo binário do ranking.
 *
 * @param registro_partida Registro preenchido que será gravado.
 * @param arquivo_ranking Endereço do ponteiro usado para abrir e fechar o arquivo.
 * @return int Retorna 0 no sucesso e -1 em caso de falha na abertura ou na escrita.
 */
int salvar_registro_partida(const Registro_partida *registro_partida, FILE **arquivo_ranking)
{
    int retorno_fwrite; //variável para declarar se a escrita foi bem sucedida

    *arquivo_ranking = fopen("dados/ranking.dat", "ab"); //abro o arquivo para acrescentar o registro ao final
    if (*arquivo_ranking == NULL)
        return -1; //caso em que não abre o arquivo

    retorno_fwrite = fwrite(registro_partida, sizeof(Registro_partida), 1, *arquivo_ranking);
    if (retorno_fwrite != 1) {
        fclose(*arquivo_ranking); //fecho o arquivo
        *arquivo_ranking = NULL;
        return -1; //caso onde o registro falhou
    }

    int retorno_fechamento = fclose(*arquivo_ranking);
    *arquivo_ranking = NULL;
    return retorno_fechamento == 0 ? 0 : -1; // Retorna 0 ou -1 dependendo se o fechamento foi bem sucedido.
} //SALVAR REGISTRO PARTIDA

/**
 * @brief Carrega os registros do ranking e entrega o vetor já ordenado.
 *        Quem chama a função deve liberar o vetor recebido com free.
 *
 * @param registros_partidas Endereço do ponteiro que recebe o vetor alocado.
 *                          Recebe NULL se o arquivo estiver vazio.
 * @param arquivo_ranking Endereço do ponteiro usado para abrir e fechar o arquivo.
 * @return int Retorna a quantidade de registros lidos, incluindo 0 para arquivo vazio.
 *             Retorna -1 em caso de falha na leitura, registro incompleto ou falha de alocação,
 *             e -2 se o arquivo não puder ser aberto.
 */
int carregar_registro_partidas(Registro_partida **registros_partidas, FILE **arquivo_ranking)
{

    int retorno_fread;
    Registro_partida *vetor = NULL; //vetor que armazena todas as partidas lidas antes de passar para o vetor original
    Registro_partida *vetor_temp;   //variável usado para fazer os realocs
    Registro_partida registro_lido; //armazena a partida lida naquele momento
    int contador_partidas_lidas = 0;

    *arquivo_ranking = fopen("dados/ranking.dat", "rb"); //abrindo o arquivo binário no modo rb
    if (*arquivo_ranking == NULL)
        return -2; //caso em que não abre o arquivo

    do {
        retorno_fread = fread(&registro_lido, 1, sizeof(registro_lido), *arquivo_ranking); //leio elementos de um byte para obter a quantidade de bytes lidos

        if (retorno_fread == sizeof(registro_lido)) { //comparo com a quantidade de bytes

            vetor_temp = realloc(vetor, (contador_partidas_lidas + 1) * sizeof(Registro_partida)); //realoco o vetor para guardar mais uma partida lida
            if (vetor_temp == NULL) {
                free(vetor);              //libero o vetor
                fclose(*arquivo_ranking); //fecho o arquivo
                *arquivo_ranking = NULL;
                return -1; //caso onde o registro falhou
            }
            else {
                vetor = vetor_temp;                             //como o realocamento deu certo assumo o vetor como o valor do temp
                vetor[contador_partidas_lidas] = registro_lido; //no meu vetor coloco na posição que eu li, aquela partida em si
                contador_partidas_lidas++;                      //aumento 1 ao meu contador de partidas lidas
            }
        }
        else if (retorno_fread == 0) {
            if (ferror(*arquivo_ranking) != 0) { //verifco se houve erro, caso não houve o arquivo só chegou ao fim
                free(vetor);                     //libero o vetor
                fclose(*arquivo_ranking);        //fecho o arquivo
                *arquivo_ranking = NULL;
                return -1; //caso onde o registro falhou
            }
            break; //se ele devolve 0, significa que chegou ao fim normalmente, quebro o laço
        }
        else {
            free(vetor);              //libero o vetor
            fclose(*arquivo_ranking); //fecho o arquivo
            *arquivo_ranking = NULL;
            return -1; //caso onde o registro falhou
        }
    } while (retorno_fread != 0);

    *registros_partidas = vetor;

    fclose(*arquivo_ranking); //fecho o arquivo
    *arquivo_ranking = NULL;

    if(contador_partidas_lidas > 1){ //com 0 ou 1 registro lido o ranking já está em ordem
        ordenar_registros_partidas(*registros_partidas, contador_partidas_lidas); //chamo a função que organiza os registros para deixarem na ordem correta
    }

    return contador_partidas_lidas; //retorno quantos registros foram lidos
} //CARREGAR REGISTRO PARTIDAS

/**
 * @brief Compara dois registros pelo progresso, pelos ovos restantes e pelo menor tempo ativo.
 *
 * @param a Primeiro registro da comparação.
 * @param b Segundo registro da comparação.
 * @return int Retorna 1 se a deve vir antes de b e 0 se vem depois ou se há empate.
 */
int registro_vem_antes(const Registro_partida *a, const Registro_partida *b)
{

    if (a->progresso > b->progresso)
        return 1;

    if (a->progresso == b->progresso) {
        if (a->ovos_restantes > b->ovos_restantes)
            return 1;

        if (a->ovos_restantes == b->ovos_restantes) {
            if (a->tempo_ativo < b->tempo_ativo)
                return 1;
        }
    }

    return 0;
} //REGISTRO VEM ANTES

/**
 * @brief Ordena o vetor pelo maior progresso, mais ovos restantes e menor tempo ativo.
 *        Mantém a ordem de leitura dos registros que empatam nos três critérios.
 *
 * @param registros_partidas Vetor de registros que será reorganizado.
 * @param quantidade Quantidade de registros do vetor.
 */
void ordenar_registros_partidas(Registro_partida *registros_partidas, int quantidade){

    int retorno_funcao;
    Registro_partida variavel_auxiliar;

    for(int i = 0; i < (quantidade-1); i++){ //for que repassa a comparação entre os vizinhos nos casos em que eles foram trocados de posição
        for(int j = 0; j < (quantidade-1-i); j++){ //for que compara os vizinhos
            retorno_funcao = registro_vem_antes(&registros_partidas[j+1], &registros_partidas[j]); //estou verificando se o próximo registro deve vir antes ou não, se deve vir, a função retorna 1
            if(retorno_funcao == 1){
                variavel_auxiliar = registros_partidas[j];
                registros_partidas[j] = registros_partidas[j+1];
                registros_partidas[j+1] = variavel_auxiliar;
            }//troca da posição dos vetores
        }//for interno
    }//for externo
}//ORDENAR REGISTROS PARTIDAS
