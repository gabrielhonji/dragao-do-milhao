/**
 * @file arte_cenas.c
 * @brief Compõe os cenários com os PNGs exportados pela equipe.
 */

#include "arte_cenas.h"
#include "cores.h"

/**
 * @brief Cenas do jogo.
 */
typedef enum {
    MAPA_MENU,
    DRAGAO_MENU,
    MAPA_JORNADA,
    DRAGAO_JORNADA,
    BRILHO_JORNADA,
    PATIO,
    DRAGAO_BATALHA,
    PRINCIPE_BATALHA,
    PRINCESA_BATALHA,
    DRAGAO_VITORIA,
    PRINCESA_VITORIA,
    PRINCIPE_VITORIA,
    BRILHO_VITORIA,
    PRINCESA_DERROTA,
    DRAGAO_DERROTA,
    PRINCIPE_DERROTA,
    FUNDO_GRUTA,
    OURO_BAIXO,
    OURO_MEDIO,
    OURO_ALTO,
    FILHOTE_MEDIO,
    FILHOTE_ESQUERDA,
    FILHOTE_DIREITA,
    DRAGAO_CREDITOS,
    FUNDO_DERROTA_JORNADA,
    DRAGAO_CAIDO_JORNADA,
    TOTAL_ARTES,
} Arte;

typedef struct {
    const char *arquivo;
    Texture2D textura;
} ArteCena;

// O índice associa cada caminho ao lugar onde sua textura fica carregada.
static ArteCena artes[TOTAL_ARTES] = {
    [MAPA_MENU] = {"assets/visual/fundos/mapa_menu.png"},
    [DRAGAO_MENU] = {"assets/visual/cenas/01/dragao_voo.png"},
    [MAPA_JORNADA] = {"assets/visual/fundos/mapa.png"},
    [DRAGAO_JORNADA] = {"assets/visual/cenas/02/dragao_voo.png"},
    [BRILHO_JORNADA] = {"assets/visual/cenas/02/brilho_10x10.png"},
    [PATIO] = {"assets/visual/fundos/patio.png"},
    [DRAGAO_BATALHA] = {"assets/visual/cenas/03/dragao.png"},
    [PRINCIPE_BATALHA] = {"assets/visual/cenas/03/principe.png"},
    [PRINCESA_BATALHA] = {"assets/visual/cenas/03/princesa_torre.png"},
    [DRAGAO_VITORIA] = {"assets/visual/cenas/14/dragao.png"},
    [PRINCESA_VITORIA] = {"assets/visual/cenas/14/princesa_livre.png"},
    [PRINCIPE_VITORIA] = {"assets/visual/cenas/14/principe_caido.png"},
    [BRILHO_VITORIA] = {"assets/visual/cenas/04/brilho_15x15.png"},
    [PRINCESA_DERROTA] = {"assets/visual/cenas/15/princesa_torre.png"},
    [DRAGAO_DERROTA] = {"assets/visual/cenas/15/dragao_derrotado.png"},
    [PRINCIPE_DERROTA] = {"assets/visual/cenas/15/principe.png"},
    [FUNDO_GRUTA] = {"assets/visual/fundos/gruta.png"},
    [OURO_BAIXO] = {"assets/visual/cenas/17/dragao_e_ouro.png"},
    [OURO_MEDIO] = {"assets/visual/cenas/18/dragao_e_ouro.png"},
    [OURO_ALTO] = {"assets/visual/cenas/19/dragao_e_ouro.png"},
    [FILHOTE_MEDIO] = {"assets/visual/cenas/18/filhote_gruta_direita.png"},
    [FILHOTE_ESQUERDA] = {"assets/visual/cenas/19/filhote_gruta_direita.png"},
    [FILHOTE_DIREITA] = {"assets/visual/cenas/19/filhote_gruta_esquerda.png"},
    [DRAGAO_CREDITOS] = {"assets/visual/cenas/24/dragao_voo.png"},
    [FUNDO_DERROTA_JORNADA] = {"assets/visual/fundos/derrota_jornada.png"},
    [DRAGAO_CAIDO_JORNADA] = {"assets/visual/cenas/derrota_jornada/dragao_caido.png"},
};

void carregar_arte_cenas(void)
{
    for (int i = 0; i < TOTAL_ARTES; i++) {
        artes[i].textura = LoadTexture(artes[i].arquivo);
        if (IsTextureValid(artes[i].textura))
            SetTextureFilter(artes[i].textura, TEXTURE_FILTER_POINT);
    }
}

void descarregar_arte_cenas(void)
{
    for (int i = 0; i < TOTAL_ARTES; i++) {
        if (IsTextureValid(artes[i].textura))
            UnloadTexture(artes[i].textura);
    }
}

