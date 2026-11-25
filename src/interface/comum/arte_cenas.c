/**
 * @file arte_cenas.c
 * @brief Compõe os cenários com os PNGs exportados pela equipe.
 */

#include "arte_cenas.h"
#include "cores.h"
#include <math.h>

/**
 * @brief Cenas do jogo.
 */
typedef enum
{
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
    PATIO_SOPRO,
    DRAGAO_SOPRO,
    TROTE_VERSO,
    TROTE_FOGO,
    TROTE_OVO,
    TROTE_CARTA_SELADA,
    ECO_CEU,
    ECO_POMBA,
    ECO_PEDRA,
    ECO_TELHADO,
    HUD_OVO,
    HUD_BURRO,
    HUD_DRAGAO,
    HUD_PRINCESA,
    OVO_MARCO,
    FUNDO_RENASCIMENTO,
    ECLOSAO,
    CARTA_ABERTURA,
    DRAGAO_CHEGADA,
    FAISCA_BATALHA,
    FAISCA_DESPERTAR,
    DRAGAO_DESPERTADO,
    PRINCIPE_DESPERTAR,
    TOTAL_ARTES,
} Arte;

typedef struct
{
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
    [PATIO_SOPRO] = {"assets/visual/fundos/patio_sopro.png"},
    [DRAGAO_SOPRO] = {"assets/visual/cenas/10/dragao_sopro.png"},
    [TROTE_VERSO] = {"assets/visual/icones/trote_carta_fechada.png"},
    [TROTE_FOGO] = {"assets/visual/icones/trote_carta_fogo.png"},
    [TROTE_OVO] = {"assets/visual/icones/trote_carta_ovo.png"},
    [TROTE_CARTA_SELADA] = {"assets/visual/cenas/12/carta_selada.png"},
    [ECO_CEU] = {"assets/visual/fundos/ceu_povoado.png"},
    [ECO_POMBA] = {"assets/visual/cenas/12/pomba.png"},
    [ECO_PEDRA] = {"assets/visual/cenas/12/passagem_pedra.png"},
    [ECO_TELHADO] = {"assets/visual/cenas/12/eco_telhado_chamine.png"},
    [HUD_OVO] = {"assets/visual/icones/ovo_hud.png"},
    [HUD_BURRO] = {"assets/visual/icones/ajuda_trote.png"},
    [HUD_DRAGAO] = {"assets/visual/icones/ajuda_sopro.png"},
    [HUD_PRINCESA] = {"assets/visual/icones/ajuda_eco.png"},
    [OVO_MARCO] = {"assets/visual/cenas/07/ovo_marco.png"},
    [FUNDO_RENASCIMENTO] = {"assets/visual/fundos/gruta_renascimento.png"},
    [ECLOSAO] = {"assets/visual/cenas/08/eclosao.png"},
    [CARTA_ABERTURA] = {"assets/visual/cenas/04/carta_selada.png"},
    [DRAGAO_CHEGADA] = {"assets/visual/cenas/25/dragao_voo.png"},
    [FAISCA_BATALHA] = {"assets/visual/cenas/06/faisca_48x54.png"},
    [FAISCA_DESPERTAR] = {"assets/visual/cenas/09/faisca_44x50.png"},
    [DRAGAO_DESPERTADO] = {"assets/visual/cenas/09/dragao_despertado.png"},
    [PRINCIPE_DESPERTAR] = {"assets/visual/cenas/09/principe.png"},
};

// A cor multiplica o RGB do sprite durante o desenho. O alfa 255 preserva a transparência do PNG.
static const Color luz_vale = {230, 208, 183, 255};     // Fim de tarde no menu, na rota e na derrota da jornada.
static const Color luz_creditos = {205, 179, 161, 255}; // Acompanha o mapa escurecido pelo véu.
static const Color luz_patio = {244, 232, 217, 255};    // Pedra iluminada pelo sol.
static const Color luz_torre = {212, 217, 231, 255};    // Sombra mais fria na abertura da torre.
static const Color luz_gruta = {199, 163, 192, 255};    // Reflexo violeta das rochas.

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
    for (int i = 0; i < TOTAL_ARTES; i++)
        if (IsTextureValid(artes[i].textura))
            UnloadTexture(artes[i].textura);
}

