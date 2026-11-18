/**
 * @file main.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Main do projeto Dragão do Milhão - chama o menu de cadastro inicial e as funções correspondentes a este, a main também é responsável pela carga e salvamento dos dados nos respectivos arquivos
 * @version 0.1
 * @date 2026-11-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "perguntas/banco_perguntas.h"
#include "perguntas/arquivos_perguntas.h"

int main(){

    Banco_perguntas banco_perguntas;
    FILE *arquivos_csv = NULL;
    int retorno_carregar_csv;
    int opcao;
    FILE *arquivo_bin = NULL;
    int retorno_binario;
    FILE *arquivo_exportacao_csv = NULL;
    int retorno_exportar_csv;
    
    inicializa_banco(&banco_perguntas);
    
    retorno_binario = carregar_binario(&banco_perguntas, &arquivo_bin);
    if(retorno_binario == -2){//o arquivo binário não existir significa que estou na primeira execução, nesse caso leio as perguntas do CSV.
        printf("Não foi possível abrir o binário.Tentando carregar o CSV\n"); //falha na abertura do arquivo binário
        retorno_carregar_csv = carregar_csv(&banco_perguntas, &arquivos_csv);
        if(retorno_carregar_csv == -1){
            free(banco_perguntas.perguntas);
            perror("Erro na abertura do arquivo CSV");
            exit(EXIT_FAILURE);//caso de erro no carregamento do arquivo CSV, eu saio do programa e imprimo essa mensagem de erro
        }else{
            printf("\nForam lidas %d perguntas do arquivo .csv\n", retorno_carregar_csv); //teste 
        }
    }else if(retorno_binario == -1){ //erro na abertura do arquivo binário
        free(banco_perguntas.perguntas);
        perror("Erro na abertura do arquivo binário");
        exit(EXIT_FAILURE);//caso de erro no carregamento do arquivo binário, eu saio do programa e imprimo essa mensagem de erro
    }else{
        printf("Arquivo binário carregado\n");
    }
    

    do{
        printf("\nMENU DE CADASTRO\n");
        printf("    1 - Cadastrar Pergunta Nova\n");
        printf("    2 - Listar Perguntas Cadastradas\n");
        printf("    3 - Pesquisar Pergunta\n");
        printf("    4 - Alterar Pergunta\n");
        printf("    5 - Excluir Pergunta\n");
        printf("    6 - Pesquisar Perguntas pelo nível de Dificuldade\n");
        printf("    7 - Exportar CSV\n");
        printf("    0 - Encerrar Cadastro\n");
        do{
            printf("Digite sua opção: ");
            scanf("%d", &opcao);
            printf("\n");
            if(opcao != 0 && opcao != 1 && opcao != 2 && opcao != 3 && opcao != 4 && opcao != 5 && opcao != 6 && opcao != 7){
                printf("Opção Inválida\n");
            }
        }while(opcao != 0 && opcao != 1 && opcao != 2 && opcao != 3 && opcao != 4 && opcao != 5 && opcao != 6 && opcao != 7);

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
                retorno_pesquisa = pesquisar_pergunta(&banco_perguntas, id_pergunta_procurada); //retorna o índice de onde a pergunta está localizada no vetor
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

            case 7: 
                retorno_exportar_csv = exportar_csv(&banco_perguntas, &arquivo_exportacao_csv);
                if(retorno_exportar_csv == -1){
                    printf("Erro na gravação dos dados no arquivo CSV\n");
                }else{
                    printf("Os dados foram exportados para o arquivo CSV\n");
                }
                break;

            default:
                printf("Opção Inválida\n");
                break;
        }//switch
    }while(opcao != 0);

    retorno_binario = salvar_binario(&banco_perguntas, &arquivo_bin);
    if(retorno_binario == -1){
        printf("Erro ao salvar arquivo binário\n");
    }else{
        printf("Arquivo binário criado\n");
    }

    free(banco_perguntas.perguntas);

    return  0;
}