/**
 * @brief Desenha um PNG inteiro no seu tamanho nativo, incluindo margens transparentes e sombra.
 * @param arte Índice da textura na tabela.
 * @param x Posição da borda esquerda do PNG na tela lógica.
 * @param y Posição da borda superior do PNG na tela lógica.
 */
static void desenhar_png(Arte arte, int x, int y)
{
    Texture2D textura = artes[arte].textura;
    if (IsTextureValid(textura))
        DrawTexture(textura, x, y, WHITE);
}

// As coordenadas abaixo são as da coluna destino do manifesto de sprites v2.
void desenhar_arte_menu(void)
{
    desenhar_png(MAPA_MENU, 28, 28);
    DrawRectangle(28, 28, 516, 484, COR_VEU_MAPA); // Escurece o mapa antes de desenhar o dragão.
    desenhar_png(DRAGAO_MENU, 115, 153);
}

// Reaproveita o mapa da Jornada nos créditos. Aplica o véu escuro antes de desenhar o dragão.
void desenhar_arte_creditos(void)
{
    desenhar_png(MAPA_JORNADA, 28, 92);
    DrawRectangle(28, 92, 484, 316, COR_VEU_MAPA);
    desenhar_png(DRAGAO_CREDITOS, 171, 162);
}

// Escolhe pelo estado do quiz quais imagens desenhar, começando pelo fundo de cada cena.
void desenhar_arte_quiz(CenaQuiz cena)
{
    switch (cena) {
    case CENA_JORNADA:
        desenhar_png(MAPA_JORNADA, 28, 92);
        desenhar_png(BRILHO_JORNADA, 73, 348);
        desenhar_png(BRILHO_JORNADA, 203, 319);
        desenhar_png(BRILHO_JORNADA, 326, 224);
        desenhar_png(BRILHO_JORNADA, 456, 176);
        desenhar_png(DRAGAO_JORNADA, 124, 256);
        break;
    case CENA_BATALHA:
        desenhar_png(PATIO, 28, 92);
        desenhar_png(DRAGAO_BATALHA, 38, 180);
        desenhar_png(PRINCIPE_BATALHA, 345, 238);
        desenhar_png(PRINCESA_BATALHA, 388, 152);
        break;
    case CENA_VITORIA:
        desenhar_png(PATIO, 28, 92);
        desenhar_png(DRAGAO_VITORIA, 36, 180);
        desenhar_png(PRINCESA_VITORIA, 289, 244);
        desenhar_png(PRINCIPE_VITORIA, 363, 328);
        desenhar_png(BRILHO_VITORIA, 391, 256);
        desenhar_png(BRILHO_JORNADA, 315, 280);
        break;
    case CENA_DERROTA:
        desenhar_png(PATIO, 28, 92);
        desenhar_png(PRINCESA_DERROTA, 388, 152);
        desenhar_png(DRAGAO_DERROTA, 41, 260);
        desenhar_png(PRINCIPE_DERROTA, 368, 238);
        DrawRectangle(28, 92, 484, 316, Fade(BLACK, 0.24f)); // Na derrota, escurece toda a cena.
        break;
    case CENA_DERROTA_JORNADA:
        desenhar_png(FUNDO_DERROTA_JORNADA, 28, 92);
        desenhar_png(DRAGAO_CAIDO_JORNADA, 109, 240);
        break;
    }
}

void desenhar_arte_gruta(EstadoGruta estado, int filhotes, bool mostrar_dragao)
{
    desenhar_png(FUNDO_GRUTA, 24, 80);
    if (!mostrar_dragao)
        return;

    switch (estado) {
    case GRUTA_VAZIA:
        // A retirada antes do primeiro marco reutiliza o dragão em pé, centralizado no chão.
        desenhar_png(DRAGAO_BATALHA, 339, 195);
        break;
    case GRUTA_OURO_BAIXO:
        desenhar_png(OURO_BAIXO, 311, 242);
        if (filhotes >= 1)
            desenhar_png(FILHOTE_ESQUERDA, 260, 318); // Ao lado da pilha pequena.
        if (filhotes >= 2)
            desenhar_png(FILHOTE_DIREITA, 610, 318);
        break;
    case GRUTA_OURO_MEDIO:
        desenhar_png(OURO_MEDIO, 287, 222);
        if (filhotes >= 1)
            desenhar_png(FILHOTE_MEDIO, 239, 318);
        if (filhotes >= 2)
            desenhar_png(FILHOTE_DIREITA, 650, 318); // À direita da pilha média.
        break;
    case GRUTA_OURO_ALTO:
        desenhar_png(OURO_ALTO, 259, 196);
        // Os recortes já vêm voltados para o centro sem precisar espelhar.
        if (filhotes >= 1)
            desenhar_png(FILHOTE_ESQUERDA, 213, 318);
        if (filhotes >= 2)
            desenhar_png(FILHOTE_DIREITA, 685, 318);
        break;
    }
}
