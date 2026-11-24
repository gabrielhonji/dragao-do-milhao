/**
 * @file main.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Coordena o modo visual de teste, o cadastro pelo terminal e a carga e o salvamento do banco de perguntas.
 * @version 0.1
 * @date 2026-11-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "perguntas/banco_perguntas.h"
#include "perguntas/arquivos_perguntas.h"
#include "interface/telas.h"

void cadastro_pergunta(Pergunta *pergunta); //Lê os dados da pergunta pelo terminal.

/**
 * @brief Seleciona o modo de execução. No modo terminal, carrega o banco, executa o menu e salva os dados ao sair.
 *
 * @param quantidade_argumentos Quantidade de argumentos recebidos na execução.
 * @param argumentos Vetor de argumentos. A opção --visual abre o modo visual de teste.
 * @return int Retorna 0 no encerramento normal.
 */
int main(int quantidade_argumentos, char *argumentos[]){
    if (quantidade_argumentos > 1 && strcmp(argumentos[1], "--visual") == 0) {
        executar_tela_teste();
        return 0;
    }

    Banco_perguntas banco_perguntas;
    FILE *arquivos_csv = NULL;
    int retorno_carregar_csv;
    int opcao;
    FILE *arquivo_dat = NULL;
    int retorno_binario;
    FILE *arquivo_exportacao_csv = NULL;
    int retorno_exportar_csv;
    
    inicializa_banco(&banco_perguntas);
    
    retorno_binario = carregar_binario(&banco_perguntas, &arquivo_dat);
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
        
            case 1:{
                Pergunta pergunta;
                int retorno_insercao;
                //Preenche a pergunta no terminal antes de enviá-la ao banco.
                cadastro_pergunta(&pergunta);
                retorno_insercao = inserir_pergunta(&banco_perguntas, &pergunta);
                if(retorno_insercao == -1){
                    printf("Falha na realocação de memória\n");
                }else if(retorno_insercao == -3){
                    printf("A pergunta cadastrada não é válida\n");
                } else{
                    printf("Pergunta cadastrada com sucesso\n");
                }
                break;}

            case 2:
                listar_perguntas(&banco_perguntas);
                break;
                
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
                Pergunta pergunta_alterada;
                int id_pergunta_alteracao;
                int retorno_alteracao;
                do{
                    printf("\nDigite o ID da pergunta a ser alterada: ");
                    scanf("%d", &id_pergunta_alteracao);
                    if(id_pergunta_alteracao < 0){
                        printf("ID inválido\n");
                    }
                }while(id_pergunta_alteracao < 0);
                //O ID identifica o registro. A estrutura recebe os novos dados.
                cadastro_pergunta(&pergunta_alterada);
                retorno_alteracao = alterar_pergunta(&banco_perguntas, id_pergunta_alteracao, &pergunta_alterada);
                if(retorno_alteracao == -1){
                    printf("Pergunta não encontrada\n");
                }else if(retorno_alteracao == -3){
                    printf("A alteração feita não é válida\n");
                } else{
                    printf("Pergunta alterada com sucesso\n");
                }
                break;}

            case 5:{
                int id_pergunta_excluida;
                int retorno_exclusao;
                do{
                    printf("\nDigite o ID da pergunta a ser excluída: ");
                    scanf("%d", &id_pergunta_excluida);
                    if(id_pergunta_excluida < 0){
                        printf("ID inválido\n");
                    }
                }while(id_pergunta_excluida < 0);
                retorno_exclusao = excluir_pergunta(&banco_perguntas, id_pergunta_excluida);
                if(retorno_exclusao == -1){
                    printf("Pergunta não encontrada\n");
                }else if(retorno_exclusao == -2){
                    printf("Falha na realocação de memória\n");
                } else{
                    printf("Pergunta excluída com sucesso\n");
                }
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

    retorno_binario = salvar_binario(&banco_perguntas, &arquivo_dat);
    if(retorno_binario == -1){
        printf("Erro ao salvar arquivo binário\n");
    }else{
        printf("Arquivo binário criado\n");
    }

    free(banco_perguntas.perguntas);
    free(banco_perguntas.ids_disponiveis);

    return  0;
}



/**
 * @brief Função que lê o enunciado, as alternativas, a resposta e a dificuldade pelo terminal.
 *        Repete a leitura da resposta quando a letra estiver fora de A a D e a da dificuldade
 *        quando o número estiver fora do intervalo de 1 a 5.
 *
 * @param pergunta Ponteiro para a estrutura que receberá os dados lidos. O banco controla o identificador.
 */
 void cadastro_pergunta(Pergunta *pergunta){
    int opcao; //Armazena o número lido antes de atribuí-lo à dificuldade.
    int caracter;

    setbuf(stdin, NULL);
    printf("Digite o enunciado: ");
    fgets(pergunta->enunciado, 500, stdin);
    pergunta->enunciado[strcspn(pergunta->enunciado, "\n")] = '\0';
    setbuf(stdin, NULL);

    printf("ALTERNATIVAS: \n");
    printf("    Alternativa A: ");
    fgets(pergunta->alternativa.A, 100, stdin);
    pergunta->alternativa.A[strcspn(pergunta->alternativa.A, "\n")] = '\0';
    setbuf(stdin, NULL);
    printf("    Alternativa B: ");
    fgets(pergunta->alternativa.B, 100, stdin);
    pergunta->alternativa.B[strcspn(pergunta->alternativa.B, "\n")] = '\0';
    setbuf(stdin, NULL);
    printf("    Alternativa C: ");
    fgets(pergunta->alternativa.C, 100, stdin);
    pergunta->alternativa.C[strcspn(pergunta->alternativa.C, "\n")] = '\0';
    setbuf(stdin, NULL);
    printf("    Alternativa D: ");
    fgets(pergunta->alternativa.D, 100, stdin);
    pergunta->alternativa.D[strcspn(pergunta->alternativa.D, "\n")] = '\0';
    setbuf(stdin, NULL);

    do{//Verifica a letra da alternativa correta.
        printf("Alternativa Correta: ");
        scanf(" %c", &pergunta->resposta); //O espaço antes de %c ignora espaços e quebras de linha anteriores.
        do{
            caracter = getchar();
        }while(caracter != '\n' && caracter != EOF); //Descarta o restante da linha após a resposta.
        if(pergunta->resposta != 'A' && pergunta->resposta != 'B' && pergunta->resposta != 'C' && pergunta->resposta != 'D'){
            printf("Opção Inválida\n");
        }
    }while(pergunta->resposta != 'A' && pergunta->resposta != 'B' && pergunta->resposta != 'C' && pergunta->resposta != 'D');

    printf("Dificuldade: \n");
    printf("    Muito Fácil   (1)\n");
    printf("    Fácil         (2)\n");
    printf("    Médio         (3)\n");
    printf("    Difícil       (4)\n");
    printf("    Muito Difícil (5)\n");
    do{//Verifica o nível de dificuldade.
        printf("Número - ");
        scanf("%d", &opcao);
        setbuf(stdin, NULL); //Configura a entrada sem buffer.
        if(opcao < 1 || opcao > 5){
            printf("Opção Inválida\n");
        }
    }while(opcao < 1 || opcao > 5);
    pergunta->dificuldade = opcao;
    setbuf(stdin,NULL); //Configura a entrada sem buffer.

 }//CADASTRO PERGUNTA
