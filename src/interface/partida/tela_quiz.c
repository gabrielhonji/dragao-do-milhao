/**
 * @file tela_quiz.c
 * @brief Atualiza e desenha as perguntas da Jornada e da Batalha.
 */

#include <string.h>
#include "tela_quiz.h"
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

enum
{
    QUESTOES_JORNADA = 8,
};

static const char *const nomes_ajuda[TOTAL_AJUDAS] = {"Burro", "Dragão", "Princesa"};

/**
 * @brief Desenha um enunciado de uma linha, quebrando-o nos espaços.
 * @param fonte Fonte usada para medir e desenhar.
 * @param area Área que fornece a origem e a largura do texto.
 * @param pergunta Pergunta cujo enunciado será exibido.
 */
static void desenhar_enunciado(Font fonte, Rectangle area, const Pergunta *pergunta)
{
    const float tamanho = 20.0f;
    const float espacamento = 1.0f;
    char texto[sizeof pergunta->enunciado];
    strcpy(texto, pergunta->enunciado);

    // Vai gerando palavra por palavra até que bata com o limite de width, qnd acontece ele substitiu o ' ' por '\n'.
    char *inicio_linha = texto;
    char *ultimo_espaco = NULL;
    for (char *fim_palavra = texto;; fim_palavra++) {
        char separador = *fim_palavra;
        if (separador != ' ' && separador != '\0')
            continue;

        // MeasureTextEx lê até '\0'. Coloca o separador de volta depois da medida.
        *fim_palavra = '\0';
        float largura = MeasureTextEx(fonte, inicio_linha, tamanho, espacamento).x;
        *fim_palavra = separador;

        if (largura > area.width && ultimo_espaco != NULL) {
            *ultimo_espaco = '\n';
            inicio_linha = ultimo_espaco + 1;
        }

        if (separador == '\0')
            break;
        ultimo_espaco = fim_palavra;
    }

    DrawTextEx(fonte, texto, (Vector2){area.x, area.y}, tamanho, espacamento, BLACK);
}

/**
 * @brief Verifica se uma ajuda ainda pode ser usada nesta pergunta.
 * @param ajuda Índice do botão de ajuda.
 * @param usos_restantes Usos da ajuda disponíveis na partida.
 * @param dicas Efeitos e usos registrados na pergunta atual.
 * @param resposta_confirmada Indica se o jogador já confirmou uma alternativa.
 * @return Verdadeiro quando o botão deve aceitar cliques.
 */
static bool ajuda_disponivel(int ajuda, int usos_restantes, const DicasPergunta *dicas, bool resposta_confirmada)
{
    return usos_restantes > 0 && !resposta_confirmada && !dicas->resposta_revelada &&
           (ajuda != AJUDA_BURRO || !dicas->burro_usado);
}

/**
 * @brief Aplica o efeito de uma ajuda na pergunta atual.
 *
 * @param ajuda Índice do botão de ajuda.
 * @param pergunta Pergunta cujo gabarito orienta o efeito.
 * @param dicas Efeitos acumulados na pergunta atual.
 */
static void aplicar_ajuda(int ajuda, const Pergunta *pergunta, DicasPergunta *dicas)
{
    // A Princesa revela o gabarito. O retorno encerra a ajuda antes do sorteio.
    if (ajuda == AJUDA_PRINCESA) {
        dicas->resposta_revelada = true;
        return;
    }

    int correta = pergunta->resposta - 'A'; // Índice do gabarito entre 0 e 3.
    int incorretas[4];                      // Índices das alternativas erradas ainda disponíveis.
    int total_incorretas = 0;               // Quantidade guardada e posição livre para o próximo índice.
    for (int i = 0; i < 4; i++) {
        if (i != correta && !dicas->eliminadas[i]) {
            // Guarda o índice encontrado e avança para a próxima posição da lista.
            incorretas[total_incorretas] = i;
            total_incorretas++;
        }
    }

    // Sem candidatas, o sorteio teria um intervalo de 0 a -1.
    if (total_incorretas == 0)
        return;

    // Os valores de 1 a 70 representam os 70% de chance de o Burro acertar.
    if (ajuda == AJUDA_BURRO && GetRandomValue(1, 100) <= 70) {
        dicas->palpite_burro = pergunta->resposta;
        return;
    }
    int sorteada = incorretas[GetRandomValue(0, total_incorretas - 1)]; // Índice do palpite errado ou da opção que o Dragão deixa.
    if (ajuda == AJUDA_BURRO) {
        dicas->palpite_burro = 'A' + sorteada;
        return;
    }

    // Ajuda do Dragão deixa a correta e uma incorreta.
    for (int i = 0; i < 4; i++) {
        // Marca true nas opções diferentes da correta e da incorreta sorteada.
        dicas->eliminadas[i] = (i != correta) && (i != sorteada);
    }
    // Limpa o palpite do Burro se o Dragão apagou a alternativa que ele indicou.
    if (dicas->palpite_burro != '\0' && dicas->eliminadas[dicas->palpite_burro - 'A'])
        dicas->palpite_burro = '\0';
}