/**
 * @brief Desenha um PNG inteiro no seu tamanho nativo, incluindo margens transparentes e sombra.
 * @param arte Índice da textura na tabela.
 * @param x Posição da borda esquerda do PNG na tela lógica.
 * @param y Posição da borda superior do PNG na tela lógica.
 * @param luz Cor aplicada à textura durante o desenho.
 */
static void desenhar_png_com_luz(Arte arte, int x, int y, Color luz)
{
    Texture2D textura = artes[arte].textura;
    if (IsTextureValid(textura))
        DrawTexture(textura, x, y, luz);
}

/** @brief Desenha a textura com suas cores originais. */
static void desenhar_png(Arte arte, int x, int y)
{
    desenhar_png_com_luz(arte, x, y, WHITE);
}

void desenhar_arte_carta_burro(Rectangle area, int simbolo)
{
    const Arte faces[] = {TROTE_FOGO, TROTE_OVO, TROTE_CARTA_SELADA};
    bool revelada = simbolo >= 0 && simbolo < 3;
    Arte arte = revelada ? faces[simbolo] : TROTE_VERSO;
    Texture2D textura = artes[arte].textura;
    if (!IsTextureValid(textura))
        return;

    // Uma escala inteira mantém os pixels do símbolo nítidos.
    float escala = revelada ? 2.0f : 3.0f;
    Vector2 posicao = {
        area.x + (area.width - textura.width * escala) / 2,
        area.y + (area.height - textura.height * escala) / 2,
    };
    DrawTextureEx(textura, posicao, 0.0f, escala, WHITE);
}

void desenhar_arte_sopro(void)
{
    desenhar_png(PATIO_SOPRO, 160, 108);
    desenhar_png_com_luz(DRAGAO_SOPRO, 190, 182, luz_patio);
}

void desenhar_fundo_eco(Vector2 origem)
{
    desenhar_png(ECO_CEU, (int)origem.x, (int)origem.y);
}

void desenhar_pomba_eco(Vector2 centro, Rectangle campo)
{
    Texture2D textura = artes[ECO_POMBA].textura;
    if (!IsTextureValid(textura))
        return;

    // A posição física continua no peito. Os offsets localizam esse pivô no PNG de 60 × 50.
    Rectangle destino = {floorf(centro.x) - 33, floorf(centro.y) - 29, textura.width, textura.height};
    Rectangle visivel = GetCollisionRec(destino, campo);
    if (visivel.width > 0 && visivel.height > 0) {
        Rectangle fonte = {visivel.x - destino.x, visivel.y - destino.y, visivel.width, visivel.height};
        DrawTexturePro(textura, fonte, visivel, (Vector2){0}, 0.0f, WHITE);
    }
}

void desenhar_obstaculo_eco(Rectangle obstaculo, Rectangle campo, bool tem_telhado)
{
    Rectangle visivel = GetCollisionRec(obstaculo, campo);
    if (visivel.width <= 0 || visivel.height <= 0)
        return;

    DrawRectangleRec(visivel, COR_FACE_BOTAO);
    Texture2D pedra = artes[ECO_PEDRA].textura;
    if (IsTextureValid(pedra)) {
        const float altura_faixa = 28; // Recorte opaco das primeiras fileiras do sprite da passagem.
        float margem_x = visivel.x - obstaculo.x;
        for (float y = visivel.y; y < visivel.y + visivel.height; y += altura_faixa) {
            float altura = fminf(altura_faixa, visivel.y + visivel.height - y);
            Rectangle fonte = {2 + margem_x, 22, visivel.width, altura}; // Posição da alvenaria dentro do PNG.
            DrawTexturePro(pedra, fonte, (Rectangle){visivel.x, y, visivel.width, altura}, (Vector2){0}, 0, WHITE);
        }
    }

    Texture2D telhado = artes[ECO_TELHADO].textura;
    if (tem_telhado && IsTextureValid(telhado)) {
        // O telhado cabe na área sólida e acompanha o recorte horizontal da entrada e saída.
        float escala = obstaculo.width / telhado.width;
        Rectangle topo = {obstaculo.x, obstaculo.y, obstaculo.width, telhado.height * escala};
        Rectangle recorte = GetCollisionRec(topo, visivel);
        if (recorte.width > 0 && recorte.height > 0) {
            Rectangle fonte = {(recorte.x - topo.x) / escala, (recorte.y - topo.y) / escala,
                               recorte.width / escala, recorte.height / escala};
            DrawTexturePro(telhado, fonte, recorte, (Vector2){0}, 0, WHITE);
        }
    }
}

