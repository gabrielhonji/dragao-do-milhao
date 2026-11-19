/**
 * @file telas.c
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Abre a janela de teste e alterna entre o menu e a jornada.
 * @version 0.1
 * @date 2026-11-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "telas.h"
#include "componentes.h"

typedef enum { TELA_MENU, TELA_JORNADA } Tela;
enum { LARGURA_BASE = 960, ALTURA_BASE = 540 };

/**
 * @brief Calcula onde exibir a tela na janela.
 * @return Retângulo centralizado, na maior escala inteira que cabe quando possível.
 */
static Rectangle calcular_area_exibicao(void)
{
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float escala_x = (float)largura_tela / LARGURA_BASE;
    float escala_y = (float)altura_tela / ALTURA_BASE;
    float escala = escala_x < escala_y ? escala_x : escala_y;

    // Uma escala inteira mantém todos os pixels do mesmo tamanho.
    if (escala >= 1.0f) {
        escala = (int)escala;
    }

    int largura_exibida = (int)(LARGURA_BASE * escala);
    int altura_exibida = (int)(ALTURA_BASE * escala);
    return (Rectangle){(largura_tela - largura_exibida) / 2, (altura_tela - altura_exibida) / 2,
                       largura_exibida, altura_exibida};
}

/**
 * @brief Carrega a Pixelify Sans com caracteres ASCII e Latin-1 imprimíveis.
 * @param tamanho Altura em pixels usada para rasterizar os caracteres.
 * @return Fonte carregada ou a fonte padrão da raylib se houver falha.
 */
static Font carregar_fonte_interface(int tamanho)
{
    // ASCII e Latin-1 imprimíveis somam 191 códigos.
    int caracteres[191];
    int total = 0;

    // Carregamento dos caracteres em ASCII (ex: ' ', '1', '.').
    for (int codigo = 32; codigo <= 126; codigo++) {
        caracteres[total++] = codigo;
    }
    // Carregamento de caracteres da extensão Latin-1 (ex: 'á', 'õ', 'ç').
    for (int codigo = 160; codigo <= 255; codigo++) {
        caracteres[total++] = codigo;
    }

    Font fonte = LoadFontEx("assets/fontes/PixelifySans.ttf", tamanho, caracteres, total);
    if (fonte.texture.id != 0 && fonte.texture.id != GetFontDefault().texture.id) {
        SetTextureFilter(fonte.texture, TEXTURE_FILTER_POINT);
    }
    return fonte;
}

/**
 * @brief Alterna entre a janela normal e uma janela sem bordas do tamanho do monitor.
 * @param tela_cheia Indica se a janela já ocupa o monitor.
 * @param posicao_janela Guarda a posição para restaurar a janela normal.
 * @return Novo estado da tela cheia.
 */
static bool alternar_tela_cheia(bool tela_cheia, Vector2 *posicao_janela)
{
    if (tela_cheia) {
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
        SetWindowSize(LARGURA_BASE, ALTURA_BASE);
        SetWindowPosition((int)posicao_janela->x, (int)posicao_janela->y);
        return false;
    }

    *posicao_janela = GetWindowPosition();
    int monitor = GetCurrentMonitor();
    Vector2 posicao_monitor = GetMonitorPosition(monitor);

    // Modifica a janela existente sem pedir uma troca de modo de vídeo ao monitor.
    SetWindowState(FLAG_WINDOW_UNDECORATED);
    SetWindowPosition((int)posicao_monitor.x, (int)posicao_monitor.y);
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    return true;
}

/**
 * @brief Indica se a textura pertence a uma fonte personalizada carregada.
 * @param fonte Fonte devolvida pelo carregador da raylib.
 * @return Verdadeiro quando a textura existe e é diferente da fonte padrão.
 */
static bool fonte_carregada(Font fonte)
{
    return fonte.texture.id != 0 && fonte.texture.id != GetFontDefault().texture.id;
}

