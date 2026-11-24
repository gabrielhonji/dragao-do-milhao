/**
 * @file minigame_sopro.c
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 2026-11-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "minigame_sopro.h"

MinigameSopro jogo;
//adicionar srand(time(NULL)) na main
void iniciarMinigameSopro(){

    jogo.tempo =0;
    jogo.posicao_barra =0;
    jogo.velocidade_barra =10;
    jogo.resultado =0;

    jogo.zona_inicio =rand()%43;
    jogo.zona_final =jogo.zona_inicio +8;
}

float atualizaBarra(float posicao_barra, float velocidade_da_barra, float tempo_decorrido){
   posicao_barra += velocidade_da_barra * tempo_decorrido;
   return posicao_barra;
}

void atualizarMinigame(float tempo_decorrido, int apertou_espaco){

    if(jogo.resultado!=0){
        return;
    }

    jogo.tempo+=tempo_decorrido;
      if(jogo.tempo >=10){
         jogo.resultado =-1;
         return;
      }

    jogo.posicao_barra=atualizaBarra(jogo.posicao_barra,jogo.velocidade_barra,tempo_decorrido);

    if(jogo.posicao_barra>=50){
        jogo.posicao_barra=50;
        jogo.velocidade_barra=-10;
    }
    else if(jogo.posicao_barra<=0){
        jogo.posicao_barra=0;
        jogo.velocidade_barra=10;
    }

    if(apertou_espaco==1){
        if(jogo.posicao_barra>=jogo.zona_inicio && jogo.posicao_barra<=jogo.zona_final){
            jogo.resultado =1;
        }
        else{
            jogo.resultado =-1;
        }
    }
}

int obterResultadoMinigame(void){
   return jogo.resultado;
}