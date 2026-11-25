/**
 * @file tela_gruta.c
 * @brief Organiza a prévia e o resultado mostrados na gruta.
 */

#include "tela_gruta.h"
#include "componentes.h"

void abrir_previa_gruta(TelaGruta *gruta)
{
    *gruta = (TelaGruta){.estado = GRUTA_VAZIA, .previa = true};
}

void atualizar_previa_gruta(TelaGruta *gruta, bool janela_em_foco)
{
    const int teclas_gruta[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR};
    // Cada tecla escolhe o estado com o mesmo índice no enum.
    for (int i = 0; i < 4; i++) {
        if (gruta->previa && janela_em_foco && IsKeyPressed(teclas_gruta[i])) {
            gruta->estado = (EstadoGruta)i;
            break;
        }
    }

}

void preparar_gruta_retirada(TelaGruta *gruta, int marco)
{
    // O ouro corresponde ao último marco conquistado na partida.
    if (marco == 10)
        gruta->estado = GRUTA_OURO_MEDIO;
    else if (marco == 5)
        gruta->estado = GRUTA_OURO_BAIXO;
    else
        gruta->estado = GRUTA_VAZIA;
    gruta->previa = false;

}

void preparar_gruta_final(TelaGruta *gruta, Situacao_partida situacao)
{
    gruta->estado = situacao == vitoria ? GRUTA_OURO_ALTO : GRUTA_VAZIA;
    gruta->previa = false;
}

/**
 * @brief Desenha a gruta dependendo do estado atual e quantidade de ovos.
 *
 * @param fonte Fonte usada nos rótulos.
 * @param estado Define se há ouro e qual o tamanho da pilha.
 * @param ovos_restantes Quantidade de filhotes que aparecem ao lado da pilha.
 * @param mostrar_dragao Mostra o dragão vivo, mesmo que a retirada tenha ocorrido antes do primeiro marco.
 */
static void desenhar_gruta(Font fonte, EstadoGruta estado, int ovos_restantes, bool mostrar_dragao)
{
    DrawTextEx(fonte, "Gruta", (Vector2){24, 24}, 24, 1, BLACK);
    desenhar_arte_gruta(estado, ovos_restantes, mostrar_dragao);
    if (!mostrar_dragao)
        DrawTextEx(fonte, "A gruta está vazia.", (Vector2){48, 104}, 24, 1, RAYWHITE);
}


void desenhar_tela_gruta(const TelaGruta *gruta, const Partida *partida, Font fonte,
                        Rectangle voltar, float x_mensagem, int tamanho_corpo)
{
    const int ovos_teste[] = {0, 0, 1, 2}; // Filhotes de cada composição na prévia.
    int filhotes = gruta->previa ? ovos_teste[gruta->estado] : partida->ovos_restantes;
    // Uma retirada antes do primeiro marco deixa o dragão vivo no chão, sem pilha.
    bool mostrar_dragao = gruta->estado != GRUTA_VAZIA || (!gruta->previa && partida->situacao == retirada);
    desenhar_gruta(fonte, gruta->estado, filhotes, mostrar_dragao);
    desenhar_botao(fonte, voltar, "Voltar");

    const char *texto_gruta = "Prévia: teclas 1 a 4";
    if (!gruta->previa) {
        if (partida->situacao == vitoria)
            texto_gruta = "Vitória! A princesa foi salva.";
        else if (partida->situacao == derrota)
            texto_gruta = "Derrota! A gruta está vazia.";
        else if (partida->checkpoint > 0)
            texto_gruta = TextFormat("Retirada no marco %d.", partida->checkpoint);
        else
            texto_gruta = "Retirada antes do primeiro marco.";
    }
    // Divide o espaço vertical livre para alinhar o texto ao botão Voltar.
    const Vector2 posicao_texto = {x_mensagem, voltar.y + (voltar.height - tamanho_corpo) / 2.0f};
    DrawTextEx(fonte, texto_gruta, posicao_texto, tamanho_corpo, 1, BLACK);
}
