/**
 * @file arquivo_partidas.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Estruturação dos arquivos dos jogadores para Ranking
 * @version 0.1
 * @date 2026-11-24
 *
 * @copyright Copyright (c) 2026
 */

#ifndef ARQUIVO_PARTIDAS_H
#define ARQUIVO_PARTIDAS_H
#include "quiz.h"
#include <stdio.h>

typedef struct{
    char nome_jogador[100];
    Situacao_partida desfecho;
    int progresso;
    int ovos_restantes;
    double tempo_ativo;
} Registro_partida;


int partida_entra_no_ranking(const Partida *partida);

int preenche_registro_partida(Registro_partida *registro_partida, const Partida *partida, const char *nome_jogador);

int salvar_registro_partida(const Registro_partida *registro_partida, FILE **arquivo_ranking);

int carregar_registro_partidas(Registro_partida **registros_partidas, FILE **arquivo_ranking); //o ponteiro duplo no primeiro parâmetro permite retornar o vetor alocado para a interface

int registro_vem_antes(const Registro_partida *a, const Registro_partida *b);

void ordenar_registros_partidas(Registro_partida *registros_partidas, int quantidade);



#endif /*ARQUIVO_PARTIDAS_H*/
