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

 typedef struct{
    char A[101];
    char B[101];
    char C[101];
    char D[101];
 } Alternativa;

 typedef struct{
    char enunciado[501];
    Alternativa alternativa;
    char resposta;
    Dificuldade dificuldade;  
    char dica[201];
    int identificador; //auxilia na identificação da pergunta mesmo se ela mudar de posição no vetor
 } Pergunta;

 #endif /*PERGUNTA_H*/