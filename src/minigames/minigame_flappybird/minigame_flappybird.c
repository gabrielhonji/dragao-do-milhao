/**
 * @file minigame_flappybird.c
 * @author Enzo Andreassa Pizza (enzoandreassa@alunos.utfpr.edu.br)
 * @brief Atualiza o voo, as colisões e a pontuação do Eco da Torre.
 */

#include "minigame_flappybird.h"
#include <stdlib.h>

void iniciarMinigame_flappybird(FlappyBird *jogo)
{
    *jogo = (FlappyBird){
        .posx_pombo = ECO_LARGURA / 5.0f,
        .posy_pombo = ECO_ALTURA / 2.0f,
        .cano = {.pos_x = ECO_LARGURA, .abertura_y = ECO_ALTURA / 2.0f},
    };
}

void atualizaMinigame_flappybird(FlappyBird *jogo, float tempo_quadro, bool bateu_asas)
{
    if (jogo->resultado != 0 || (!jogo->iniciou && !bateu_asas))
        return;
    jogo->iniciou = true;

    // Y cresce para baixo. Bater asas aplica uma velocidade negativa, e a gravidade puxa de volta.
    jogo->velocidade_pombo += 530.0f * tempo_quadro;
    if (bateu_asas)
        jogo->velocidade_pombo = -200.0f;
    jogo->posy_pombo += jogo->velocidade_pombo * tempo_quadro;

    if (jogo->posy_pombo <= ECO_RAIO_POMBA || jogo->posy_pombo >= ECO_ALTURA - ECO_RAIO_POMBA) {
        // Mantém o sprite visível no ponto do impacto e encerra antes de contar pontos.
        jogo->posy_pombo = jogo->posy_pombo <= ECO_RAIO_POMBA ? ECO_RAIO_POMBA : ECO_ALTURA - ECO_RAIO_POMBA;
        jogo->resultado = -1;
        return;
    }

    Obstaculo *cano = &jogo->cano;
    cano->pos_x -= 134.0f * tempo_quadro;
    float fim_cano = cano->pos_x + ECO_LARGURA_OBSTACULO;
    float esquerda_pombo = jogo->posx_pombo - ECO_RAIO_POMBA;
    float direita_pombo = jogo->posx_pombo + ECO_RAIO_POMBA;
    const float meio_vao = ECO_TAMANHO_ABERTURA / 2.0f;

    // Só verifica o vão quando o obstáculo e a pomba ocupam a mesma faixa horizontal.
    if (direita_pombo >= cano->pos_x && esquerda_pombo <= fim_cano &&
        (jogo->posy_pombo - ECO_RAIO_POMBA < cano->abertura_y - meio_vao ||
         jogo->posy_pombo + ECO_RAIO_POMBA > cano->abertura_y + meio_vao))
    {
        jogo->resultado = -1;
        return;
    }

    // O ponto só entra quando a pomba inteira já passou pelo obstáculo.
    if (!cano->contou_ponto && esquerda_pombo > fim_cano) {
        cano->contou_ponto = true;
        jogo->pontuacao++;
        if (jogo->pontuacao == ECO_META)
            jogo->resultado = 1;
    }

    if (fim_cano <= 0) {
        // Deixa 36 px de margem entre o vão e as bordas do campo.
        const int margem = 36 + ECO_TAMANHO_ABERTURA / 2;
        int abertura_y = margem + rand() % (ECO_ALTURA - 2 * margem + 1);
        *cano = (Obstaculo){.pos_x = ECO_LARGURA, .abertura_y = abertura_y};
    }
}