void desenhar_icone_quiz(IconeQuiz icone, Rectangle area, Color cor)
{
    const Arte icones[] = {HUD_OVO, HUD_BURRO, HUD_DRAGAO, HUD_PRINCESA};
    Texture2D textura = artes[icones[icone]].textura;
    if (IsTextureValid(textura)) {
        Vector2 posicao = {floorf(area.x + (area.width - textura.width) / 2),
                           floorf(area.y + (area.height - textura.height) / 2)};
        DrawTextureV(textura, posicao, cor);
    }
}

void desenhar_ovo_marco(Rectangle cena)
{
    DrawRectangleRec(cena, Fade(COR_ESCURO, 0.48f));
    Rectangle centro = {cena.x + (cena.width - 105) / 2, cena.y + (cena.height - 105) / 2, 105, 105}; // Medidas do sprite.
    desenhar_png(OVO_MARCO, (int)centro.x, (int)centro.y);
}

// As coordenadas abaixo são as da coluna destino do manifesto de sprites v2.
void desenhar_arte_menu(void)
{
    desenhar_png(MAPA_MENU, 28, 28);
    DrawRectangle(28, 28, 516, 484, COR_VEU_MAPA); // Escurece o mapa antes de desenhar o dragão.
    desenhar_png_com_luz(DRAGAO_MENU, 115, 153, luz_vale);
}

// Reaproveita o mapa da Jornada nos créditos. Aplica o véu escuro antes de desenhar o dragão.
void desenhar_arte_creditos(void)
{
    desenhar_png(MAPA_JORNADA, 28, 92);
    DrawRectangle(28, 92, 484, 316, COR_VEU_MAPA);
    desenhar_png_com_luz(DRAGAO_CREDITOS, 171, 162, luz_creditos);
}

/** @brief Interpola o voo entre pontos da estrada, em coordenadas do mapa. */
static void desenhar_rota_jornada(float passos)
{
    static const Vector2 rota[] = {
        {44, 253}, {96, 228}, {158, 213}, {207, 185}, {243, 147},
        {288, 116}, {334, 98}, {387, 78}, {438, 62},
    };
    const int ultimo = (int)(sizeof rota / sizeof rota[0]) - 1;
    int inicio = (int)passos;
    if (inicio >= ultimo)
        inicio = ultimo - 1;
    float fracao = passos - inicio;
    // A curva desacelera perto dos pontos de parada.
    fracao = fracao * fracao * (3.0f - 2.0f * fracao);
    Vector2 posicao = {
        rota[inicio].x + (rota[inicio + 1].x - rota[inicio].x) * fracao,
        rota[inicio].y + (rota[inicio + 1].y - rota[inicio].y) * fracao,
    };

    desenhar_png(MAPA_JORNADA, 28, 92);
    desenhar_png(BRILHO_JORNADA, 73, 353);
    desenhar_png(BRILHO_JORNADA, 203, 319);
    desenhar_png(BRILHO_JORNADA, 326, 224);
    desenhar_png(BRILHO_JORNADA, 456, 176);
    // Soma a origem do mapa e desconta o centro da silhueta dentro do PNG.
    desenhar_png_com_luz(DRAGAO_JORNADA, 28 + (int)posicao.x - 44, 92 + (int)posicao.y - 32, luz_vale);
}

void desenhar_impacto_batalha(float avanco, bool despertado)
{
    Color brilho = Fade(WHITE, 1.0f - avanco);
    desenhar_png_com_luz(despertado ? FAISCA_DESPERTAR : FAISCA_BATALHA, 328, 266, brilho);
}

