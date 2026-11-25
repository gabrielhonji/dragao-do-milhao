/**
 * @file banco_perguntas.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Implementa o cadastro, a consulta, a alteração e a exclusão de perguntas.
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
  * @brief Inicializa um banco vazio, sem vetores alocados.
  * 
  * @param banco_perguntas Ponteiro para o banco de perguntas.
  */
 void inicializa_banco (Banco_perguntas *banco_perguntas){
    banco_perguntas->perguntas = NULL; //inicializando as perguntas em NULL
    banco_perguntas->quant_perguntas = 0;
    banco_perguntas->capacidade_reservada = 0;
    banco_perguntas->proximo_id = 0;
    banco_perguntas->ids_disponiveis = NULL;
    banco_perguntas->quant_ids_disponiveis= 0;
 }//INICIALIZA BANCO


 /**
  * @brief Função que valida e cadastra uma cópia da pergunta recebida no vetor do banco. Amplia a capacidade se necessário e atribui o primeiro ID disponível ou, se a fila estiver vazia, o próximo ID novo. Ao concluir, aumenta a quantidade de perguntas.
  *
  * @param banco_perguntas Ponteiro para o banco inicializado que receberá a nova pergunta.
  * @param pergunta Ponteiro para a pergunta preenchida pelo chamador. O banco atribui o ID da cópia.
  * @return int Retorna 0 em caso de sucesso, -1 se a realocação falhar e -3 se a pergunta for inválida.
  */
 int inserir_pergunta(Banco_perguntas *banco_perguntas, Pergunta *pergunta){

    Pergunta *perguntas_temp; //Recebe o resultado da realocação do vetor.
    int retorno_pergunta_valida;

    retorno_pergunta_valida = pergunta_valida(pergunta);
    if(retorno_pergunta_valida == -3){
        return -3;
    }

    if(banco_perguntas->capacidade_reservada == banco_perguntas->quant_perguntas){

        perguntas_temp = realloc(banco_perguntas->perguntas, (banco_perguntas->capacidade_reservada + 1)*sizeof(Pergunta));
        if(perguntas_temp == NULL){
            return -1;
        }else{
            banco_perguntas->perguntas = perguntas_temp;
            banco_perguntas->capacidade_reservada += 1; //Atualiza a capacidade após a realocação.
        }
    }

    banco_perguntas->perguntas[banco_perguntas->quant_perguntas] = *pergunta;

    if(banco_perguntas->quant_ids_disponiveis > 0){ //Reaproveita o primeiro ID da fila.
        banco_perguntas->perguntas[banco_perguntas->quant_perguntas].identificador = banco_perguntas->ids_disponiveis[0];

        for(int i = 0; i<(banco_perguntas->quant_ids_disponiveis-1); i++){
            banco_perguntas->ids_disponiveis[i] = banco_perguntas->ids_disponiveis[i+1];
        } //Desloca os IDs restantes para manter a ordem da fila.
        banco_perguntas->quant_ids_disponiveis--;
    }else{
        banco_perguntas->perguntas[banco_perguntas->quant_perguntas].identificador = banco_perguntas->proximo_id;
        banco_perguntas->proximo_id++;
    }

    banco_perguntas->quant_perguntas++;

    return 0;
 }//INSERIR PERGUNTA


 /**
  * @brief Exibe no terminal todas as perguntas na ordem do vetor.
  * 
  * @param banco_perguntas Ponteiro para o banco de perguntas.
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
  * @brief Localiza a posição de uma pergunta pelo seu ID.
  * 
  * @param banco_perguntas Ponteiro para o banco de perguntas.
  * @param id_pergunta_procurada ID da pergunta procurada.
  * @return Índice encontrado ou -1 se o ID não estiver cadastrado.
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
  * @brief Exibe no terminal os dados da pergunta na posição indicada.
  * 
  * @param banco_perguntas Ponteiro para o banco de perguntas.
  * @param indice Índice válido no vetor de perguntas.
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
 }//EXIBIR PERGUNTA



 /**
  * @brief Função que localiza uma pergunta pelo ID e valida os novos dados antes de copiá-los. A pergunta é atualizada na posição encontrada e mantém o identificador original.
  *
  * @param banco_perguntas Ponteiro para o banco que contém a pergunta cadastrada.
  * @param id_pergunta_alteracao ID da pergunta que será alterada.
  * @param pergunta_alterada Ponteiro para a estrutura preenchida com os novos dados.
  * @return int Retorna 0 em caso de sucesso, -1 se o ID não for encontrado e -3 se os dados forem inválidos.
  */
 int alterar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_alteracao, Pergunta *pergunta_alterada){
    int retorno_pesquisa; //Índice da pergunta localizada pelo ID.
    int retorno_pergunta_valida;

    retorno_pesquisa = pesquisar_pergunta(banco_perguntas, id_pergunta_alteracao);
    if(retorno_pesquisa == -1){
        return -1;
    }

    retorno_pergunta_valida = pergunta_valida(pergunta_alterada);
    if(retorno_pergunta_valida == -3){
        return -3;
    }
    banco_perguntas->perguntas[retorno_pesquisa] = *pergunta_alterada;
    banco_perguntas->perguntas[retorno_pesquisa].identificador = id_pergunta_alteracao;
    return 0;
 }//ALTERAR PERGUNTA
 

 
 /**
  * @brief Função que exclui uma pergunta pelo ID e reserva seu identificador para reaproveitamento. Depois de adicionar o ID ao fim da fila, desloca as perguntas seguintes no vetor e reduz a quantidade cadastrada.
  *
  * @param banco_perguntas Ponteiro para o banco que contém a pergunta cadastrada e a fila de IDs disponíveis.
  * @param id_pergunta_excluida ID da pergunta que será excluída.
  * @return int Retorna 0 em caso de sucesso, -1 se o ID não for encontrado e -2 se a realocação da fila falhar.
  */
 int excluir_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_excluida){
    int retorno_pesquisa;
    int *ids_disponiveis_temp;

    retorno_pesquisa = pesquisar_pergunta(banco_perguntas, id_pergunta_excluida);
    if(retorno_pesquisa == -1){
        return -1;
    }
    
    ids_disponiveis_temp = realloc(banco_perguntas->ids_disponiveis, (banco_perguntas->quant_ids_disponiveis+1)*sizeof(int));
    if(ids_disponiveis_temp == NULL){
        return -2;
    }else{
        banco_perguntas->ids_disponiveis = ids_disponiveis_temp;
    }
    banco_perguntas->ids_disponiveis[banco_perguntas->quant_ids_disponiveis] = id_pergunta_excluida; //Adiciona o ID ao fim da fila.
    banco_perguntas->quant_ids_disponiveis ++;


    //Preenche a posição da pergunta excluída deslocando as seguintes para a esquerda.
    for(int i = retorno_pesquisa; i<banco_perguntas->quant_perguntas-1; i++){
        banco_perguntas->perguntas[i] = banco_perguntas->perguntas[i+1];
    }
    
    banco_perguntas->quant_perguntas --;

    return 0;
 }//EXCLUIR PERGUNTAS



 /**
  * @brief Permite avançar e voltar entre as perguntas da dificuldade escolhida no terminal.
  * 
  * @param banco_perguntas Ponteiro para o banco de perguntas.
  * @param dificuldade_procurada Nível de dificuldade usado como filtro.
  */
 void listar_por_dificuldade(Banco_perguntas *banco_perguntas, Dificuldade dificuldade_procurada){
    int indice_atual = -1; //variável que verifica se alguma pergunta foi encontrada ou não
    int opcao;
    int indice_anterior; //variável usada para verificar o vetor nos limites (borda da esquerda ou direita), no seu fim

    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        if(banco_perguntas->perguntas[i].dificuldade == dificuldade_procurada){
            indice_atual = i;
            break; //ao encontrar essa primeira pergunta eu paro a procura
        }
    }//for que percorre todas as perguntas
    if(indice_atual == -1){
        printf("Nenhuma pergunta possui a dificuldade procurada\n");
        return;
    }

    do{
        exibir_pergunta(banco_perguntas, indice_atual);

        printf("\nMENU DE NAVEGAÇÃO\n");
        printf("    1 - Próxima\n");
        printf("    2 - Anterior\n");
        printf("    0 - Voltar ao menu de cadastro\n");
        printf("Digite sua opção: ");
        scanf("%d", &opcao);
        if(opcao != 0 && opcao != 1 && opcao != 2){
            printf("Opção Inválida\n");
        }

        indice_anterior = indice_atual;
        switch (opcao){
        case 1:
            for(int i = (indice_atual+1); i<banco_perguntas->quant_perguntas; i++){ //for procura pergunta do indice atual pra frente
                if(banco_perguntas->perguntas[i].dificuldade == dificuldade_procurada){
                    indice_atual = i;
                    break; //ao encontrar essa primeira pergunta eu paro a procura
                }
            }//for que percorre todas as perguntas
            if(indice_atual == indice_anterior){ // O índice permanece igual quando não há outra pergunta adiante.
                printf("Fim das perguntas cadastradas nessa dificuldade\n");
            }
            break;

        case 2:
            for(int i = (indice_atual-1); i>=0; i--){ //for procura perguntas do indice atual pra baixo
                if(banco_perguntas->perguntas[i].dificuldade == dificuldade_procurada){
                    indice_atual = i;
                    break; //ao encontrar essa primeira pergunta eu paro a procura
                }
            }//for que percorre todas as perguntas
            if(indice_atual == indice_anterior){ // O índice permanece igual quando não há outra pergunta anterior.
                printf("Não há perguntas anteriores à esta nessa dificuldade\n");
            }
            break;

        case 0:
            printf("Fim do menu de navegação\n");
            break;
        
        default:
            break;
        }
    }while(opcao != 0);

 }//LISTAR POR DIFICULDADE

 /**
  * @brief Função que verifica se a resposta está entre A e D e se a dificuldade está entre 1 e 5. A inserção e a alteração aplicam essa validação aos dados recebidos do terminal ou da interface gráfica.
  *
  * @param pergunta Ponteiro para a estrutura que contém a resposta e a dificuldade a verificar.
  * @return int Retorna 0 quando os dois campos forem válidos e -3 quando algum deles for inválido.
  */
 int pergunta_valida(Pergunta *pergunta){
    if((pergunta->resposta == 'A' || pergunta->resposta == 'B' || pergunta->resposta == 'C' || pergunta->resposta == 'D') && (pergunta->dificuldade >=1 && pergunta->dificuldade <=5)){
        return 0;
    }
    return -3; //caso em que as condições não são atendidas
 }
