/**
 * @file arquivos_perguntas.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Estruturação dos arquivos de pergunta
 * @version 0.1
 * @date 2026-11-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "arquivos_perguntas.h"

 /**
  * @brief Função que carrega o arquivo CSV das perguntas, caso não consiga carregar, retorna -1. Além de carregar armazena as perguntas do arquivo no nosso vetor de perguntas
  * 
  * @param banco_perguntas struct banco_perguntas sendo passado por parâmetro
  * @param arquivo_csv recebe o arquivo CSV de onde serão lido os dados
  * @return int retorna -1 em erro, retorna a quantidade de perguntas lidas se bem sucedido
  */
 int carregar_csv(Banco_perguntas *banco_perguntas, FILE **arquivo_csv){

    char texto[101]; //variável usada para ler o cabeçalho do arquivo csv
    int contador_perguntas = 0;
    char perguntas_lidas[100][1201]; //as perguntas vão ser armazenadas nesse vetor de strings, somente depois vou transformar strings em dados das perguntas de fato
    Pergunta *perguntas_temp;
    char *fim; //variável usada para armazenar o fim de cada leitura seccionada das perguntas
    char *virgulas[8]; //vetor usado para armazenar a posição de cada vírgula e começar a leitura do próximo campo a partir desta posição
    char *campos[9]; //vetor que armazena os campos de cada pergunta, sendo os campos: ID, enunciado, Alternativa A...
    int maior_id;

    *arquivo_csv = fopen("dados/perguntas_iniciais.csv", "r"); //abro o arquivo localizado na pasta dados com o modo de leitura
    if(*arquivo_csv == NULL){
        return -1; //caso haja erro na abertura retorno -1 para a main
    }

    fgets(texto, 100, *arquivo_csv); //lê a primeira linha do csv - cabeçalho - não contabilizo as linhas aqui

    while(contador_perguntas < 100 && fgets(perguntas_lidas[contador_perguntas], 1200, *arquivo_csv) != NULL){ //o fgets vai rodar até que seja possível ler linhas, caso não seja possível ele retorna NULL por padrão. A limitação em 100 é para a quantidade de perguntas, visto que vamos trabalhar com só até 100 perguntas
        contador_perguntas ++; //contabiliza as linhas lidas, sendo válidas ou não
    }
    
    for(int i = 0; i<contador_perguntas; i++){
        
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

        campos[0] = perguntas_lidas[i]; //o meu primeiro campo 
        //ID DA PERGUNTA
        virgulas[0] = strchr(campos[0], ','); //procura a posição da primeira vírgula - essa função me retorna o endereço dentro da string em memória
        if(virgulas[0] == NULL){ //casos em que não acha uma vírgula
            continue; 
        }
        *virgulas[0] = '\0'; //quando acho a vírgula, finalizo a string com \0
        banco_perguntas->perguntas[banco_perguntas->quant_perguntas].identificador = strtol(campos[0], &fim, 10); //transformando o texto de ID para número

        //ENUNCIADO
        campos[1] = virgulas[0]+1; //o enunciado (campos[1]) começa uma casa à frente da posição da última vírgula, isso se repete
        virgulas[1] = strchr(campos[1], ',');
        if(virgulas[1] == NULL){ //casos em que não acha uma vírgula
            continue; 
        }
        *virgulas[1] = '\0';
        strcpy(banco_perguntas->perguntas[banco_perguntas->quant_perguntas].enunciado, campos[1]); //uso strcpy para copiar a string achada para a posição dela no meu vetor de perguntas

        //ALTERNATIVA A
        campos[2] = virgulas[1]+1;
        virgulas[2] = strchr(campos[2], ',');
        if(virgulas[2] == NULL){
            continue;
        }
        *virgulas[2] = '\0';
        strcpy(banco_perguntas->perguntas[banco_perguntas->quant_perguntas].alternativa.A, campos[2]);

        //ALTERNATIVA B
        campos[3] = virgulas[2]+1;
        virgulas[3] = strchr(campos[3], ',');
        if(virgulas[3] == NULL){
            continue;
        }
        *virgulas[3] = '\0';
        strcpy(banco_perguntas->perguntas[banco_perguntas->quant_perguntas].alternativa.B, campos[3]);

        //ALTERNATIVA C
        campos[4] = virgulas[3]+1;
        virgulas[4] = strchr(campos[4], ',');
        if(virgulas[4] == NULL){
            continue;
        }
        *virgulas[4] = '\0';
        strcpy(banco_perguntas->perguntas[banco_perguntas->quant_perguntas].alternativa.C, campos[4]);

        //ALTERNATIVA D
        campos[5] = virgulas[4]+1;
        virgulas[5] = strchr(campos[5], ',');
        if(virgulas[5] == NULL){
            continue;
        }
        *virgulas[5] = '\0';
        strcpy(banco_perguntas->perguntas[banco_perguntas->quant_perguntas].alternativa.D, campos[5]);

        //RESPOSTA
        campos[6] = virgulas[5]+1;
        virgulas[6] = strchr(campos[6], ',');
        if(virgulas[6] == NULL){
            continue;
        }
        *virgulas[6] = '\0';
        banco_perguntas->perguntas[banco_perguntas->quant_perguntas].resposta = *campos[6];

        //DIFICULDADE
        campos[7] = virgulas[6]+1;
        campos[7][strcspn(campos[7], "\n")] = '\0'; //como é o fim da pergunta, preciso ver onde está o \n e não mais a linha
        banco_perguntas->perguntas[banco_perguntas->quant_perguntas].dificuldade = strtol(campos[7], &fim, 10); //transformando o texto de dificuldade para número
        
        banco_perguntas->quant_perguntas++;
    }//for que percorre todas as perguntas do csv

    //for que percorre as perguntas cadastradas do arquivo para identificar qual foi o maior ID - faço isso para que quando for cadastrar, cadastre com um ID diferente dos já existentes
    if(banco_perguntas->quant_perguntas != 0){ //uso banco_perguntas->quant_perguntas pq já sei quantas válidas foram cadastradas e meu contador contador_perguntas pode contar linhas inválidas
        maior_id = banco_perguntas->perguntas[0].identificador;
        for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
            if(banco_perguntas->perguntas[i].identificador > maior_id){
                maior_id = banco_perguntas->perguntas[i].identificador;
            }
        }
        banco_perguntas->proximo_id = maior_id+1; //o ID usado para cadastrar a primeira pergunta vai ser o maior lido aqui mais um
    }

    fclose(*arquivo_csv);
    *arquivo_csv = NULL;

    return contador_perguntas;

 }//CARREGAR CSV



 /**
  * @brief Função que abre o arquivo binário, salva as perguntas cadastradas e retorna -1 em caso de erro. Se houver sucesso retorna 0;
  * 
  * @param banco_perguntas struct banco_perguntas sendo passado por parâmetro
  * @param arquivo_bin recebe o arquivo binário onde serão salvas as informações
  * @return int retorna -1 em erro e se houver sucesso retorna 0
  */
 int salvar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_bin){

    int retorno_fwrite;

    *arquivo_bin = fopen("dados/perguntas.bin", "wb");
    if(*arquivo_bin == NULL){ //caso dê erro na abertura do arquivo eu retorno -1 para a main
        return -1;
    }

    retorno_fwrite = fwrite(&banco_perguntas->quant_perguntas, sizeof(int), 1, *arquivo_bin); //o primeiro dado salvo no arquivo binário é a quantidade de perguntas
    if(retorno_fwrite != 1){ //conferindo se as informações foram gravadas no arquivo binário
        fclose(*arquivo_bin); //fecho antes de retornar o erro
        *arquivo_bin = NULL;
        return -1;
    }

    retorno_fwrite = fwrite(&banco_perguntas->proximo_id, sizeof(int), 1, *arquivo_bin); //o segundo dado salvo no arquivo binário é o próximo id que será utilizado para cadastrar perguntas futuras
    if(retorno_fwrite != 1){ //conferindo se as informações foram gravadas no arquivo binário
        fclose(*arquivo_bin); //fecho antes de retornar o erro
        *arquivo_bin = NULL;
        return -1;
    }

    retorno_fwrite = fwrite(banco_perguntas->perguntas, sizeof(Pergunta), banco_perguntas->quant_perguntas, *arquivo_bin); //por fim eu salvo as perguntas no arquivo binário
    if(retorno_fwrite != banco_perguntas->quant_perguntas){ //conferindo se as informações foram gravadas no arquivo binário
        fclose(*arquivo_bin); //fecho antes de retornar o erro
        *arquivo_bin = NULL;
        return -1;
    }

    fclose(*arquivo_bin);
    *arquivo_bin = NULL;

    return 0;
 }//SALVAR BINARIO



 /**
  * @brief Função que carrega o arquivo binário e salva as informações presentes nele nos campos destinados. A primeira informação é a quant de perguntas, a segunda é o próximo ID e por fim tenho os dados das perguntas em si. Caso de erro retorna -1 para a main, em caso de sucesso retorna 0. Se o arquivo não existir, retorna -2.
  * 
  * @param banco_perguntas struct banco_perguntas sendo passado por parâmetro
  * @param arquivo_bin recebe o arquivo binário de onde serão lidas as informações
  * @return int retorna -2 se o arquivo não existir, -1 se der erro e 0 se ocorrer tudo certo
  */
 int carregar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_bin){

    Banco_perguntas banco_perguntas_temp;
    inicializa_banco(&banco_perguntas_temp); //inicializo o banco de perguntas temporário
    int retorno_fread;

    *arquivo_bin = fopen("dados/perguntas.bin", "rb");
    if(*arquivo_bin == NULL){ //caso dê erro na abertura do arquivo eu retorno -2 para a main
        return -2; //esse -2 vai ser lido na main
    }

    retorno_fread = fread(&banco_perguntas_temp.quant_perguntas, sizeof(int), 1, *arquivo_bin);
    if(retorno_fread != 1){ //conferindo se as informações foram lidas do arquivo binário
        fclose(*arquivo_bin); //fecho antes de retornar o erro
        *arquivo_bin = NULL;
        return -1;
    }
    if(banco_perguntas_temp.quant_perguntas > 0){
        banco_perguntas_temp.perguntas = malloc(sizeof(Pergunta)*banco_perguntas_temp.quant_perguntas); //aloco um espaço na memória para o vetor banco_perguntas_temp igual a quant de perguntas lidas do arquivo binário
        if(banco_perguntas_temp.perguntas == NULL){
            fclose(*arquivo_bin); //fecho antes de retornar o erro
            *arquivo_bin = NULL;
            printf("Erro na alocação de memória\n");
            return -1;
        }//se eu recebi que a quantidade de perguntas maior que 0, reservo um espaço para receber as perguntas
        banco_perguntas_temp.capacidade_reservada = banco_perguntas_temp.quant_perguntas; //igualo a capacidade de perguntas à quantidade de perguntas para evitar erro no banco temporário
    }else if(banco_perguntas_temp.quant_perguntas < 0){
        fclose(*arquivo_bin); //fecho antes de retornar o erro
        *arquivo_bin = NULL;
        return -1;
        //caso em que a quantidade de perguntas lidas é negativa
    }else{ //caso em que a quantidade de perguntas lidas é 0
        banco_perguntas_temp.capacidade_reservada = 0;
    }

    retorno_fread = fread(&banco_perguntas_temp.proximo_id, sizeof(int), 1, *arquivo_bin); //o segundo dado salvo no arquivo binário é o próximo id que será utilizado para cadastrar perguntas futuras
    if(retorno_fread != 1){ //conferindo se as informações foram lidas do arquivo binário
        fclose(*arquivo_bin); //fecho antes de retornar o erro
        *arquivo_bin = NULL;
        free(banco_perguntas_temp.perguntas); //libero oq foi alocado em caso de erro
        return -1;
    }

    if(banco_perguntas_temp.quant_perguntas > 0){//só leio as perguntas caso eu tiver uma quantidade de perguntas maior do que 0
        retorno_fread = fread(banco_perguntas_temp.perguntas, sizeof(Pergunta), banco_perguntas_temp.quant_perguntas, *arquivo_bin); //por fim eu leio as perguntas no arquivo binário
        if(retorno_fread != banco_perguntas_temp.quant_perguntas){ //conferindo se as informações foram lidas do arquivo binário
            fclose(*arquivo_bin); //fecho antes de retornar o erro
            *arquivo_bin = NULL;
            free(banco_perguntas_temp.perguntas); //libero oq foi alocado em caso de erro
            return -1;
        }
    }

    //primeiro salvo as informações em um banco de perguntas temporário, caso dê tudo certo passo as informações desse para o original
    free(banco_perguntas->perguntas); //libero o endereço do vetor antigo antes de atribuir o novo
    *banco_perguntas = banco_perguntas_temp;

    fclose(*arquivo_bin);
    *arquivo_bin = NULL;

    return 0;
 }//CARREGAR BINÁRIO



 /**
  * @brief Função que exporta as perguntas para um arquivo CSV separado. Caso dê erro retorna -1 para a main, em caso de sucesso retorna 0
  * 
  * @param banco_perguntas struct banco_perguntas sendo passado por parâmetro
  * @param arquivo_exportacao_csv recebe o arquivo CSV onde será armazenado as informações sobre as perguntas
  * @return int retorna -1 em caso de erro e retorna 0 em caso de sucesso
  */
 int exportar_csv(Banco_perguntas *banco_perguntas, FILE **arquivo_exportacao_csv){

    int retorno_fprintf;

    *arquivo_exportacao_csv = fopen("dados/perguntas_exportadas.csv", "w");
    if(*arquivo_exportacao_csv == NULL){
        return -1; //retorna -1 caso o arquivo não consiga ser criado
    }

    retorno_fprintf = fprintf(*arquivo_exportacao_csv, "id,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,resposta,dificuldade\n");//imprimindo o cabeçalho
    if(retorno_fprintf < 0){ //caso dê erro o fprintf retorna um valor negativo
        fclose(*arquivo_exportacao_csv);
        *arquivo_exportacao_csv = NULL;
        return -1;
    }

    for(int i = 0; i<banco_perguntas->quant_perguntas; i++){
        retorno_fprintf = fprintf(*arquivo_exportacao_csv, "%d,%s,%s,%s,%s,%s,%c,%d\n", banco_perguntas->perguntas[i].identificador, banco_perguntas->perguntas[i].enunciado, banco_perguntas->perguntas[i].alternativa.A, banco_perguntas->perguntas[i].alternativa.B, banco_perguntas->perguntas[i].alternativa.C, banco_perguntas->perguntas[i].alternativa.D, banco_perguntas->perguntas[i].resposta, banco_perguntas->perguntas[i].dificuldade);
        if(retorno_fprintf < 0){ //caso dê erro o fprintf retorna um valor negativo
            fclose(*arquivo_exportacao_csv);
            *arquivo_exportacao_csv = NULL;
            return -1;
        }
    }//for que percorre todas as perguntas cadastradas e vai imprimindo uma a uma no CSV

    fclose(*arquivo_exportacao_csv);
    *arquivo_exportacao_csv = NULL;

    return 0;
 }//EXPORTAR CSV