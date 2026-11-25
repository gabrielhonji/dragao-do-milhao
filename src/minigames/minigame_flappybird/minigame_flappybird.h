#ifndef MINIGAME_FLAPPYBIRD_H
#define MINIGAME_FLAPPYBIRD_H

typedef struct{
    float posy_pombo;
    float posx_pombo;
    float velocidade_pombo;
    float gravidade;
    int pontuacao;
    int resultado;
}FlappyBird;

typedef struct{
    float pos_x;
    float velocidade_x;
    float abertura_y;
    float largura;
    float tamanho_abertura;
    int verifica_ponto;
}Obstaculo;

extern FlappyBird jogo;
extern Obstaculo cano;

void iniciarMinigame_flappybird(void);
void atualizaMinigame_flappybird(float deltaTime, int apertou_espaco);

#endif