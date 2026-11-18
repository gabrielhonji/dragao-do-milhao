/**
 * @file pergunta.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Estruturação das perguntas
 * @version 0.1
 * @date 2026-11-10
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #ifndef PERGUNTA_H
    #define PERGUNTA_H

 typedef enum {muito_facil=1, facil, medio, dificil, muito_dificil} Dificuldade;
 //os valores das dificuldades variam de 1 a 5, começando em 1 por muito_facil e acabando em 5 com muito_dificil

 typedef struct{
    char A[101]; //texto da alternativa A
    char B[101]; //texto da alternativa B
    char C[101]; //texto da alternativa C
    char D[101]; //texto da alternativa D
 } Alternativa;

 typedef struct{
    char enunciado[501]; //string que armazena o enunciado da pergunta
    Alternativa alternativa; //chamo a struct Alternativa que contém as strings A, B, C e D
    char resposta; //char que armazena a letra da alternativa correta
    Dificuldade dificuldade; //chamo a enum Dificuldade que associa números a nomes definidos
    int identificador; //auxilia na identificação da pergunta mesmo se ela mudar de posição no vetor
 } Pergunta;

 #endif /*PERGUNTA_H*/