/**
 * @file quiz.c
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Implementação das funções definidas no header quiz.h
 * @version 0.1
 * @date 2026-11-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "quiz.h"

 /**
  * @brief Função usada para inicializar a estrutura partida
  * 
  * @param partidas recebe o ponteiro para partida que será inicializado
  */
 void inicializa_partida(Partida *partidas){
   partidas->nome_jogador[0] = '\0'; //zerando a minha string do nome colocando o caracter finalizador na primeira posição
   partidas->questao_atual = 0; //ainda não estou em nenhuma questao != de ID
   partidas->acertos = 0; //zero os acertos
   partidas->situacao = andamento;
   for(int i = 0; i<15; i++){
      partidas->ids_sorteados[i] = -1; //um a um coloco os ids em -1
   }
   for(int i = 0; i<3; i++){
      partidas->minigames_usados[i] = 0;  //um a um indico que os minigames ainda não foram usados
   }
 } //INICIALIZA PARTIDA

 

 /**
  * @brief Função que recebe qual vai ser a próxima questão e retorna o nível de dificuldade dessa
  * 
  * @param partidas 
  * @return Dificuldade 
  */
 Dificuldade determina_dificuldade(Partida *partidas){

   if((partidas->questao_atual+1) < 1 || (partidas->questao_atual+1) > 15){
      return 0; //caso as próximas questões são abaixo de 1 ou acima de 15
   }

   if((partidas->questao_atual+1) >= 1 && (partidas->questao_atual+1) <=2){
      return muito_facil;
   }else if((partidas->questao_atual+1) <= 4){
         return facil;
   }else if((partidas->questao_atual+1) <= 8){
         return medio;
   }else if((partidas->questao_atual+1) <= 12){
         return dificil;
   }else{
         return muito_dificil;
   }
}//DETERMINA DIFICULDADE


/**
 * @brief Procura no meu vetor de ids já sorteados o id atual, para ver se pode apresentar essa pergunta ou se ela já foi apresentada
 * 
 * @param partida struct partida sendo passado por referência
 * @param id_pergunta id da pergunta atual que o programa irá verificar se já foi sorteada
 * @return int 
 */
 int pergunta_ja_sorteada(Partida *partida, int id_pergunta){

   for(int i = 0; i<partida->questao_atual; i++){
      if(partida->ids_sorteados[i] == id_pergunta){
         return 1;
      }
   }
   return 0;
}// PERGUNTA JA SORTEADA
