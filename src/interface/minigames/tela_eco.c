/**
 * @file tela_eco.c
 * @brief Lê Espaço e apresenta o percurso do Eco da Torre.
 */

#include "tela_eco.h"
#include <math.h>
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

static const Rectangle campo = {160, 108, ECO_LARGURA, ECO_ALTURA}; // Mesmo quadro usado pelo Sopro.
static const Rectangle voltar_eco = {600, 484, 200, 44};            // À direita da mensagem de resultado.

bool atualizar_tela_eco(FlappyBird *jogo, bool janela_em_foco)
{
    if (jogo->resultado != 0)
        return botao_clicado(voltar_eco);

    if (janela_em_foco) {
        // Limita o avanço num quadro lento para a pomba não atravessar um obstáculo entre dois testes.
        float tempo_quadro = fminf(GetFrameTime(), 0.05f);
        atualizaMinigame_flappybird(jogo, tempo_quadro, IsKeyPressed(KEY_SPACE));
    }
    return false;
}

/**
 * @brief Desenha as duas partes sólidas do obstáculo e mantém o vão livre.
 * @param cano Posição horizontal e centro do vão no campo.
 */
static void desenhar_obstaculo(const Obstaculo *cano)
{
    // O desenho veste os dois retângulos sólidos e mantém o mesmo vão usado pela colisão.
    float x = campo.x + cano->pos_x;
    float meio_vao = ECO_TAMANHO_ABERTURA / 2.0f;
    float topo_vao = cano->abertura_y - meio_vao;
    float fim_vao = cano->abertura_y + meio_vao;
    Rectangle superior = {x, campo.y, ECO_LARGURA_OBSTACULO, topo_vao};
    Rectangle inferior = {x, campo.y + fim_vao, ECO_LARGURA_OBSTACULO, campo.height - fim_vao};
    desenhar_obstaculo_eco(superior, campo, false);
    desenhar_obstaculo_eco(inferior, campo, true);
}

void desenhar_eco(const FlappyBird *jogo, Font fonte, Font fonte_titulo, int tamanho_corpo, int tamanho_titulo)
{
    DrawTextEx(fonte_titulo, "Eco da Torre", (Vector2){160, 12}, tamanho_titulo, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, "Espaço para bater asas. Passe pelos vãos.", (Vector2){160, 75}, tamanho_corpo, 1, COR_TEXTO_CLARO);
    desenhar_painel((Rectangle){156, 104, 648, 344}, COR_ESCURO); // Moldura de 4 px em torno do campo.
    desenhar_fundo_eco((Vector2){campo.x, campo.y});
    desenhar_obstaculo(&jogo->cano);
    desenhar_pomba_eco((Vector2){campo.x + jogo->posx_pombo, campo.y + jogo->posy_pombo}, campo);

    const char *placar = TextFormat("Passagens: %d/%d", jogo->pontuacao, ECO_META);
    DrawTextEx(fonte, placar, (Vector2){160, 454}, tamanho_corpo, 1, COR_TEXTO_CLARO);
    if (jogo->resultado != 0) {
        const char *mensagem = jogo->resultado == 1 ? "Carta entregue!" : "A pomba caiu!";
        Color cor = jogo->resultado == 1 ? COR_BORDA_ACERTO : COR_BORDA_ERRO;
        DrawTextEx(fonte, mensagem, (Vector2){160, 494}, tamanho_corpo, 1, cor);
        desenhar_botao(fonte, voltar_eco, "Voltar à pergunta");
    }
}
