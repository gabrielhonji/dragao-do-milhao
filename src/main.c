/**
 * @file main.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Carrega o banco de perguntas e abre a interface gráfica ou o cadastro pelo terminal.
 * @version 0.1
 * @date 2026-11-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "perguntas/banco_perguntas.h"
#include "perguntas/arquivos_perguntas.h"
#include "interface/telas.h"

static int cadastro_pergunta(Pergunta *pergunta);
static void descartar_linha(void);

/**
 * @brief Carrega o banco salvo ou importa o CSV quando o binário ainda não existe.
 * @param banco Banco inicializado que recebe as perguntas.
 * @param arquivo_dat Endereço do ponteiro usado pelas operações do binário.
 * @return Verdadeiro quando o banco está disponível para abrir o menu.
 */
static bool preparar_banco_perguntas(Banco_perguntas *banco, FILE **arquivo_dat)
{
    int retorno = carregar_binario(banco, arquivo_dat);
    if (retorno == -2) {
        puts("Banco de perguntas ausente. Carregando o CSV inicial.");
        FILE *arquivo_csv = NULL;
        int quantidade = carregar_csv(banco, &arquivo_csv);
        if (quantidade < 0) {
            fputs("Erro ao ler dados/perguntas_iniciais.csv.\n", stderr);
            return false;
        }
        printf("Foram lidas %d perguntas do arquivo .csv\n", quantidade);

        retorno = salvar_binario_inicial(banco, arquivo_dat);
        if (retorno == 0) {
            puts("Arquivo binário criado");
            return true;
        }
        if (retorno < 0) {
            fputs("Erro ao criar dados/perguntas.dat.\n", stderr);
            return false;
        }

        // Outra execução criou o banco depois da nossa tentativa de leitura.
        retorno = carregar_binario(banco, arquivo_dat);
    }

    if (retorno != 0) {
        fputs("Erro ao carregar dados/perguntas.dat.\n", stderr);
        return false;
    }
    puts("Arquivo binário carregado");
    return true;
}

/**
 * @brief Prepara o banco e inicia o modo de execução solicitado.
 * @param quantidade_argumentos Quantidade de argumentos, incluindo o nome do programa.
 * @param argumentos Argumentos de execução. --terminal abre o cadastro por texto.
 * @return EXIT_FAILURE se os argumentos forem inválidos ou se o banco não puder ser preparado. Retorna 0 ao encerrar a sessão.
 */
