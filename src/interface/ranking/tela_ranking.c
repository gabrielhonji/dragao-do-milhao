/**
 * @file tela_ranking.c
 * @brief Carrega e apresenta os resultados do ranking.
 */

#include <errno.h>
#include <stdlib.h>
#include "tela_ranking.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

void carregar_ranking(TelaRanking *ranking)
{
    FILE *arquivo_ranking = NULL; // O leitor recebe seu endereço e abre o arquivo.
    ranking->total = carregar_registro_partidas(&ranking->registros, &arquivo_ranking);

    // Antes do primeiro resultado salvo, o arquivo ainda não existe.
    if (ranking->total == -2 && errno == ENOENT) // A abertura falhou porque o arquivo ou caminho não existe.
        ranking->total = 0;

}

void liberar_ranking(TelaRanking *ranking)
{
    free(ranking->registros);
    ranking->registros = NULL;
}

void desenhar_ranking(const TelaRanking *ranking, Font fonte, Font fonte_titulo, Rectangle voltar)
{
    DrawTextEx(fonte_titulo, "Ranking", (Vector2){24, 24}, 34, 1, COR_TEXTO_CLARO);
    desenhar_botao(fonte, voltar, "Voltar");

    const Rectangle painel_ranking = {24, 80, 912, 348}; // Painel entre o título e o footer.
    desenhar_painel(painel_ranking, COR_ESCURO);

    const float y_cabecalho = 104;
    const float altura_linha = 36; // Distância vertical entre os registros.
    const float x_posicao = 48;
    const float x_nome = 110;
    const float x_acertos = 540;
    const float x_ovos = 670;
    const float x_tempo = 780;

    if (ranking->total < 0)
        DrawTextEx(fonte, "Não foi possível carregar o ranking.", (Vector2){x_posicao, y_cabecalho}, 18, 1, COR_AVISO);
    else if (ranking->total == 0)
        DrawTextEx(fonte, "Nenhum resultado foi registrado.", (Vector2){x_posicao, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
    else {
        // Escreve os cabeçalhos.
        DrawTextEx(fonte, "#", (Vector2){x_posicao, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
        DrawTextEx(fonte, "Nome", (Vector2){x_nome, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
        DrawTextEx(fonte, "Acertos", (Vector2){x_acertos, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
        DrawTextEx(fonte, "Ovos", (Vector2){x_ovos, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
        DrawTextEx(fonte, "Tempo", (Vector2){x_tempo, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);

        // Linhas de rankings.
        const int limite_visivel = 8; // Quantidade de linhas que cabe no painel.
        for (int i = 0; i < ranking->total && i < limite_visivel; i++) {
            const Registro_partida *registro = &ranking->registros[i];
            float y = y_cabecalho + (i + 1) * altura_linha; // A primeira linha vem depois do cabeçalho

            DrawTextEx(fonte, TextFormat("%d", i + 1), (Vector2){x_posicao, y}, 18, 1, COR_TEXTO_CLARO);
            desenhar_previa_texto(fonte, registro->nome_jogador, (Vector2){x_nome, y}, x_acertos - x_nome - 16); // Preserva 16px antes de Acertos.
            DrawTextEx(fonte, TextFormat("%d", registro->progresso), (Vector2){x_acertos, y}, 18, 1, COR_TEXTO_CLARO);
            DrawTextEx(fonte, TextFormat("%d", registro->ovos_restantes), (Vector2){x_ovos, y}, 18, 1, COR_TEXTO_CLARO);
            DrawTextEx(fonte, TextFormat("%.2f s", registro->tempo_ativo), (Vector2){x_tempo, y}, 18, 1, COR_TEXTO_CLARO);
        }
    }
}