void executar_tela_teste(void)
{
    Tela tela_atual = TELA_MENU;
    const Rectangle jogar = {48, 188, 280, 52};
    const Rectangle voltar = {24, 464, 200, 52};
    const Rectangle mapa = {24, 88, 492, 325};
    const Rectangle quiz = {544, 88, 392, 325};

    InitWindow(LARGURA_BASE, ALTURA_BASE, "Dragão do Milhão");
    SetTargetFPS(60);

    // Rasterização das fontes separadamente, tanto a do corpo quanto a do título.
    const int tamanho_corpo = 24;
    Font fonte = carregar_fonte_interface(tamanho_corpo);
    if (!fonte_carregada(fonte)) {
        TraceLog(LOG_ERROR, "Falha ao carregar assets/fontes/PixelifySans.ttf");
        CloseWindow();
        return;
    }
    const int tamanho_titulo = 48;
    Font fonte_titulo = carregar_fonte_interface(tamanho_titulo);
    if (!fonte_carregada(fonte_titulo)) {
        TraceLog(LOG_ERROR, "Falha ao carregar a fonte do título");
        UnloadFont(fonte);
        CloseWindow();
        return;
    }

    bool tela_cheia = false;
    Vector2 posicao_janela = GetWindowPosition();
    int escala_da_fonte = 1;
    int ultima_escala_solicitada = 1;

    while (!WindowShouldClose()) {
        // Modo fullscreen sendo uma Janela maximizada sem bordas.
        if (IsKeyPressed(KEY_F11)) {
            tela_cheia = alternar_tela_cheia(tela_cheia, &posicao_janela);
        }

        // Sem uma área útil, avança o quadro sem desenhar a interface.
        Rectangle area_exibida = calcular_area_exibicao();
        if (area_exibida.width <= 0 || area_exibida.height <= 0) {
            BeginDrawing();
            EndDrawing();
            continue;
        }

        // Converte as coordenadas do mouse para bater com a nova escala da janela.
        SetMouseOffset(-(int)area_exibida.x, -(int)area_exibida.y);
        SetMouseScale(LARGURA_BASE / area_exibida.width, ALTURA_BASE / area_exibida.height);

        float escala = area_exibida.width / LARGURA_BASE;
        int escala_dos_pixels = (int)(escala * GetRenderWidth() / GetScreenWidth() + 0.5f);
        if (escala_dos_pixels < 1) {
            escala_dos_pixels = 1;
        }

        // Rasteriza novas letras só quando muda o tamanho em pixels na tela.
        if (escala_dos_pixels != ultima_escala_solicitada) {
            ultima_escala_solicitada = escala_dos_pixels;
            
            if (escala_dos_pixels != escala_da_fonte) {
                Font novo_texto = carregar_fonte_interface(tamanho_corpo * escala_dos_pixels);
                Font novo_titulo = carregar_fonte_interface(tamanho_titulo * escala_dos_pixels);

                if (fonte_carregada(novo_texto) && fonte_carregada(novo_titulo)) {
                    UnloadFont(fonte);
                    UnloadFont(fonte_titulo);
                    fonte = novo_texto;
                    fonte_titulo = novo_titulo;
                    escala_da_fonte = escala_dos_pixels;
                    TraceLog(LOG_INFO, "Fontes atualizadas: texto %d px, titulo %d px",
                             fonte.baseSize, fonte_titulo.baseSize);
                } else {
                    if (fonte_carregada(novo_texto)) {
                        UnloadFont(novo_texto);
                    }
                    if (fonte_carregada(novo_titulo)) {
                        UnloadFont(novo_titulo);
                    }
                    TraceLog(LOG_WARNING, "Mantendo as fontes da escala anterior");
                }
            }
        }

        bool confirmou = IsKeyPressed(KEY_ENTER);

        if (tela_atual == TELA_MENU) {
            if (confirmou || botao_clicado(jogar)) {
                tela_atual = TELA_JORNADA;
            }
        } else if (confirmou || botao_clicado(voltar)) {
            tela_atual = TELA_MENU;
        }

        // A câmera converte as coordenadas lógicas em coordenadas da janela.
        Camera2D camera = {.offset = {area_exibida.x, area_exibida.y}, .zoom = escala};
        BeginDrawing();
        
        ClearBackground(BLACK);
        BeginScissorMode((int)area_exibida.x, (int)area_exibida.y, (int)area_exibida.width,
                         (int)area_exibida.height);
        BeginMode2D(camera);
        DrawRectangle(0, 0, LARGURA_BASE, ALTURA_BASE, RAYWHITE);

        if (tela_atual == TELA_MENU) {
            DrawTextEx(fonte_titulo, "Dragão do Milhão", (Vector2){48, 96}, tamanho_titulo, 1,
                       BLACK);
            desenhar_botao(fonte, jogar, "Jogar");
        } else {
            DrawTextEx(fonte, "Jornada - Questão 1 de 15", (Vector2){24, 24}, 24, 1, BLACK);

            DrawRectangleRec(mapa, LIGHTGRAY);
            DrawTextEx(fonte, "Mapa aéreo", (Vector2){48, 112}, 24, 1, BLACK);

            DrawRectangleRec(quiz, LIGHTGRAY);
            DrawTextEx(fonte, "Painel do quiz", (Vector2){568, 112}, 24, 1, BLACK);

            desenhar_botao(fonte, voltar, "Voltar");
        }
        EndMode2D();
        EndScissorMode();
        EndDrawing();
    }

    UnloadFont(fonte_titulo);
    UnloadFont(fonte);
    CloseWindow();
}
