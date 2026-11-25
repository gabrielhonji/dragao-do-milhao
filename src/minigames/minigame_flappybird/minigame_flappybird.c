/**
 * @file minigame_flappybird.c
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 2026-11-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "minigame_flappybird.h"
#include <time.h>

    FlappyBird jogo;
    Obstaculo cano;

void iniciarMinigame_flappybird(){

    srand(time(NULL));

    jogo.velocidade_pombo=0;
    jogo.pontuacao=0;
    jogo.posy_pombo=270;
    jogo.posx_pombo=200;
    jogo.gravidade=800;
    jogo.resultado=0;

    cano.pos_x=960;
    cano.abertura_y=270;
    cano.largura= 70;
    cano.tamanho_abertura=160;
    cano.velocidade_x=-200;
    cano.verifica_ponto=0;
}



void atualizaMinigame_flappybird(float deltaTime, int apertou_espaco){

    if(jogo.resultado!=0){
        return;
    }   
    jogo.velocidade_pombo+=jogo.gravidade*deltaTime;
    if(apertou_espaco==1){
        jogo.velocidade_pombo=-300;
    }
    jogo.posy_pombo+=jogo.velocidade_pombo*deltaTime;
    if(jogo.posy_pombo>=495){
        jogo.resultado=-1;
    }
    if(jogo.posy_pombo<=15){
        jogo.resultado=-1;
    }

    cano.pos_x+=cano.velocidade_x*deltaTime;
    if(cano.pos_x<= -cano.largura){
        cano.pos_x=960;
        cano.abertura_y=rand()%(410-130+1)+130;
        cano.verifica_ponto=0;
    }
    if(jogo.posx_pombo+15>=cano.pos_x && jogo.posx_pombo-15<=cano.pos_x+cano.largura){
        if(jogo.posy_pombo-15<cano.abertura_y-cano.tamanho_abertura/2.0f ||  jogo.posy_pombo+15>cano.abertura_y+cano.tamanho_abertura/2.0f){    
            jogo.resultado=-1;
        }
        if(jogo.resultado != 0){
            return;
        }
    }
    if(jogo.posx_pombo>cano.pos_x+cano.largura){
        if(cano.verifica_ponto==0){
            jogo.pontuacao++;
            cano.verifica_ponto=1;
        }
    }
    if(jogo.pontuacao==7){
        jogo.resultado=1;
    }
}