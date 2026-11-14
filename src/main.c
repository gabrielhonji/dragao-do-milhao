/**
 * @file main.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 2026-11-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "perguntas/banco_perguntas.h"

int main(){

    Banco_perguntas banco_perguntas;
    int opcao;

    inicializa_banco(&banco_perguntas);
    
    do{
        printf("\nMENU DE CADASTRO\n");
        printf("    1 - Cadastrar Pergunta Nova\n");
        printf("    2 - Listar Perguntas Cadastradas\n");
        printf("    3 - Pesquisar Pergunta\n");
        printf("    4 - Alterar Pergunta\n");
        printf("    5 - Excluir Pergunta\n");
        printf("    6 - Pesquisar Perguntas pelo nível de Dificuldade\n");
        printf("    0 - Encerrar Cadastro\n");
        printf("Digite sua opção: ");
        scanf("%d", &opcao);

        switch(opcao){
            
            case 0:
                printf("Cadastro Encerrado\n");
                break;
        
            case 1:
                inserir_pergunta(&banco_perguntas);
                break;

            case 2:
                listar_perguntas(&banco_perguntas);
                break;
                
            //nos cases de 3 a 6 colocamos as instruções entre chaves, pois estamos declarando variáveis dentro dos cases, isso evita erros
            case 3:{
                int id_pergunta_procurada;
                int retorno_pesquisa;
                do{
                    printf("Digite o ID da pergunta procurada: ");
                    scanf("%d", &id_pergunta_procurada);
                    if(id_pergunta_procurada < 0){
                        printf("ID inválido\n");
                    }
                }while(id_pergunta_procurada < 0);
                retorno_pesquisa = pesquisar_pergunta(&banco_perguntas, id_pergunta_procurada); //retorna o indice de onde a pergunta está localizada no vetor
                if(retorno_pesquisa == -1){
                    printf("Pergunta não encontrada\n");
                }else{
                    exibir_pergunta(&banco_perguntas, retorno_pesquisa);
                }//else da procura da pesquisa
                break;}

            case 4:{
                int id_pergunta_alteracao;
                do{
                    printf("\nDigite o ID da pergunta a ser alterada: ");
                    scanf("%d", &id_pergunta_alteracao);
                    if(id_pergunta_alteracao < 0){
                        printf("ID inválido\n");
                    }
                }while(id_pergunta_alteracao < 0);
                alterar_pergunta(&banco_perguntas, id_pergunta_alteracao);
                break;}

            case 5:{
                int id_pergunta_excluida;
                do{
                    printf("\nDigite o ID da pergunta a ser excluída: ");
                    scanf("%d", &id_pergunta_excluida);
                    if(id_pergunta_excluida < 0){
                        printf("ID inválido\n");
                    }
                }while(id_pergunta_excluida < 0);
                excluir_pergunta(&banco_perguntas, id_pergunta_excluida);
                break;}

            case 6:{
                Dificuldade dificuldade_procurada;
                int opcao_dificuldade;
                printf("\nDigite o nível de dificuldade: \n");
                printf("    Muito Fácil   (1)\n");
                printf("    Fácil         (2)\n");
                printf("    Médio         (3)\n");
                printf("    Difícil       (4)\n");
                printf("    Muito Difícil (5)\n");
                do{
                    printf("Número: ");
                    scanf("%d", &opcao_dificuldade);
                    if(opcao_dificuldade < 1 || opcao_dificuldade > 5){
                        printf("Opção Inválida\n");
                    }
                }while(opcao_dificuldade < 1 || opcao_dificuldade > 5);
                dificuldade_procurada = opcao_dificuldade;
                listar_por_dificuldade(&banco_perguntas, dificuldade_procurada);
                break;}

            default:
                printf("Opção Inválida\n");
                break;
        }//switch
    }while(opcao != 0);

    free(banco_perguntas.perguntas);

    return  0;
}