LayoutQuiz montar_layout_quiz(Rectangle mapa, Rectangle painel, Rectangle voltar)
{
    LayoutQuiz layout = {.mapa = mapa, .painel = painel, .voltar = voltar};
    layout.acao_quiz = (Rectangle){painel.x, voltar.y, 200, voltar.height};
    layout.y_ajudas = mapa.y + mapa.height + 5; // Espaço entre a cena e os botões de ajuda.
    const float inicio_ajudas = mapa.x + 16; // Recuo lateral de 16 px.
    for (int i = 0; i < TOTAL_AJUDAS; i++)
        layout.ajuda_rec[i] = (Rectangle){inicio_ajudas + i * 156, layout.y_ajudas, 148, 44}; // 148 px de largura e 8 px de intervalo.

    const float margem = 16.0f; // Recuo lateral dos painéis e superior do enunciado.
    const float altura_enunciado = 76.0f;
    const float espaco_apos_enunciado = 12.0f;
    const float altura_alternativa = 44.0f;
    const float espaco_entre_alternativas = 8.0f;
    layout.enunciado_quiz = (Rectangle){painel.x + margem, painel.y + margem, painel.width - 2.0f * margem, altura_enunciado};

    float y_alternativa = layout.enunciado_quiz.y + layout.enunciado_quiz.height + espaco_apos_enunciado;
    for (int i = 0; i < 4; i++) {
        layout.alternativa_rec[i] =
            (Rectangle){layout.enunciado_quiz.x, y_alternativa, layout.enunciado_quiz.width, altura_alternativa};
        y_alternativa += altura_alternativa + espaco_entre_alternativas;
    }

    return layout;
}

bool iniciar_quiz(TelaQuiz *tela, const Banco_perguntas *banco)
{
    inicializa_partida(&tela->partida);
    tela->pergunta_atual = sorteia_pergunta(banco, &tela->partida);
    tela->faltam_perguntas = tela->pergunta_atual == NULL;
    if (tela->faltam_perguntas)
        return false;

    tela->questao_exibida = tela->partida.questao_atual;
    tela->usos_ajuda[AJUDA_BURRO] = 2;
    tela->usos_ajuda[AJUDA_DRAGAO] = 1;
    tela->usos_ajuda[AJUDA_PRINCESA] = 1;
    tela->dicas = (DicasPergunta){0};
    tela->alternativa_selecionada = -1;
    tela->resposta_confirmada = false;
    return true;
}

