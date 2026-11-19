/**
 * @file minigame_burro.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-11-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include "minigame_burro.h"
 #include <time.h>

 int jogarMinigame_burro(){
    int cartas[6]={0,0,1,1,2,2};
    int escolha_1,escolha_2;
    int chances=2;
    int tentativas_corretas=0;
    srand(time(NULL));

    for(int i=0;i<6;i++){

    int j=rand()%6;

    int temp=cartas[i];
    cartas[i]=cartas[j];
    cartas[j]=temp;

    }

    printf("Escolha a primeira carta:\n");
    scanf("%d",&escolha_1);
    printf("Escolha a segunda carta:\n");
    scanf("%d",&escolha_2);

    while(chances>=0 && tentativas_corretas<3){
        if(cartas[escolha_1]==cartas[escolha_2]){
            printf("Correto\n");
            tentativas_corretas++;
            if(tentativas_corretas=3){
                break
            }

        }else{
            printf("Incorreto\n");
            chances--;
        }
    }
    return 0;
 }