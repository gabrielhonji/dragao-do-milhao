/**
 * @file arquivos_perguntas.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Implementa a leitura do CSV inicial e o armazenamento do banco de perguntas.
 * @version 0.1
 * @date 2026-11-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <errno.h>
 #include <stdbool.h>
 #ifdef _WIN32
 #include <windows.h>
 #else
 #include <unistd.h>
 #endif
 #include "arquivos_perguntas.h"
 #define LIMITE_PERGUNTAS 150

 /**
  * @brief Carrega o CSV inicial no vetor do banco e calcula o próximo ID.
  * 
  * @param banco_perguntas Banco inicializado para receber as perguntas da base inicial.
  * @param arquivo_csv Endereço do ponteiro usado para abrir e fechar o CSV de origem.
  * @return Quantidade de perguntas no banco ou -1 em caso de falha na abertura ou na leitura.
  */
 int carregar_csv(Banco_perguntas *banco_perguntas, FILE **arquivo_csv){

    char texto[101]; //variável usada para ler o cabeçalho do arquivo csv
    int contador_perguntas = 0;
    char perguntas_lidas[LIMITE_PERGUNTAS][1201]; // Armazena as linhas antes de separar os campos.
    Pergunta *perguntas_temp;
    char *fim; // Recebe a posição em que a conversão numérica terminou.
    char *virgulas[8]; // Guarda os separadores que delimitam os campos.
    char *campos[9]; // Guarda o início de cada campo dentro da linha.
    int maior_id;

    *arquivo_csv = fopen("dados/perguntas_iniciais.csv", "r"); //abro o arquivo localizado na pasta dados com o modo de leitura
    if(*arquivo_csv == NULL){
        return -1; //caso haja erro na abertura retorno -1 para a main
    }

    fgets(texto, 100, *arquivo_csv); //lê a primeira linha do csv - cabeçalho - não contabilizo as linhas aqui

    while(contador_perguntas < LIMITE_PERGUNTAS && fgets(perguntas_lidas[contador_perguntas], sizeof perguntas_lidas[0], *arquivo_csv) != NULL){ // O limite também dimensiona o vetor de linhas.
        contador_perguntas ++; //contabiliza as linhas lidas, sendo válidas ou não
    }

    if (ferror(*arquivo_csv)) {
        fclose(*arquivo_csv);
        *arquivo_csv = NULL;
        return -1;
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
        campos[1] = virgulas[0]+1; // Cada campo começa no primeiro caractere após a vírgula anterior.
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
        campos[7][strcspn(campos[7], "\n")] = '\0'; // O último campo termina na quebra de linha ou no fim da string.
        banco_perguntas->perguntas[banco_perguntas->quant_perguntas].dificuldade = strtol(campos[7], &fim, 10); //transformando o texto de dificuldade para número
        
        banco_perguntas->quant_perguntas++;
    }//for que percorre todas as perguntas do csv

    // O primeiro ID novo deve ficar acima de todos os IDs importados.
    if(banco_perguntas->quant_perguntas != 0){ // A quantidade importada pode ser menor que o total de linhas lidas.
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

    return banco_perguntas->quant_perguntas;

 }//CARREGAR CSV



 /**
  * @brief Grava o banco em um arquivo temporário e o publica após concluir a gravação.
  * 
  * @param banco_perguntas Banco com as perguntas, o próximo ID e a fila de IDs disponíveis.
  * @param arquivo_dat Endereço do ponteiro usado para escrever e fechar o arquivo temporário.
  * @param substituir Verdadeiro para atualizar um banco salvo, falso para criar o banco inicial.
  * @return 0 em sucesso, 1 se já existe um banco que deve ser preservado ou -1 em caso de erro.
  */
 static int gravar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_dat, bool substituir)
 {
    const char *destino = "dados/perguntas.dat";
    char temporario[72];

    // Grava no mesmo diretório antes de substituir o banco, para preservar o arquivo se a gravação falhar.
#ifdef _WIN32
    unsigned long processo = GetCurrentProcessId();
#else
    unsigned long processo = (unsigned long)getpid();
#endif
    snprintf(temporario, sizeof temporario, "dados/perguntas-%lu.tmp", processo);
    *arquivo_dat = fopen(temporario, "wb");
    if (*arquivo_dat == NULL)
        return -1;

    size_t total_ids = (size_t)banco_perguntas->quant_ids_disponiveis;
    size_t total_perguntas = (size_t)banco_perguntas->quant_perguntas;
    int gravou = fwrite(&banco_perguntas->quant_perguntas, sizeof(int), 1, *arquivo_dat) == 1 &&
                 fwrite(&banco_perguntas->proximo_id, sizeof(int), 1, *arquivo_dat) == 1 &&
                 fwrite(&banco_perguntas->quant_ids_disponiveis, sizeof(int), 1, *arquivo_dat) == 1 &&
                 (total_ids == 0 || fwrite(banco_perguntas->ids_disponiveis, sizeof(int), total_ids, *arquivo_dat) == total_ids) &&
                 (total_perguntas == 0 || fwrite(banco_perguntas->perguntas, sizeof(Pergunta), total_perguntas, *arquivo_dat) == total_perguntas);
    int fechou = fclose(*arquivo_dat);
    *arquivo_dat = NULL;

    int resultado = -1;
    if (gravou && fechou == 0) {
#ifdef _WIN32
        DWORD opcoes = substituir ? MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH : MOVEFILE_WRITE_THROUGH;
        if (MoveFileExA(temporario, destino, opcoes))
            return 0;
        DWORD erro = GetLastError();
        if (!substituir && (erro == ERROR_ALREADY_EXISTS || erro == ERROR_FILE_EXISTS))
            resultado = 1;
#else
        if (substituir && rename(temporario, destino) == 0)
            return 0;
        // O link publica o banco inicial somente se o destino ainda estiver livre.
        if (!substituir) {
            if (link(temporario, destino) == 0)
                resultado = 0;
            else if (errno == EEXIST)
                resultado = 1;
        }
#endif
    }

    remove(temporario);
    return resultado;
 }


 /**
  * @brief Atualiza o binário com o estado atual do banco.
  * @param banco_perguntas Banco que será gravado.
  * @param arquivo_dat Endereço do ponteiro usado para gravar o arquivo.
  * @return 0 em sucesso ou -1 em caso de falha.
  */
 int salvar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_dat)
 {
    return gravar_binario(banco_perguntas, arquivo_dat, true);
 }


 int salvar_binario_inicial(Banco_perguntas *banco_perguntas, FILE **arquivo_dat)
 {
    return gravar_binario(banco_perguntas, arquivo_dat, false);
 }



 /**
  * @brief Lê o binário em um banco temporário e substitui o banco atual após concluir a leitura.
  * O arquivo guarda a quantidade de perguntas, o próximo ID, a quantidade de IDs disponíveis,
  * o vetor de IDs disponíveis e o vetor de perguntas, nessa ordem.
  * 
  * @param banco_perguntas Banco inicializado que recebe os dados do arquivo.
  * @param arquivo_dat Endereço do ponteiro usado para abrir e fechar o binário.
  * @return 0 em sucesso, -2 se o arquivo estiver ausente ou -1 em caso de outra falha.
  */
 int carregar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_dat){

    Banco_perguntas banco_perguntas_temp;
    inicializa_banco(&banco_perguntas_temp); //inicializo o banco de perguntas temporário
    int retorno_fread;

    *arquivo_dat = fopen("dados/perguntas.dat", "rb");
    if (*arquivo_dat == NULL)
        return errno == ENOENT ? -2 : -1;

    //LENDO A QUANTIDADE DE PERGUNTAS
    retorno_fread = fread(&banco_perguntas_temp.quant_perguntas, sizeof(int), 1, *arquivo_dat);
    if(retorno_fread != 1){ //conferindo se as informações foram lidas do arquivo binário
        fclose(*arquivo_dat); //fecho antes de retornar o erro
        *arquivo_dat = NULL;
        return -1;
    }
    if(banco_perguntas_temp.quant_perguntas > 0){
        banco_perguntas_temp.perguntas = malloc(sizeof(Pergunta)*banco_perguntas_temp.quant_perguntas); //aloco um espaço na memória para o vetor banco_perguntas_temp igual a quant de perguntas lidas do arquivo binário
        if(banco_perguntas_temp.perguntas == NULL){
            fclose(*arquivo_dat); //fecho antes de retornar o erro
            *arquivo_dat = NULL;
            printf("Erro na alocação de memória\n");
            return -1;
        }//se eu recebi que a quantidade de perguntas maior que 0, reservo um espaço para receber as perguntas
        banco_perguntas_temp.capacidade_reservada = banco_perguntas_temp.quant_perguntas; //igualo a capacidade de perguntas à quantidade de perguntas para evitar erro no banco temporário
    }else if(banco_perguntas_temp.quant_perguntas < 0){
        fclose(*arquivo_dat); //fecho antes de retornar o erro
        *arquivo_dat = NULL;
        return -1;
        //caso em que a quantidade de perguntas lidas é negativa
    }else{ //caso em que a quantidade de perguntas lidas é 0
        banco_perguntas_temp.capacidade_reservada = 0;
    }

    //LENDO QUAL O VALOR DO PRÓXIMO ID
    retorno_fread = fread(&banco_perguntas_temp.proximo_id, sizeof(int), 1, *arquivo_dat); //o segundo dado salvo no arquivo binário é o próximo id que será utilizado para cadastrar perguntas futuras
    if(retorno_fread != 1){ //conferindo se as informações foram lidas do arquivo binário
        fclose(*arquivo_dat); //fecho antes de retornar o erro
        *arquivo_dat = NULL;
        free(banco_perguntas_temp.perguntas);
        return -1;
    }

    //LENDO A QUANTIDADE DE IDS ARMAZENADOS
    retorno_fread = fread(&banco_perguntas_temp.quant_ids_disponiveis, sizeof(int), 1, *arquivo_dat);
    if(retorno_fread != 1){ //conferindo se as informações foram lidas do arquivo binário
        free(banco_perguntas_temp.perguntas);
        fclose(*arquivo_dat); //fecho antes de retornar o erro
        *arquivo_dat = NULL;
        return -1;
    }
    if(banco_perguntas_temp.quant_ids_disponiveis > 0){
        banco_perguntas_temp.ids_disponiveis = malloc(sizeof(int)*banco_perguntas_temp.quant_ids_disponiveis); //aloco um espaço na memória para o vetor banco_perguntas_temp igual a quant de ids lidos do arquivo binário
        if(banco_perguntas_temp.ids_disponiveis == NULL){
            free(banco_perguntas_temp.perguntas);
            fclose(*arquivo_dat); //fecho antes de retornar o erro
            *arquivo_dat = NULL;
            printf("Erro na alocação de memória\n");
            return -1;
        }//se eu recebi que a quantidade de ids_dispniveis maior que 0, reservo um espaço para receber os ids
    }else if(banco_perguntas_temp.quant_ids_disponiveis < 0){
        free(banco_perguntas_temp.perguntas);
        fclose(*arquivo_dat); //fecho antes de retornar o erro
        *arquivo_dat = NULL;
        return -1;
        //caso em que a quantidade de ids lidas é negativa
    }

    if(banco_perguntas_temp.quant_ids_disponiveis > 0){ // Lê a fila de IDs quando ela contém registros.
        retorno_fread = fread(banco_perguntas_temp.ids_disponiveis, sizeof(int), banco_perguntas_temp.quant_ids_disponiveis, *arquivo_dat); //por fim eu leio os IDs no arquivo binário
        if(retorno_fread != banco_perguntas_temp.quant_ids_disponiveis){ //conferindo se as informações foram lidas do arquivo binário
            fclose(*arquivo_dat); //fecho antes de retornar o erro
            *arquivo_dat = NULL;
            free(banco_perguntas_temp.ids_disponiveis);
            free(banco_perguntas_temp.perguntas);
            return -1;
        }
    }

    if(banco_perguntas_temp.quant_perguntas > 0){ // Lê o vetor de perguntas quando a coleção contém registros.
        retorno_fread = fread(banco_perguntas_temp.perguntas, sizeof(Pergunta), banco_perguntas_temp.quant_perguntas, *arquivo_dat); //por fim eu leio as perguntas no arquivo binário
        if(retorno_fread != banco_perguntas_temp.quant_perguntas){ //conferindo se as informações foram lidas do arquivo binário
            fclose(*arquivo_dat); //fecho antes de retornar o erro
            *arquivo_dat = NULL;
            free(banco_perguntas_temp.perguntas);
            free(banco_perguntas_temp.ids_disponiveis); //libero o vetor caso dê erro
            return -1;
        }
    }

    // Após a leitura completa, transfere a propriedade dos vetores para o banco definitivo.
    free(banco_perguntas->perguntas); //libero o endereço do vetor antigo antes de atribuir o novo
    free(banco_perguntas->ids_disponiveis); //libero o vetor
    *banco_perguntas = banco_perguntas_temp;

    fclose(*arquivo_dat);
    *arquivo_dat = NULL;

    return 0;
 }//CARREGAR BINÁRIO



 /**
  * @brief Escreve o banco atual em dados/perguntas_exportadas.csv, substituindo uma exportação anterior.
  * 
  * @param banco_perguntas Banco cujas perguntas serão consultadas.
  * @param arquivo_exportacao_csv Endereço do ponteiro usado para abrir e fechar o arquivo de saída.
  * @return -1 se a abertura ou um fprintf falhar. Retorna 0 após escrever as linhas e fechar o arquivo.
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