AcaoQuiz atualizar_quiz(TelaQuiz *tela, const LayoutQuiz *layout, const Banco_perguntas *banco, bool confirmou)
{
    int alternativa_clicada = -1;                               // Índice clicado neste quadro, ou -1 sem clique válido.
    int ajuda_clicada = -1;                                     // Índice da ajuda clicada neste quadro, ou -1.

    if (!tela->resposta_confirmada) {
        // Acumula a duração do último quadro enquanto a pergunta está aberta.
        if (tela->partida.situacao == andamento && tela->pergunta_atual && !tela->faltam_perguntas)
            tela->partida.tempo_ativo += GetFrameTime();
        // Só registra um clique em alternativa que ainda pode ser escolhida.
        for (int i = 0; i < 4; i++) {
            if (!tela->dicas.eliminadas[i] && botao_clicado(layout->alternativa_rec[i])) {
                alternativa_clicada = i;
                break;
            }
        }
        // Só registra um clique em ajuda que ainda tem usos disponíveis.
        for (int i = 0; i < TOTAL_AJUDAS; i++) {
            if (ajuda_disponivel(i, tela->usos_ajuda[i], &tela->dicas, tela->resposta_confirmada) && botao_clicado(layout->ajuda_rec[i])) {
                ajuda_clicada = i;
                break;
            }
        }
    }

    // Compara o clique com a seleção que já existia antes dele.
    bool repetiu_alternativa = alternativa_clicada >= 0 && alternativa_clicada == tela->alternativa_selecionada;


    if (botao_clicado(layout->voltar)) {
        if (tela->faltam_perguntas)
            return QUIZ_VOLTAR_MENU;
        if (!tela->resposta_confirmada)
            return QUIZ_PEDIR_RETIRADA;
        if (tela->partida.situacao == derrota || tela->partida.situacao == vitoria)
            return QUIZ_MOSTRAR_RESULTADO;
        return QUIZ_SEM_ACAO;
    }
    // Consome o uso assim que o jogador pede a ajuda.
    else if (ajuda_clicada >= 0) {
        tela->usos_ajuda[ajuda_clicada]--;
        if (ajuda_clicada == AJUDA_DRAGAO)
            return QUIZ_ABRIR_SOPRO;

        if (ajuda_clicada == AJUDA_BURRO) {
            tela->dicas.burro_usado = true;
            return QUIZ_ABRIR_BURRO;
        }

        aplicar_efeito_ajuda_quiz(tela, ajuda_clicada);
    }
    // Enter, o botão de ação ou repetir o clique na alternativa confirmam a escolha.
    else if (!tela->faltam_perguntas && tela->alternativa_selecionada >= 0 &&
             (confirmou || botao_clicado(layout->acao_quiz) || repetiu_alternativa)) {
        if (!tela->resposta_confirmada) {
            char resposta = 'A' + tela->alternativa_selecionada;
            // O quiz aplica as regras da partida. A tela guarda o resultado para dar o retorno ao jogador.
            tela->resultado_resposta = processa_resposta(&tela->partida, tela->pergunta_atual, resposta);
            tela->resposta_confirmada = true;
        }
        // Acertos, marcos e renascimentos deixam a partida em andamento.
        else if (tela->partida.situacao == andamento) {
            const Pergunta *proxima = sorteia_pergunta(banco, &tela->partida);
            if (proxima == NULL)
                tela->faltam_perguntas = true;
            else {
                // Troca a pergunta só depois de um sorteio bem sucedido.
                tela->pergunta_atual = proxima;
                tela->questao_exibida = tela->partida.questao_atual;
                tela->alternativa_selecionada = -1;
                tela->resposta_confirmada = false;
                // Efeitos somem ao trocar a pergunta. Os usos restantes resetam por partida.
                tela->dicas = (DicasPergunta){0};
            }
        }
    }
    // Um clique válido muda a escolha. Nos demais quadros, a seleção continua guardada.
    else if (alternativa_clicada >= 0) {
        tela->alternativa_selecionada = alternativa_clicada;
    }

    return QUIZ_SEM_ACAO;
}

void aplicar_efeito_ajuda_quiz(TelaQuiz *tela, int ajuda)
{
    aplicar_ajuda(ajuda, tela->pergunta_atual, &tela->dicas);

    // Se a ajuda apagou a escolha atual, o jogador precisa escolher outra.
    if (tela->alternativa_selecionada >= 0 && tela->dicas.eliminadas[tela->alternativa_selecionada])
        tela->alternativa_selecionada = -1;
}

bool quiz_em_batalha(const TelaQuiz *tela)
{
    // A questão 8 ainda mostra a Jornada. A troca acontece quando a questão 9 aparece.
    return tela->questao_exibida > QUESTOES_JORNADA;
}

