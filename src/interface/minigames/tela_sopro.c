/**
 * @file tela_sopro.c
 * @brief Lê os controles e exibe o Sopro Ancestral.
 */

#include "tela_sopro.h"
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

static const Rectangle voltar_sopro = {600, 484, 200, 44}; // À direita da mensagem de resultado.

bool atualizar_tela_sopro(MinigameSopro *sopro, bool janela_em_foco)
{
    if (sopro->resultado == 0) {
        // IsKeyPressed soma carga a cada novo aperto de Espaço.
        atualizarMinigame(sopro, GetFrameTime(), janela_em_foco && IsKeyPressed(KEY_SPACE));
        return false;
    }
    return botao_clicado(voltar_sopro);
}

void desenhar_sopro(const MinigameSopro *sopro, Font fonte, Font fonte_titulo,
                    int tamanho_corpo, int tamanho_titulo, int largura, int altura)
{
    float tempo_restante = SOPRO_TEMPO_LIMITE - sopro->tempo;
    if (tempo_restante <= 0)
        tempo_restante = 0.0f;

    // A pressão do tempo aparece na borda: ela vai de 24 a 160 pixels lógicos e ganha opacidade.
    float progresso = 1.0f - tempo_restante / SOPRO_TEMPO_LIMITE;
    int espessura = 24 + (int)(136.0f * progresso);
    Color borda = Fade(RED, 0.70f * progresso * progresso);
    Color transparente = Fade(RED, 0.0f);

    const Rectangle barra = {160, 458, 640, 18};
    Rectangle preenchimento = barra;
    preenchimento.width *= sopro->carga / 100.0f; // A porcentagem define quanto da barra é preenchida.

    desenhar_painel((Rectangle){156, 104, 648, 344}, COR_ESCURO);
    desenhar_arte_sopro();

    DrawRectangleGradientV(0, 0, largura, espessura, borda, transparente);
    DrawRectangleGradientH(0, 0, espessura, altura, borda, transparente);
    DrawRectangleGradientH(largura - espessura, 0, espessura, altura, transparente, borda);
    DrawRectangleGradientV(0, altura - espessura, largura, espessura, transparente, borda);

    DrawTextEx(fonte, TextFormat("%.1f s", tempo_restante), (Vector2){720, 30}, tamanho_corpo, 1, BLACK);
    DrawTextEx(fonte_titulo, "Sopro Ancestral", (Vector2){160, 12}, tamanho_titulo, 1, BLACK);
    DrawTextEx(fonte, "Aperte Espaço repetidamente para carregar o sopro.", (Vector2){160, 75}, tamanho_corpo, 1, BLACK);

    DrawRectangleRec(barra, COR_FACE_BLOQUEADA);
    DrawRectangleRec(preenchimento, COR_FACE_PRIMARIA);

    if (sopro->resultado != 0) {
        if (sopro->resultado == 1)
            DrawTextEx(fonte, "Sopro carregado!", (Vector2){160, 494}, tamanho_corpo, 1, BLACK);
        else
            DrawTextEx(fonte, "Tempo esgotado!", (Vector2){160, 494}, tamanho_corpo, 1, BLACK);
        desenhar_botao(fonte, voltar_sopro, "Voltar à pergunta");
    }
}
