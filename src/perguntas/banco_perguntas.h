/**
 * @file banco_perguntas.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Estruturação do banco de perguntas, usaremos durante os arquivos para manipular as perguntas, pois já me informa a quantidade de perguntas e a capacidade, desse modo, mantivemos uma organização maior do código
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
        Pergunta *perguntas;
        int quant_perguntas;
        int capacidade_reservada;
        int proximo_id;
    } Banco_perguntas;

    void inicializa_banco(Banco_perguntas *banco_perguntas);

    void inserir_pergunta(Banco_perguntas *banco_perguntas); //insiro novas perguntas

    void listar_perguntas(Banco_perguntas *banco_perguntas); //listo todas as perguntas armazenadas

    int pesquisar_pergunta(Banco_perguntas *banco_pergunta, int id_pergunta_procurada); //pesquiso uma pergunta pelo seu ID e retorno sua posição no vetor

    void exibir_pergunta(Banco_perguntas *banco_perguntas, int indice); //exibo a pergunta a partir do seu ID digitado

    void alterar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_alteracao); //altera a pergunta a partir do seu ID

    void cadastro_pergunta(Banco_perguntas *banco_perguntas, int indice); //pede ao usuário as informações da pergunta

    void excluir_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_excluida); //pede ao usuário o id da pergunta a ser excluída e excloi ela, modificando o vetor

    void listar_por_dificuldade(Banco_perguntas *banco_perguntas, Dificuldade dificuldade_procurada); //pede para o usuário digitar uma dificuldade e lista todas as perguntas que se encontram nessa dificuldade

 #endif /*BANCO_PERGUNTAS_H*/