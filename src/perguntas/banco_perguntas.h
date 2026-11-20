/**
 * @file banco_perguntas.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Define o banco de perguntas e declara as operações sobre a coleção.
 * @version 0.1
 * @date 2026-11-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #ifndef BANCO_PERGUNTAS_H
    #define BANCO_PERGUNTAS_H

    #include "pergunta.h"

    typedef struct{
        Pergunta *perguntas; //chamo a struct Pergunta já declarada anteriormente em pergunta.h, perguntas é um ponteiro para o vetor dinâmico de perguntas
        int quant_perguntas; //inteiro que armazena a quantidade de perguntas atuais
        int capacidade_reservada; //inteiro que armazena quantos espaços estão alocados para o vetor
        int proximo_id; //identificador reservado para o próximo cadastro
        int *ids_disponiveis; //essa variável a de baixo são usadas para lidar com a questão de IDs fantasmas ao excluir perguntas
        int quant_ids_disponiveis;
    } Banco_perguntas; //estrutura criada para a organização do código

    void inicializa_banco(Banco_perguntas *banco_perguntas); //inicializacão da estrutura zerando ela

    void inserir_pergunta(Banco_perguntas *banco_perguntas); //insiro novas perguntas

    void listar_perguntas(Banco_perguntas *banco_perguntas); //listo todas as perguntas armazenadas

    int pesquisar_pergunta(Banco_perguntas *banco_pergunta, int id_pergunta_procurada); //pesquiso uma pergunta pelo seu ID e retorno sua posição no vetor

    void exibir_pergunta(Banco_perguntas *banco_perguntas, int indice); //exibo a pergunta a partir do indice recebido

    void alterar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_alteracao); //altera a pergunta a partir do seu ID recebido 

    void cadastro_pergunta(Banco_perguntas *banco_perguntas, int indice); //pede ao usuário as informações da pergunta

    void excluir_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_excluida); //exclui a pergunta a partir do seu ID recebido

    void listar_por_dificuldade(Banco_perguntas *banco_perguntas, Dificuldade dificuldade_procurada); //recebe uma dificuldade por parâmetro e permite ao usuário navegar entre as perguntas que possuem essa dificuldade

 #endif /*BANCO_PERGUNTAS_H*/