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
        Pergunta *perguntas; //Vetor dinâmico das perguntas cadastradas.
        int quant_perguntas; //Quantidade de perguntas cadastradas.
        int capacidade_reservada; //Quantidade de posições alocadas no vetor de perguntas.
        int proximo_id; //ID usado no próximo cadastro quando a fila de IDs disponíveis estiver vazia.
        int *ids_disponiveis; //Fila de IDs excluídos, na ordem em que serão reaproveitados.
        int quant_ids_disponiveis; //Quantidade de IDs disponíveis para reaproveitamento.
    } Banco_perguntas; //estrutura criada para a organização do código

    void inicializa_banco(Banco_perguntas *banco_perguntas); //inicializacão da estrutura zerando ela

    int inserir_pergunta(Banco_perguntas *banco_perguntas, Pergunta *pergunta); //Cadastra uma cópia da pergunta e atribui o ID.

    void listar_perguntas(Banco_perguntas *banco_perguntas); //listo todas as perguntas armazenadas

    int pesquisar_pergunta(Banco_perguntas *banco_pergunta, int id_pergunta_procurada); //pesquiso uma pergunta pelo seu ID e retorno sua posição no vetor

    void exibir_pergunta(Banco_perguntas *banco_perguntas, int indice); //exibo a pergunta a partir do indice recebido

    int alterar_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_alteracao, Pergunta *pergunta_alterada); //Atualiza os dados da pergunta e mantém seu ID.

    int excluir_pergunta(Banco_perguntas *banco_perguntas, int id_pergunta_excluida); //Exclui pelo ID e o reserva para reaproveitamento.

    void listar_por_dificuldade(Banco_perguntas *banco_perguntas, Dificuldade dificuldade_procurada); //recebe uma dificuldade por parâmetro e permite ao usuário navegar entre as perguntas que possuem essa dificuldade

    int pergunta_valida(Pergunta *pergunta); //Verifica a resposta e a dificuldade da pergunta recebida.

 #endif /*BANCO_PERGUNTAS_H*/
