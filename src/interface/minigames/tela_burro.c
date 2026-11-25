/**
 * @file tela_burro.c
 * @brief Lê os cliques e exibe as cartas do Trote Certeiro.
 */

#include "tela_burro.h"
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

static const Rectangle voltar_burro = {536, 480, 200, 44};

/** @brief Devolve a área usada para desenhar e clicar em uma carta do tabuleiro. */
static Rectangle retangulo_carta(int posicao)
{
    return (Rectangle){248 + (posicao % 3) * 160, 140 + (posicao / 3) * 132, 144, 108};
}

bool atualizar_tela_burro(MinigameBurro *burro)
{
    if (burro->resultado != 0)
        return botao_clicado(voltar_burro);

    if (burro->segunda_carta != -1) {
        atualizarMinigameBurro(burro, GetFrameTime());
        return false;
    }

    for (int i = 0; i < 6; i++) {
        if (botao_clicado(retangulo_carta(i))) {
            selecionar_Carta_Burro(burro, i);
            break;
        }
    }
    return false;
}

void desenhar_burro(const MinigameBurro *burro, Font fonte, Font fonte_titulo, int tamanho_corpo, int tamanho_titulo)
{
    const Rectangle tabuleiro = {224, 120, 512, 332};
    DrawTextEx(fonte_titulo, "Trote Certeiro", (Vector2){224, 16}, tamanho_titulo, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, "Vire duas cartas e encontre os três pares.", (Vector2){224, 80}, tamanho_corpo, 1, COR_TEXTO_CLARO);
    desenhar_painel(tabuleiro, COR_ESCURO);

    bool pode_virar = burro->resultado == 0 && burro->segunda_carta == -1;
    for (int i = 0; i < 6; i++) {
        if (burro->encontradas[i])
            continue;

        Rectangle area = retangulo_carta(i);
        bool revelada = i == burro->primeira_carta || i == burro->segunda_carta;
        EstiloBotao estilo = revelada ? BOTAO_SELECIONADO : BOTAO_SECUNDARIO;
        desenhar_botao_com_estado(fonte, area, "", estilo, pode_virar && !revelada);
        desenhar_arte_carta_burro(area, revelada ? burro->cartas[i] : -1);
    }

    const int tamanho_placar = 20;
    DrawTextEx(fonte, TextFormat("Pares: %d/3", burro->pares_encontrados), (Vector2){248, 410}, tamanho_placar, 1, COR_TEXTO_CLARO);
    const char *chances = TextFormat("Erros restantes: %d", burro->chances);
    float largura_chances = MeasureTextEx(fonte, chances, tamanho_placar, 1).x;
    DrawTextEx(fonte, chances, (Vector2){712 - largura_chances, 410}, tamanho_placar, 1, COR_TEXTO_CLARO);

    if (burro->resultado != 0) {
        const char *mensagem = burro->resultado == 1 ? "O Burro tem um palpite!" : "Você ficou sem chances.";
        Color cor = burro->resultado == 1 ? COR_BORDA_ACERTO : COR_BORDA_ERRO;
        DrawTextEx(fonte, mensagem, (Vector2){224, 490}, tamanho_corpo, 1, cor);
        desenhar_botao(fonte, voltar_burro, "Voltar à pergunta");
    }
}
