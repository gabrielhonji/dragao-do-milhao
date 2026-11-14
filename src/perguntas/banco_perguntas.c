/**
 * @file banco_perguntas.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Banco de perguntas
 * @version 0.1
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "banco_perguntas.h"

 /**
  * @brief Função que inicializa o banco de perguntas como zerado
  * 
  * @param banco_perguntas 
  */
 void inicializa_banco (Banco_perguntas *banco_perguntas){
    banco_perguntas->perguntas = NULL; //inicializando as perguntas em NULL
    banco_perguntas->quant_perguntas = 0;
    banco_perguntas->capacidade_reservada = 0;
    banco_perguntas->proximo_id = 0;
 }//INICIALIZA BANCO


 /**
  * @brief Função que verifica a quantidade de armazenamento disponível para as perguntas, caso não haja, realoca e pede para o usuário cadastrar uma pergunta, chamando a função cadastro_pergunta para isso
  * 
  * @param banco_perguntas 
  */
 void inserir_pergunta(Banco_perguntas *banco_perguntas){

    Pergunta *perguntas_temp; //usado para realocações

    if(banco_perguntas->capacidade_reservada == banco_perguntas->quant_perguntas){

        banco_perguntas->capacidade_reservada += 1; //aumento de 1 em 1 a capacidade reservada para as perguntas

        perguntas_temp = realloc(banco_perguntas->perguntas, banco_perguntas->capacidade_reservada*sizeof(Pergunta));
        if(perguntas_temp == NULL){
            perror("Erro na realocação de memória");
            exit(EXIT_FAILURE);
        }else{
            banco_perguntas->perguntas = perguntas_temp; //se deu certo o realocamento, o meu vetor banco_perguntas assume o valor do temporário
        }
    }//conferindo a capacidade de perguntas

    cadastro_pergunta(banco_perguntas, banco_perguntas->quant_perguntas); //passo como indice a ultima posição do meu vetor de perguntas, que é a quantidade de perguntas totais novas

    banco_perguntas->perguntas[banco_perguntas->quant_perguntas].identificador = banco_perguntas->proximo_id; //atribuo o ID da pergunta à última posição disponível no vetor

    banco_perguntas->proximo_id++;
    banco_perguntas->quant_perguntas++;
 }//INSERIR PERGUNTA


 /**
  * @brief Função que percorre de 0 até a quantidade de perguntas e lista uma por uma
  * 
  * @param banco_perguntas 
  */
 void listar_perguntas(Banco_perguntas *banco_perguntas){

    if(banco_perguntas->quant_perguntas == 0){
         printf("Nenhuma pergunta cadastrada\n");
     }

    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        exibir_pergunta(banco_perguntas, i); //uso uma função cadastrada responsável por exibir as perguntas um a um
        printf("\n"); //estética
    }//for de listagem
 }//LISTAR PERGUNTAS


 /**
  * @brief Função que recebe o ID da pergunta e procura ela
  * 
  * @param banco_pergunta 
  */
 int pesquisar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_procurada){

    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        if(id_pergunta_procurada == banco_perguntas->perguntas[i].identificador){
            return i;
        }
    }//for

    return -1; //caso em que já percorreu todas as perguntas e não encontrou aquela com o id digitado
 }//PESQUISAR PERGUNTA


 /**
  * @brief Função que recebe um indice e exibi a pergunta cadastrada naquele indice 
  * 
  * @param banco_perguntas 
  * @param indice
  */
 void exibir_pergunta(Banco_perguntas *banco_perguntas, int indice){
    printf("\nPERGUNTA %d:\n", banco_perguntas->perguntas[indice].identificador);
        printf("    Enunciado: %s\n", banco_perguntas->perguntas[indice].enunciado);
        printf("    Alternativas: \n");
        printf("        Alternativa A: %s\n", banco_perguntas->perguntas[indice].alternativa.A);
        printf("        Alternativa B: %s\n", banco_perguntas->perguntas[indice].alternativa.B);
        printf("        Alternativa C: %s\n", banco_perguntas->perguntas[indice].alternativa.C);
        printf("        Alternativa D: %s\n", banco_perguntas->perguntas[indice].alternativa.D);
        printf("    Alternativa Correta: %c\n", banco_perguntas->perguntas[indice].resposta);
        printf("    Dificuldade: ");
        switch (banco_perguntas->perguntas[indice].dificuldade){
        case muito_facil:
            printf("Muito Fácil\n");
            break;
        
        case facil:
            printf("Fácil\n");
            break;

        case medio:
            printf("Médio\n");
            break;

        case dificil:
            printf("Difícil\n");
            break;

        case muito_dificil:
            printf("Muito Difícil\n");
            break;
        default:
            break;
        }//switch da dificuldade
        printf("    Dica: %s\n", banco_perguntas->perguntas[indice].dica);
 }//EXIBIR PERGUNTA



 /**
  * @brief Função que recebe o ID da pergunta a ser alterada e pede para o usuário digitar os dados dessa novamente
  * 
  * @param banco_perguntas 
  * @param id_pergunta_alteracao 
  */
 void alterar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_alteracao){
    int retorno_pesquisa; //indice de onde a pergunta está localizada no vetor
    retorno_pesquisa = pesquisar_pergunta(banco_perguntas, id_pergunta_alteracao);
    if(retorno_pesquisa == -1){
        printf("O ID da pergunta a ser alterada não foi encontrado\n");
        return;
    }
    cadastro_pergunta(banco_perguntas, retorno_pesquisa); //refaço o cadastro da pergunta, passando como indice a sua posição que foi retornada pela função anterior
    exibir_pergunta(banco_perguntas, retorno_pesquisa); //exibo a pergunta alterada
    return;
 }//ALTERAR PERGUNTA


 /**
  * @brief Função que solicita ao usuário os dados da pergunta
  * 
  * @param banco_perguntas 
  * @param indice 
  */
 void cadastro_pergunta(Banco_perguntas *banco_perguntas, int indice){
    int opcao; //variável auxiliar para atribuir as dificuldades usando enum

    setbuf(stdin, NULL);
    printf("Digite o enunciado: ");
    fgets(banco_perguntas->perguntas[indice].enunciado, 500, stdin);
    banco_perguntas->perguntas[indice].enunciado[strcspn(banco_perguntas->perguntas[indice].enunciado, "\n")] = '\0';
    setbuf(stdin, NULL);

    printf("ALTERNATIVAS: \n");
    printf("    Alternativa A: ");
    fgets(banco_perguntas->perguntas[indice].alternativa.A, 100, stdin);
    banco_perguntas->perguntas[indice].alternativa.A[strcspn(banco_perguntas->perguntas[indice].alternativa.A, "\n")] = '\0';
    setbuf(stdin, NULL);
    printf("    Alternativa B: ");
    fgets(banco_perguntas->perguntas[indice].alternativa.B, 100, stdin);
    banco_perguntas->perguntas[indice].alternativa.B[strcspn(banco_perguntas->perguntas[indice].alternativa.B, "\n")] = '\0';
    setbuf(stdin, NULL);
    printf("    Alternativa C: ");
    fgets(banco_perguntas->perguntas[indice].alternativa.C, 100, stdin);
    banco_perguntas->perguntas[indice].alternativa.C[strcspn(banco_perguntas->perguntas[indice].alternativa.C, "\n")] = '\0';
    setbuf(stdin, NULL);
    printf("    Alternativa D: ");
    fgets(banco_perguntas->perguntas[indice].alternativa.D, 100, stdin);
    banco_perguntas->perguntas[indice].alternativa.D[strcspn(banco_perguntas->perguntas[indice].alternativa.D, "\n")] = '\0';
    setbuf(stdin, NULL);

    do{//verificação da alternativa de resposta
        setbuf(stdin, NULL);//limpa o buffer
        printf("Alternativa Correta: ");
        scanf(" %c", &banco_perguntas->perguntas[indice].resposta); //esse espaço antes do %c garanta com que os enters não sejam armazenados
        if(banco_perguntas->perguntas[indice].resposta != 'A' && banco_perguntas->perguntas[indice].resposta != 'B' && banco_perguntas->perguntas[indice].resposta != 'C' && banco_perguntas->perguntas[indice].resposta != 'D'){
            printf("Opção Inválida\n");
        }
    }while(banco_perguntas->perguntas[indice].resposta != 'A' && banco_perguntas->perguntas[indice].resposta != 'B' && banco_perguntas->perguntas[indice].resposta != 'C' && banco_perguntas->perguntas[indice].resposta != 'D');

    printf("Dificuldade: \n");
    printf("    Muito Fácil   (1)\n");
    printf("    Fácil         (2)\n");
    printf("    Médio         (3)\n");
    printf("    Difícil       (4)\n");
    printf("    Muito Difícil (5)\n");
    do{//verificador
        printf("Número - ");
        scanf("%d", &opcao);
        if(opcao < 1 || opcao > 5){
            printf("Opção Inválida\n");
        }
    }while(opcao < 1 || opcao > 5);
    banco_perguntas->perguntas[indice].dificuldade = opcao;
    setbuf(stdin,NULL); //limpeza de buffer

    printf("Dica: ");
    fgets(banco_perguntas->perguntas[indice].dica, 200, stdin);
    banco_perguntas->perguntas[indice].dica[strcspn(banco_perguntas->perguntas[indice].dica, "\n")] = '\0';
    setbuf(stdin, NULL); //limpeza de buffer
 }//CADASTRO PERGUNTA

 
 /**
  * @brief Função que recebe o id da pergunta a ser excluída, verfica se o ID existe, caso exista, exclui a pergunta e desloca as restantes uma posição para a esquerda no vetor de perguntas
  * 
  * @param banco_perguntas 
  * @param id_pergunta_excluida 
  */
 void excluir_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_excluida){
    int retorno_pesquisa;

    retorno_pesquisa = pesquisar_pergunta(banco_perguntas, id_pergunta_excluida);
    if(retorno_pesquisa == -1){
        printf("O ID da pergunta a ser excluída não foi encontrado\n");
        return;
    }

    //for responsável por pegar todas as perguntas na frente da pergunta a ser excluída e deslocarem essas uma casa para a esquerda
    for(int i = retorno_pesquisa; i<banco_perguntas->quant_perguntas-1; i++){ //não percorro até a última posição, pois essa será excluida a partir do momento em que desloco todas as perguntas uma unidade para a esquerda
        banco_perguntas->perguntas[i] = banco_perguntas->perguntas[i+1];
    }

    banco_perguntas->quant_perguntas --; //como eu exluo uma pergunta, eu estou reduzindo a quantidade de perguntas totais, a capacidade para reservar se mantem a mesma, assim como o id

    printf("A pergunta de ID %d foi excluída\n", id_pergunta_excluida);
 }//EXCLUIR PERGUNTAS


 void listar_por_dificuldade(Banco_perguntas *banco_perguntas, Dificuldade dificuldade_procurada){
    int verificador = 0; //variável usada para ver se realmente possui perguntas naquela dificuldade ou não
    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        if(banco_perguntas->perguntas[i].dificuldade == dificuldade_procurada){
            exibir_pergunta(banco_perguntas, i);
            verificador++;
        }
    }//for que percorre todas as perguntas
    if(verificador == 0){
        printf("Nenhuma pergunta possui a dificuldade procurada\n");
    }
 }//LISTAR POR DIFICULDADE