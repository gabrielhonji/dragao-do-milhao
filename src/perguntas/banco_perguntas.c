/**
 * @file banco_perguntas.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Implementa o cadastro, a consulta, a altera├º├úo e a exclus├úo de perguntas.
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
  * @brief Fun├º├úo que inicializa o banco de perguntas como zerado
  *
  * @param banco_perguntas struct banco_perguntas sendo passado por par├ómetro
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
  * @brief Fun├º├úo que valida e cadastra uma c├│pia da pergunta recebida no vetor do banco. Amplia a capacidade se necess├írio e atribui o primeiro ID dispon├¡vel ou, se a fila estiver vazia, o pr├│ximo ID novo. Ao concluir, aumenta a quantidade de perguntas.
  *
  * @param banco_perguntas Ponteiro para o banco inicializado que receber├í a nova pergunta.
  * @param pergunta Ponteiro para a pergunta preenchida pelo chamador. O banco atribui o ID da c├│pia.
  * @return int Retorna 0 em caso de sucesso, -1 se a realoca├º├úo falhar e -3 se a pergunta for inv├ílida.
  */
 int inserir_pergunta(Banco_perguntas *banco_perguntas, Pergunta *pergunta){

    Pergunta *perguntas_temp; //Recebe o resultado da realoca├º├úo do vetor.
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
            banco_perguntas->capacidade_reservada += 1; //Atualiza a capacidade ap├│s a realoca├º├úo.
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
  * @brief Fun├º├úo que percorre de 0 at├⌐ a quantidade de perguntas e lista uma por uma
  *
  * @param banco_perguntas struct banco_perguntas sendo passado por par├ómetro
  */
 void listar_perguntas(Banco_perguntas *banco_perguntas){

    if(banco_perguntas->quant_perguntas == 0){
         printf("Nenhuma pergunta cadastrada\n");
     }

    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        exibir_pergunta(banco_perguntas, i); //uso uma fun├º├úo cadastrada respons├ível por exibir as perguntas um a um
        printf("\n"); //est├⌐tica
    }//for de listagem
 }//LISTAR PERGUNTAS


 /**
  * @brief Fun├º├úo que recebe o ID da pergunta procurada e procura ela atrav├⌐s de um for dentro das perguntas cadastradas
  *
  * @param banco_perguntas struct banco_perguntas sendo passado por par├ómetro
  * @param id_pergunta_procurada recebe o ID da pergunta que est├í procurando
  * @return int indice encontrado ou -1
  */
 int pesquisar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_procurada){

    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        if(id_pergunta_procurada == banco_perguntas->perguntas[i].identificador){
            return i;
        }
    }//for

    return -1; //caso em que j├í percorreu todas as perguntas e n├úo encontrou aquela com o id digitado
 }//PESQUISAR PERGUNTA


 /**
  * @brief Fun├º├úo que recebe um indice e exibe a pergunta cadastrada naquele indice
  *
  * @param banco_perguntas struct banco_perguntas sendo passado por par├ómetro
  * @param indice recebe o indice da pergunta que vai exibir - posi├º├úo v├ílida da pergunta no vetor
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
            printf("Muito F├ícil\n");
            break;

        case facil:
            printf("F├ícil\n");
            break;

        case medio:
            printf("M├⌐dio\n");
            break;

        case dificil:
            printf("Dif├¡cil\n");
            break;

        case muito_dificil:
            printf("Muito Dif├¡cil\n");
            break;
        default:
            break;
        }//switch da dificuldade
 }//EXIBIR PERGUNTA



 /**
  * @brief Fun├º├úo que localiza uma pergunta pelo ID e valida os novos dados antes de copi├í-los. A pergunta ├⌐ atualizada na posi├º├úo encontrada e mant├⌐m o identificador original.
  *
  * @param banco_perguntas Ponteiro para o banco que cont├⌐m a pergunta cadastrada.
  * @param id_pergunta_alteracao ID da pergunta que ser├í alterada.
  * @param pergunta_alterada Ponteiro para a estrutura preenchida com os novos dados.
  * @return int Retorna 0 em caso de sucesso, -1 se o ID n├úo for encontrado e -3 se os dados forem inv├ílidos.
  */
 int alterar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_alteracao, Pergunta *pergunta_alterada){
    int retorno_pesquisa; //├ìndice da pergunta localizada pelo ID.
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
  * @brief Fun├º├úo que exclui uma pergunta pelo ID e reserva seu identificador para reaproveitamento. Depois de adicionar o ID ao fim da fila, desloca as perguntas seguintes no vetor e reduz a quantidade cadastrada.
  *
  * @param banco_perguntas Ponteiro para o banco que cont├⌐m a pergunta cadastrada e a fila de IDs dispon├¡veis.
  * @param id_pergunta_excluida ID da pergunta que ser├í exclu├¡da.
  * @return int Retorna 0 em caso de sucesso, -1 se o ID n├úo for encontrado e -2 se a realoca├º├úo da fila falhar.
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


    //Preenche a posi├º├úo da pergunta exclu├¡da deslocando as seguintes para a esquerda.
    for(int i = retorno_pesquisa; i<banco_perguntas->quant_perguntas-1; i++){
        banco_perguntas->perguntas[i] = banco_perguntas->perguntas[i+1];
    }

    banco_perguntas->quant_perguntas --;

    return 0;
 }//EXCLUIR PERGUNTAS



 /**
  * @brief Fun├º├úo que recebe a dificuldade procurada e exibe um menu que permite a navega├º├úo entre as perguntas dessa dificuldade
  *
  * @param banco_perguntas struct banco_perguntas sendo passado por par├ómetro
  * @param dificuldade_procurada recebe a dificuldade das perguntas que est├í sendo levada em conta
  */
 void listar_por_dificuldade(Banco_perguntas *banco_perguntas, Dificuldade dificuldade_procurada){
    int indice_atual = -1; //vari├ível que verifica se alguma pergunta foi encontrada ou n├úo
    int opcao;
    int indice_anterior; //vari├ível usada para verificar o vetor nos limites (borda da esquerda ou direita), no seu fim

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
        exibir_pergunta(banco_perguntas, indice_atual); //primeiro eu exibo a pergunta atual e dps mostro o menu de navega├º├úo

        printf("\nMENU DE NAVEGA├ç├âO\n");
        printf("    1 - Pr├│xima\n");
        printf("    2 - Anterior\n");
        printf("    0 - Voltar ao menu de cadastro\n");
        printf("Digite sua op├º├úo: ");
        scanf("%d", &opcao);
        if(opcao != 0 && opcao != 1 && opcao != 2){
            printf("Op├º├úo Inv├ílida\n");
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
            if(indice_atual == indice_anterior){ //fim das perguntas ├á direita do vetor, o meu indice vai ser o mesmo, logo vai ter acabado as perguntas
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
            if(indice_atual == indice_anterior){ //fim das perguntas ├á esquerda no vetor, o meu indice vai ser o mesmo, logo vai ter acabado as perguntas
                printf("N├úo h├í perguntas anteriores ├á esta nessa dificuldade\n");
            }
            break;

        case 0:
            printf("Fim do menu de navega├º├úo\n");
            break;

        default:
            break;
        }
    }while(opcao != 0);

 }//LISTAR POR DIFICULDADE

 /**
  * @brief Fun├º├úo que verifica se a resposta est├í entre A e D e se a dificuldade est├í entre 1 e 5. A inser├º├úo e a altera├º├úo aplicam essa valida├º├úo aos dados recebidos do terminal ou da interface gr├ífica.
  *
  * @param pergunta Ponteiro para a estrutura que cont├⌐m a resposta e a dificuldade a verificar.
  * @return int Retorna 0 quando os dois campos forem v├ílidos e -3 quando algum deles for inv├ílido.
  */
 int pergunta_valida(Pergunta *pergunta){
    if((pergunta->resposta == 'A' || pergunta->resposta == 'B' || pergunta->resposta == 'C' || pergunta->resposta == 'D') && (pergunta->dificuldade >=1 && pergunta->dificuldade <=5)){
        return 0;
    }
    return -3; //caso em que as condi├º├╡es n├úo s├úo atendidas
 }