void desenhar_quiz(const TelaQuiz *tela, const LayoutQuiz *layout, Font fonte, int tamanho_corpo)
{
    const int total_questoes = 15;
    bool em_batalha = quiz_em_batalha(tela);
    bool renascendo = tela->resposta_confirmada && tela->resultado_resposta == resposta_renascimento;

    const char *nome_etapa = em_batalha ? "Batalha" : "Jornada";
    const char *titulo_quiz = TextFormat("%s - Questão %d de %d", nome_etapa, tela->questao_exibida, total_questoes);
    DrawTextEx(fonte, titulo_quiz, (Vector2){24, 24}, tamanho_corpo, 1, BLACK);

    // Alinha o contador de ovos à direita da cena, na mesma linha do título.
    const char *ovos = TextFormat("Ovos: %d", tela->partida.ovos_restantes);
    float largura_ovos = MeasureTextEx(fonte, ovos, 20, 1).x;
    DrawTextEx(fonte, ovos, (Vector2){layout->mapa.x + layout->mapa.width - largura_ovos, 24}, 20, 1, BLACK);

    CenaQuiz cena = em_batalha ? CENA_BATALHA : CENA_JORNADA;
    if (tela->partida.situacao == derrota)
        cena = em_batalha ? CENA_DERROTA : CENA_DERROTA_JORNADA; // Exibe o dragão caído no castelo ou na jornada dependendo até onde ele chegou.
    else if (tela->partida.situacao == vitoria)
        cena = CENA_VITORIA;

    desenhar_painel(layout->mapa, COR_ESCURO);
    desenhar_arte_quiz(cena);

    const char *titulo_cenario = em_batalha ? "Castelo do príncipe" : "Mapa aéreo";
    if (cena == CENA_DERROTA_JORNADA)
        titulo_cenario = "Estrada para o castelo";
    DrawTextEx(fonte, titulo_cenario, (Vector2){layout->painel.x, 24}, tamanho_corpo, 1, BLACK);

    const Vector2 posicao_progresso = {layout->mapa.x, layout->mapa.y - 39}; // Rótulo na segunda linha acima da cena.
    if (renascendo) {
        // Enquanto a pergunta anterior aparece, o progresso da partida já está no marco.
        const char *marco = TextFormat("Marco: %d", tela->partida.checkpoint);
        DrawTextEx(fonte, marco, posicao_progresso, 20, 1, BLACK);
    }
    else {
        int valor = em_batalha ? calcula_pv_principe(&tela->partida) : tela->partida.acertos;
        int limite = em_batalha ? PV_MAX_PRINCIPE : QUESTOES_JORNADA;
        const char *progresso = TextFormat("%s: %d/%d", em_batalha ? "PV" : "Etapas", valor, limite);
        DrawTextEx(fonte, progresso, posicao_progresso, 20, 1, BLACK);

        // A fração do valor que resta define a largura colorida da barra.
        const float espaco_progresso = 164;                                                                 // Reserva a coluna do rótulo à esquerda da barra.
        Rectangle fundo_vida = {layout->mapa.x + espaco_progresso, layout->mapa.y - 33, layout->mapa.width - espaco_progresso, 16}; // Centraliza a barra no rótulo.
        Rectangle preenchimento_vida = fundo_vida;
        preenchimento_vida.width *= (float)valor / limite;

        DrawRectangleRec(fundo_vida, RAYWHITE);
        DrawRectangleRec(preenchimento_vida, em_batalha ? MAROON : DARKGREEN);
        DrawRectangleLinesEx(fundo_vida, 2, DARKGRAY);
    }

    // Mostra os usos restantes e apaga os botões indisponíveis.
    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        const char *rotulo = TextFormat("%s (%d)", nomes_ajuda[i], tela->usos_ajuda[i]);
        bool habilitado = ajuda_disponivel(i, tela->usos_ajuda[i], &tela->dicas, tela->resposta_confirmada);
        desenhar_botao_com_estado(fonte, layout->ajuda_rec[i], rotulo, BOTAO_SECUNDARIO, habilitado);
    }

    // Mostra a revelação da Princesa quando ativa. Caso contrário, mostra o palpite do Burro.
    const char *texto_dica = NULL;
    if (tela->dicas.resposta_revelada)
        texto_dica = TextFormat("Princesa: resposta %c.", tela->pergunta_atual->resposta);
    else if (tela->dicas.palpite_burro != '\0')
        texto_dica = TextFormat("Burro: apostaria na %c.", tela->dicas.palpite_burro);
    if (texto_dica != NULL) {
        Vector2 posicao_dica = {layout->painel.x, layout->y_ajudas}; // Dica na mesma altura das ajudas, à direita da cena.
        DrawTextEx(fonte, texto_dica, posicao_dica, 20, 1, BLACK);
    }

    // Desenha a caixa do quiz e o enunciado da questão.
    DrawRectangleRec(layout->painel, LIGHTGRAY);
    desenhar_enunciado(fonte, layout->enunciado_quiz, tela->pergunta_atual);

    // Define os rótulos das alternativas e as desenha.
    const char *rotulos[] = {tela->pergunta_atual->alternativa.A, tela->pergunta_atual->alternativa.B,
                             tela->pergunta_atual->alternativa.C, tela->pergunta_atual->alternativa.D};
    for (int i = 0; i < 4; i++) {
        // Mantém o texto da alternativa para o jogador ver o que o Dragão apagou.
        char letra = 'A' + i;
        bool selecionado = i == tela->alternativa_selecionada;
        bool habilitado = !tela->dicas.eliminadas[i] && !tela->resposta_confirmada;
        desenhar_alternativa(fonte, layout->alternativa_rec[i], rotulos[i], selecionado, habilitado);

        // Desenha a marca da ajuda por cima do cartão e preserva a cor da seleção.
        if (tela->dicas.resposta_revelada && letra == tela->pergunta_atual->resposta)
            DrawRectangleLinesEx(layout->alternativa_rec[i], 4.0f, DARKGREEN);
        else if (!tela->dicas.resposta_revelada && letra == tela->dicas.palpite_burro)
            DrawRectangleLinesEx(layout->alternativa_rec[i], 4.0f, BROWN);
    }

    const char *mensagem = NULL; // Porque antes de selecionar não há mensagem para exibir.
    if (tela->faltam_perguntas)
        mensagem = "Sem perguntas para avançar.";
    // Define a mensagem dependendo se errou, acertou ou ganhou;
    else if (tela->resposta_confirmada) {
        if (renascendo)
            mensagem = TextFormat("Ovo usado. Próxima: questão %d.", tela->partida.checkpoint + 1);
        else if (tela->resultado_resposta == resposta_marco)
            mensagem = "Marco alcançado! Você ganhou um ovo.";
        else if (tela->resultado_resposta == resposta_derrota)
            mensagem = "Resposta incorreta.";
        else if (tela->resultado_resposta == resposta_vitoria)
            mensagem = "Vitória! A princesa foi salva.";
        else
            mensagem = "Resposta correta!";
    }
    // Alternativa selecionada antes de confirmar
    else if (tela->alternativa_selecionada >= 0)
        mensagem = TextFormat("Alternativa '%c' selecionada.", 'A' + tela->alternativa_selecionada); // Em C, aspas simples representa um caractere e aspas duplas representa uma string, por isso deve ser 'A', e não "A".
    if (mensagem != NULL) {
        Vector2 posicao = {layout->painel.x, layout->mapa.y - 39}; // Mensagem abaixo do título e acima da caixa de perguntas.
        DrawTextEx(fonte, mensagem, posicao, 20, 1.0f, BLACK);
    }

    if (tela->alternativa_selecionada >= 0 && !tela->faltam_perguntas && tela->partida.situacao == andamento) {
        const char *rotulo = tela->resposta_confirmada ? "Continuar" : "Confirmar";
        desenhar_botao(fonte, layout->acao_quiz, rotulo);
    }

    if (tela->faltam_perguntas)
        desenhar_botao(fonte, layout->voltar, "Voltar");
    else if (tela->partida.situacao == vitoria)
        desenhar_botao(fonte, layout->voltar, "Registrar nome");
    else if (tela->partida.situacao == derrota)
        desenhar_botao(fonte, layout->voltar, "Ver gruta");
    else
        // O !resposta_confimada significa que o botão desabilita após o usuário confirmar uma alternativa.
        desenhar_botao_com_estado(fonte, layout->voltar, "Retirada Tática", BOTAO_SECUNDARIO, !tela->resposta_confirmada);
}