// Escolhe pelo estado do quiz quais imagens desenhar, começando pelo fundo de cada cena.
void desenhar_arte_quiz(CenaQuiz cena, float passos)
{
    switch (cena) {
        case CENA_RENASCIMENTO:
            desenhar_png(FUNDO_RENASCIMENTO, 28, 92); // Mesmo encaixe do mapa no painel.
            desenhar_png(ECLOSAO, 119, 151); // Centraliza o recorte de 301 × 224 no cenário.
            break;
        case CENA_JORNADA:
            desenhar_rota_jornada(passos);
            break;
        case CENA_ABERTURA:
            desenhar_png(PATIO, 28, 92);
            desenhar_png_com_luz(PRINCESA_BATALHA, 388, 152, luz_torre);
            desenhar_png(CARTA_ABERTURA, 92, 248);
            break;
        case CENA_CHEGADA:
            desenhar_png(PATIO, 28, 92);
            desenhar_png_com_luz(DRAGAO_CHEGADA, 42, 161, luz_patio);
            desenhar_png_com_luz(PRINCIPE_BATALHA, 345, 238, luz_patio);
            desenhar_png_com_luz(PRINCESA_BATALHA, 388, 152, luz_torre);
            break;
        case CENA_DESPERTAR:
            desenhar_png(PATIO, 28, 92);
            desenhar_png_com_luz(DRAGAO_DESPERTADO, 38, 134, luz_patio);
            desenhar_png_com_luz(PRINCIPE_DESPERTAR, 364, 256, luz_patio);
            desenhar_png_com_luz(PRINCESA_BATALHA, 388, 152, luz_torre);
            break;
        case CENA_BATALHA:
            desenhar_png(PATIO, 28, 92);
            desenhar_png_com_luz(DRAGAO_BATALHA, 38, 180, luz_patio);
            desenhar_png_com_luz(PRINCIPE_BATALHA, 345, 238, luz_patio);
            desenhar_png_com_luz(PRINCESA_BATALHA, 388, 152, luz_torre);
            break;
        case CENA_VITORIA:
            desenhar_png(PATIO, 28, 92);
            desenhar_png_com_luz(DRAGAO_VITORIA, 36, 180, luz_patio);
            desenhar_png_com_luz(PRINCESA_VITORIA, 289, 244, luz_patio);
            desenhar_png_com_luz(PRINCIPE_VITORIA, 363, 328, luz_patio);
            desenhar_png(BRILHO_VITORIA, 391, 256);
            desenhar_png(BRILHO_JORNADA, 315, 280);
            break;
        case CENA_DERROTA:
            desenhar_png(PATIO, 28, 92);
            desenhar_png_com_luz(PRINCESA_DERROTA, 388, 152, luz_torre);
            desenhar_png_com_luz(DRAGAO_DERROTA, 41, 260, luz_patio);
            desenhar_png_com_luz(PRINCIPE_DERROTA, 368, 238, luz_patio);
            DrawRectangle(28, 92, 484, 316, Fade(BLACK, 0.24f)); // Na derrota, escurece toda a cena.
            break;
        case CENA_DERROTA_JORNADA:
            desenhar_png(FUNDO_DERROTA_JORNADA, 28, 92);
            desenhar_png_com_luz(DRAGAO_CAIDO_JORNADA, 109, 240, luz_vale);
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
            desenhar_png_com_luz(DRAGAO_BATALHA, 339, 195, luz_gruta);
            break;
        case GRUTA_OURO_BAIXO:
            desenhar_png_com_luz(OURO_BAIXO, 311, 242, luz_gruta);
            if (filhotes >= 1)
                desenhar_png_com_luz(FILHOTE_ESQUERDA, 260, 318, luz_gruta); // Ao lado da pilha pequena.
            if (filhotes >= 2)
                desenhar_png_com_luz(FILHOTE_DIREITA, 610, 318, luz_gruta);
            break;
        case GRUTA_OURO_MEDIO:
            desenhar_png_com_luz(OURO_MEDIO, 287, 222, luz_gruta);
            if (filhotes >= 1)
                desenhar_png_com_luz(FILHOTE_MEDIO, 239, 318, luz_gruta);
            if (filhotes >= 2)
                desenhar_png_com_luz(FILHOTE_DIREITA, 650, 318, luz_gruta); // À direita da pilha média.
            break;
        case GRUTA_OURO_ALTO:
            desenhar_png_com_luz(OURO_ALTO, 259, 196, luz_gruta);
            // Os recortes já vêm voltados para o centro sem precisar espelhar.
            if (filhotes >= 1)
                desenhar_png_com_luz(FILHOTE_ESQUERDA, 213, 318, luz_gruta);
            if (filhotes >= 2)
                desenhar_png_com_luz(FILHOTE_DIREITA, 685, 318, luz_gruta);
            break;
    }
}