int main(int quantidade_argumentos, char *argumentos[])
{
    int modo_terminal = quantidade_argumentos == 2 && strcmp(argumentos[1], "--terminal") == 0;
    int modo_visual = quantidade_argumentos == 1 ||
                      (quantidade_argumentos == 2 && strcmp(argumentos[1], "--visual") == 0);
    if (!modo_terminal && !modo_visual) {
        fprintf(stderr, "Uso: %s [--terminal]\n", argumentos[0]);
        return EXIT_FAILURE;
    }

    Banco_perguntas banco_perguntas;
    int opcao;
    FILE *arquivo_dat = NULL;
    FILE *arquivo_exportacao_csv = NULL;
    int retorno_exportar_csv;

    inicializa_banco(&banco_perguntas);

    if (!preparar_banco_perguntas(&banco_perguntas, &arquivo_dat)) {
        free(banco_perguntas.perguntas);
        free(banco_perguntas.ids_disponiveis);
        return EXIT_FAILURE;
    }

    if (modo_visual) {
        srand((unsigned int)time(NULL));
        executar_tela_teste(&banco_perguntas);

        free(banco_perguntas.perguntas);
        free(banco_perguntas.ids_disponiveis);
        return 0;
    }

    bool banco_alterado = false;
    do {
        printf("\nMENU DE CADASTRO\n");
        printf("    1 - Cadastrar Pergunta Nova\n");
        printf("    2 - Listar Perguntas Cadastradas\n");
        printf("    3 - Pesquisar Pergunta\n");
        printf("    4 - Alterar Pergunta\n");
        printf("    5 - Excluir Pergunta\n");
        printf("    6 - Pesquisar Perguntas pelo nível de Dificuldade\n");
        printf("    7 - Exportar CSV\n");
        printf("    0 - Encerrar Cadastro\n");
        do {
            printf("Digite sua opção: ");
            int leu = scanf("%d", &opcao);
            if (leu == EOF) {
                opcao = 0;
                break;
            }
            if (leu != 1) {
                opcao = -1;
                descartar_linha();
            }
            printf("\n");
            if (opcao != 0 && opcao != 1 && opcao != 2 && opcao != 3 && opcao != 4 && opcao != 5 && opcao != 6 && opcao != 7) {
                printf("Opção Inválida\n");
            }
        } while (opcao != 0 && opcao != 1 && opcao != 2 && opcao != 3 && opcao != 4 && opcao != 5 && opcao != 6 && opcao != 7);

        switch (opcao) {

            case 0:
                printf("Cadastro Encerrado\n");
                break;

            case 1: {
                Pergunta pergunta = {0};
                if (!cadastro_pergunta(&pergunta))
                    break;
                int resultado = inserir_pergunta(&banco_perguntas, &pergunta);
                if (resultado == 0)
                    banco_alterado = true;
                puts(resultado == 0 ? "Pergunta cadastrada com sucesso." : "Não foi possível cadastrar a pergunta.");
                break;
            }

            case 2:
                listar_perguntas(&banco_perguntas);
                break;

            // Os blocos delimitam o escopo das variáveis usadas em cada opção.
            case 3: {
                int id_pergunta_procurada;
                int retorno_pesquisa;
                do {
                    printf("Digite o ID da pergunta procurada: ");
                    scanf("%d", &id_pergunta_procurada);
                    if (id_pergunta_procurada < 0) {
                        printf("ID inválido\n");
                    }
                } while (id_pergunta_procurada < 0);
                retorno_pesquisa = pesquisar_pergunta(&banco_perguntas, id_pergunta_procurada); //retorna o índice de onde a pergunta está localizada no vetor
                if (retorno_pesquisa == -1) {
                    printf("Pergunta não encontrada\n");
                }
                else {
                    exibir_pergunta(&banco_perguntas, retorno_pesquisa);
                } //else da procura da pesquisa
                break;
            }

            case 4: {
                int id_pergunta_alteracao;
                do {
                    printf("\nDigite o ID da pergunta a ser alterada: ");
                    scanf("%d", &id_pergunta_alteracao);
                    if (id_pergunta_alteracao < 0) {
                        printf("ID inválido\n");
                    }
                } while (id_pergunta_alteracao < 0);
                if (pesquisar_pergunta(&banco_perguntas, id_pergunta_alteracao) < 0) {
                    puts("Pergunta não encontrada.");
                    break;
                }
                Pergunta pergunta = {0};
                if (!cadastro_pergunta(&pergunta))
                    break;
                int resultado = alterar_pergunta(&banco_perguntas, id_pergunta_alteracao, &pergunta);
                if (resultado == 0)
                    banco_alterado = true;
                puts(resultado == 0 ? "Pergunta alterada com sucesso." : "Não foi possível alterar a pergunta.");
                break;
            }

            case 5: {
                int id_pergunta_excluida;
                do {
                    printf("\nDigite o ID da pergunta a ser excluída: ");
                    scanf("%d", &id_pergunta_excluida);
                    if (id_pergunta_excluida < 0) {
                        printf("ID inválido\n");
                    }
                } while (id_pergunta_excluida < 0);
                int resultado = excluir_pergunta(&banco_perguntas, id_pergunta_excluida);
                if (resultado == 0)
                    banco_alterado = true;
                puts(resultado == 0 ? "Pergunta excluída com sucesso." : "Não foi possível excluir a pergunta.");
                break;
            }

            case 6: {
                Dificuldade dificuldade_procurada;
                int opcao_dificuldade;
                printf("\nDigite o nível de dificuldade: \n");
                printf("    Muito Fácil   (1)\n");
                printf("    Fácil         (2)\n");
                printf("    Médio         (3)\n");
                printf("    Difícil       (4)\n");
                printf("    Muito Difícil (5)\n");
                do {
                    printf("Número: ");
                    scanf("%d", &opcao_dificuldade);
                    if (opcao_dificuldade < 1 || opcao_dificuldade > 5) {
                        printf("Opção Inválida\n");
                    }
                } while (opcao_dificuldade < 1 || opcao_dificuldade > 5);
                dificuldade_procurada = opcao_dificuldade;
                listar_por_dificuldade(&banco_perguntas, dificuldade_procurada);
                break;
            }

            case 7:
                retorno_exportar_csv = exportar_csv(&banco_perguntas, &arquivo_exportacao_csv);
                if (retorno_exportar_csv == -1) {
                    printf("Erro na gravação dos dados no arquivo CSV\n");
                }
                else {
                    printf("Os dados foram exportados para o arquivo CSV\n");
                }
                break;

            default:
                printf("Opção Inválida\n");
                break;
        } //switch
    } while (opcao != 0);

    if (banco_alterado) {
        if (salvar_binario(&banco_perguntas, &arquivo_dat) == -1)
            puts("Erro ao salvar arquivo binário");
        else
            puts("Arquivo binário atualizado");
    }

    free(banco_perguntas.perguntas);
    free(banco_perguntas.ids_disponiveis);

    return 0;
}


/** @brief Descarta o restante da linha após uma leitura no terminal. */
static void descartar_linha(void)
{
    int caractere;
    do {
        caractere = getchar();
    } while (caractere != '\n' && caractere != EOF);
}

/**
 * @brief Lê os campos da pergunta no terminal para cadastro ou alteração.
 * @param pergunta Estrutura que recebe o enunciado, as alternativas, o gabarito e a dificuldade.
 * @return 1 quando completa a leitura e 0 quando a entrada termina antes disso.
 */
static int cadastro_pergunta(Pergunta *pergunta)
{
    const char *pedidos[] = {"Enunciado", "Alternativa A", "Alternativa B", "Alternativa C", "Alternativa D"};
    char *campos[] = {pergunta->enunciado, pergunta->alternativa.A, pergunta->alternativa.B,
                      pergunta->alternativa.C, pergunta->alternativa.D};

    descartar_linha(); // O scanf do menu deixa a quebra de linha pendente.
    for (int i = 0; i < 5; i++) {
        int tamanho = i == 0 ? sizeof pergunta->enunciado : sizeof pergunta->alternativa.A;
        printf("%s: ", pedidos[i]);
        if (fgets(campos[i], tamanho, stdin) == NULL)
            return 0;
        if (strchr(campos[i], '\n') == NULL)
            descartar_linha();
        campos[i][strcspn(campos[i], "\r\n")] = '\0';
    }

    do {
        printf("Alternativa correta (A a D): ");
        if (scanf(" %c", &pergunta->resposta) != 1)
            return 0;
        descartar_linha();
    } while (pergunta->resposta < 'A' || pergunta->resposta > 'D');

    for (;;) {
        int dificuldade;
        printf("Dificuldade (1 a 5): ");
        int leu = scanf("%d", &dificuldade);
        if (leu == EOF)
            return 0;
        descartar_linha();
        if (leu == 1 && dificuldade >= muito_facil && dificuldade <= muito_dificil) {
            pergunta->dificuldade = dificuldade;
            return 1;
        }
    }
}
