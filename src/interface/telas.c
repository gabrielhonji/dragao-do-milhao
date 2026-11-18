/**
 * @file telas.c
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief 
 * @version 0.1
 * @date 2026-11-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "telas.h"
#include <raylib.h>

void executar_tela_teste(void) {
    InitWindow(960, 540, "Laboratório do Dragao");
    SetTargetFPS(60);

    Image folha = GenImageColor(64, 32, BLANK);
    ImageDrawRectangle(&folha, 4, 12, 24, 8, GOLD);
    ImageDrawRectangle(&folha, 44, 4, 8, 24, GREEN);

    Texture2D sprite = LoadTextureFromImage(folha);
    UnloadImage(folha);
    SetTextureFilter(sprite, TEXTURE_FILTER_POINT);

    const float duracaoQuadro = 0.125f;
    float tempoAcumulado = 0.0f;
    int quadro = 0;

    while (!WindowShouldClose()) {
        tempoAcumulado += GetFrameTime();

        while (tempoAcumulado >= duracaoQuadro) {
            tempoAcumulado -= duracaoQuadro;
            quadro = (quadro + 1) % 2;
        }

        Rectangle origem = {quadro * 32.0f, 0, 32, 32};
        Rectangle destino = {416, 206, 128, 128};
        
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Dois quados de animacao", 48, 48, 24, RAYWHITE);
        DrawTexturePro(sprite, origem, destino, (Vector2){0, 0}, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(sprite);
    CloseWindow();
}
