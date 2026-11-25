/**
 * @file tela_quiz.c
 * @brief Atualiza e desenha as perguntas da Jornada e da Batalha.
 */

#include "tela_quiz.h"
#include "../comum/arte_cenas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

enum
{
    QUESTOES_JORNADA = 8,
};

static const char *const nomes_ajuda[TOTAL_AJUDAS] = {"Burro", "Dragão", "Princesa"};
static const IconeQuiz icones_ajuda[TOTAL_AJUDAS] = {ICONE_BURRO, ICONE_DRAGAO, ICONE_PRINCESA};
static const Color cores_ajuda[TOTAL_AJUDAS] = {COR_DICA_BURRO, COR_DICA_FOGO, COR_DICA_CARTA};
static const float duracao_efeito = 0.60f;

/** @brief Mantém o movimento ou impacto visível antes de liberar a próxima ação. */
static bool efeito_em_andamento(const TelaQuiz *tela)
{
    return tela->resposta_confirmada && tela->tempo_efeito < duracao_efeito;
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


LayoutQuiz montar_layout_quiz(Rectangle mapa, Rectangle painel, Rectangle rodape)
{
    LayoutQuiz layout = {.mapa = mapa, .painel = painel};
    const float intervalo_rodape = 12;
    const float largura_botao = (painel.width - intervalo_rodape) / 2;
    // Os dois botões dividem o rodapé do painel de perguntas.
    layout.retirada = (Rectangle){painel.x, rodape.y, largura_botao, rodape.height};
    layout.acao_quiz = (Rectangle){painel.x + largura_botao + intervalo_rodape, rodape.y, largura_botao, rodape.height};
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
    bool clicou_acao = botao_clicado(layout->acao_quiz);
    if (tela->faltam_perguntas)
        return clicou_acao || confirmou ? QUIZ_VOLTAR_MENU : QUIZ_SEM_ACAO;

    if (efeito_em_andamento(tela))
        tela->tempo_efeito += GetFrameTime();

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


    if (!tela->resposta_confirmada && botao_clicado(layout->retirada))
        return QUIZ_PEDIR_RETIRADA;
    // Consome o uso assim que o jogador pede a ajuda.
    else if (ajuda_clicada >= 0) {
        tela->usos_ajuda[ajuda_clicada]--;
        if (ajuda_clicada == AJUDA_DRAGAO)
            return QUIZ_ABRIR_SOPRO;

        if (ajuda_clicada == AJUDA_BURRO) {
            tela->dicas.burro_usado = true;
            return QUIZ_ABRIR_BURRO;
        }

        return QUIZ_ABRIR_ECO; // A Princesa revela a resposta depois da vitória.
    }
    // Enter, o botão de ação ou repetir o clique na alternativa confirmam a escolha.
    else if (tela->alternativa_selecionada >= 0 && (confirmou || clicou_acao || repetiu_alternativa)) {
        if (!tela->resposta_confirmada) {
            char resposta = 'A' + tela->alternativa_selecionada;
            int pv_antes = calcula_pv_principe(&tela->partida);
            // O quiz aplica as regras da partida. A tela guarda o resultado para dar o retorno ao jogador.
            tela->resultado_resposta = processa_resposta(&tela->partida, tela->pergunta_atual, resposta);
            tela->resposta_confirmada = true;
            bool acertou = resposta == tela->pergunta_atual->resposta;
            tela->tempo_efeito = acertou ? 0 : duracao_efeito;
            tela->dano_batalha = acertou && quiz_em_batalha(tela) ? pv_antes - calcula_pv_principe(&tela->partida) : 0;
        }
        else if (efeito_em_andamento(tela))
            return QUIZ_SEM_ACAO;
        else if (tela->partida.situacao != andamento)
            return QUIZ_MOSTRAR_RESULTADO;
        // Acertos, marcos e renascimentos deixam a partida em andamento.
        else {
            bool estava_na_jornada = !quiz_em_batalha(tela);
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
                if (estava_na_jornada && quiz_em_batalha(tela))
                    return QUIZ_CHEGOU_CASTELO;
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

/** @brief Combina o ícone, o contador e o estado de disponibilidade de cada ajuda. */
static void desenhar_ajudas(const TelaQuiz *tela, const LayoutQuiz *layout, Font fonte)
{
    for (int i = 0; i < TOTAL_AJUDAS; i++) {
        Rectangle area = layout->ajuda_rec[i];
        bool habilitado = ajuda_disponivel(i, tela->usos_ajuda[i], &tela->dicas, tela->resposta_confirmada);
        desenhar_botao_com_estado(fonte, area, "", BOTAO_SECUNDARIO, habilitado);

        // A coluna de 36 px à esquerda é só para o ícone.
        desenhar_icone_quiz(icones_ajuda[i], (Rectangle){area.x + 4, area.y, 32, area.height}, Fade(WHITE, habilitado ? 1.0f : 0.45f));
        const char *rotulo = TextFormat("%s (%d)", nomes_ajuda[i], tela->usos_ajuda[i]);
        Rectangle texto = {area.x + 38, area.y + 4, area.width - 46, area.height - 8}; // Recuo do ícone e margem direita.
        desenhar_texto_ajustado(fonte, texto, rotulo, 18, habilitado ? cores_ajuda[i] : COR_TEXTO_BLOQUEADO, true);
    }
}

/** @brief Desenha um ícone por ovo disponível, a partir da borda direita da cena. */
static void desenhar_ovos(const TelaQuiz *tela, Rectangle mapa)
{
    const float passo = 25; // Ícone de 16 px em uma coluna de 20 px, seguida de um intervalo de 5 px.
    for (int i = 0; i < tela->partida.ovos_restantes; i++) {
        float x = mapa.x + mapa.width - passo * (i + 1);
        desenhar_icone_quiz(ICONE_OVO, (Rectangle){x, 24, 20, 24}, WHITE);
    }
}

/** @brief Distingue a escolha, o gabarito, o erro confirmado e as opções eliminadas. */
static void desenhar_respostas(const TelaQuiz *tela, const LayoutQuiz *layout, Font fonte)
{
    const Alternativa *alternativa = &tela->pergunta_atual->alternativa;
    const char *rotulos[] = {alternativa->A, alternativa->B, alternativa->C, alternativa->D};
    for (int i = 0; i < 4; i++) {
        char letra = 'A' + i;
        bool correta = letra == tela->pergunta_atual->resposta;
        bool selecionada = i == tela->alternativa_selecionada;
        bool eliminada = tela->dicas.eliminadas[i];
        bool habilitada = !eliminada && !tela->resposta_confirmada;
        EstiloBotao estilo = BOTAO_SECUNDARIO;
        int ajuda = -1;
        char marca = eliminada ? '-' : '.';

        if (eliminada) {
            ajuda = AJUDA_DRAGAO;
            estilo = BOTAO_DICA_FOGO;
        }
        else if (tela->dicas.resposta_revelada && correta) {
            ajuda = AJUDA_PRINCESA;
            estilo = BOTAO_DICA_CARTA;
        }
        else if (!tela->dicas.resposta_revelada && letra == tela->dicas.palpite_burro) {
            ajuda = AJUDA_BURRO;
            estilo = BOTAO_DICA_BURRO;
        }

        // A borda azul continua indicando a seleção. O selo fica visível junto do resultado.
        if (selecionada)
            estilo = BOTAO_SELECIONADO;
        if (tela->resposta_confirmada && correta) {
            estilo = BOTAO_ACERTO;
            marca = '+';
        }
        else if (tela->resposta_confirmada && selecionada) {
            estilo = BOTAO_ERRO;
            marca = '!';
        }
        Rectangle area = layout->alternativa_rec[i];
        const float largura_selo = ajuda >= 0 ? 36 : 0;
        desenhar_alternativa(fonte, area, TextFormat("%c%c %s", letra, marca, rotulos[i]), estilo, habilitada, largura_selo);

        if (ajuda >= 0) {
            // A ficha ocupa o lado direito. A faixa colorida deixa cada ajuda visível mesmo sem ler o texto.
            Rectangle selo = {area.x + area.width - largura_selo - 6, area.y + 7, largura_selo, area.height - 14};
            DrawRectangleRec(selo, COR_ESCURO);
            DrawRectangleRec((Rectangle){selo.x, selo.y, 2, selo.height}, cores_ajuda[ajuda]);
            desenhar_icone_quiz(icones_ajuda[ajuda], selo, WHITE);
        }
    }
}

void desenhar_quiz(const TelaQuiz *tela, const LayoutQuiz *layout, Font fonte, int tamanho_corpo)
{
    const int total_questoes = 15;
    bool em_batalha = quiz_em_batalha(tela);
    bool renascendo = tela->resposta_confirmada && tela->resultado_resposta == resposta_renascimento;
    bool em_transicao = efeito_em_andamento(tela);
    float avanco = em_transicao ? tela->tempo_efeito / duracao_efeito : 1.0f;

    const char *nome_etapa = em_batalha ? "Batalha" : "Jornada";
    const char *titulo_quiz = TextFormat("%s - Questão %d de %d", nome_etapa, tela->questao_exibida, total_questoes);
    DrawTextEx(fonte, titulo_quiz, (Vector2){24, 24}, tamanho_corpo, 1, COR_TEXTO_CLARO);

    desenhar_ovos(tela, layout->mapa);

    bool despertado = tela->partida.acertos >= 13;
    CenaQuiz cena = em_batalha ? (despertado ? CENA_DESPERTAR : CENA_BATALHA) : CENA_JORNADA;
    if (renascendo)
        cena = CENA_RENASCIMENTO;
    else if (tela->partida.situacao == derrota)
        cena = em_batalha ? CENA_DERROTA : CENA_DERROTA_JORNADA; // Exibe o dragão caído no castelo ou na jornada dependendo até onde ele chegou.
    else if (tela->partida.situacao == vitoria && !em_transicao)
        cena = CENA_VITORIA;

    desenhar_painel(layout->mapa, COR_ESCURO);
    // O motor já contou o acerto. Subtrair o trecho ainda por voar posiciona o dragão entre os dois pontos.
    float passos = tela->partida.acertos - (1.0f - avanco);
    desenhar_arte_quiz(cena, passos);
    if (em_batalha && em_transicao) {
        desenhar_impacto_batalha(avanco, despertado);
        Vector2 impacto = {layout->mapa.x + 310, layout->mapa.y + 144 - 28 * avanco}; // Sobe a partir do príncipe.
        DrawTextEx(fonte, TextFormat("-%d PV", tela->dano_batalha), impacto, 24, 1, Fade(COR_BORDA_ERRO, 1.0f - avanco * 0.55f));
    }
    if (tela->resposta_confirmada && tela->resultado_resposta == resposta_marco && !em_transicao) {
        Rectangle interior = {layout->mapa.x + 4, layout->mapa.y + 4, layout->mapa.width - 8, layout->mapa.height - 9};
        desenhar_ovo_marco(interior); // Desconta a moldura para escurecer só a cena.
    }

    const char *titulo_cenario = em_batalha ? "Castelo do príncipe" : "Mapa aéreo";
    if (renascendo)
        titulo_cenario = "Gruta do renascimento";
    else if (cena == CENA_DERROTA_JORNADA)
        titulo_cenario = "Estrada para o castelo";
    DrawTextEx(fonte, titulo_cenario, (Vector2){layout->painel.x, 24}, tamanho_corpo, 1, COR_TEXTO_CLARO);

    const Vector2 posicao_progresso = {layout->mapa.x, layout->mapa.y - 39}; // Rótulo na segunda linha acima da cena.
    if (renascendo) {
        // Enquanto a pergunta anterior aparece, o progresso da partida já está no marco.
        const char *marco = TextFormat("Marco: %d", tela->partida.checkpoint);
        DrawTextEx(fonte, marco, posicao_progresso, 20, 1, COR_TEXTO_CLARO);
    }
    else {
        int valor = em_batalha ? calcula_pv_principe(&tela->partida) : tela->partida.acertos;
        int limite = em_batalha ? PV_MAX_PRINCIPE : QUESTOES_JORNADA;
        const char *progresso = TextFormat("%s: %d/%d", em_batalha ? "PV" : "Etapas", valor, limite);
        DrawTextEx(fonte, progresso, posicao_progresso, 20, 1, COR_TEXTO_CLARO);

        // A fração do valor que resta define a largura colorida da barra.
        const float espaco_progresso = 164;                                                                 // Reserva a coluna do rótulo à esquerda da barra.
        Rectangle fundo_vida = {layout->mapa.x + espaco_progresso, layout->mapa.y - 33, layout->mapa.width - espaco_progresso, 16}; // Centraliza a barra no rótulo.
        Rectangle preenchimento_vida = fundo_vida;
        preenchimento_vida.width *= (float)valor / limite;

        DrawRectangleRec(fundo_vida, COR_FACE_BLOQUEADA);
        DrawRectangleRec(preenchimento_vida, em_batalha ? COR_BORDA_ERRO : COR_BORDA_ACERTO);
        DrawRectangleLinesEx(fundo_vida, 2, COR_BORDA_BOTAO);
    }

    desenhar_ajudas(tela, layout, fonte);

    // Mostra a revelação da Princesa quando ativa. Caso contrário, mostra o palpite do Burro.
    const char *texto_dica = NULL;
    Color cor_dica = COR_TEXTO_CLARO;
    if (tela->dicas.resposta_revelada) {
        texto_dica = TextFormat("Princesa: resposta %c.", tela->pergunta_atual->resposta);
        cor_dica = COR_DICA_CARTA;
    }
    else if (tela->dicas.palpite_burro != '\0') {
        texto_dica = TextFormat("Burro: apostaria na %c.", tela->dicas.palpite_burro);
        cor_dica = COR_DICA_BURRO;
    }
    else if (tela->dicas.eliminadas[0] || tela->dicas.eliminadas[1] || tela->dicas.eliminadas[2] || tela->dicas.eliminadas[3]) {
        texto_dica = "Sopro: duas opções queimadas.";
        cor_dica = COR_DICA_FOGO;
    }
    if (texto_dica != NULL) {
        Rectangle area_dica = {layout->painel.x, layout->y_ajudas, layout->painel.width, 38};
        desenhar_texto_ajustado(fonte, area_dica, texto_dica, 20, cor_dica, false);
    }

    // Desenha a caixa do quiz e o enunciado da questão.
    desenhar_painel(layout->painel, COR_ESCURO);
    desenhar_texto_ajustado(fonte, layout->enunciado_quiz, tela->pergunta_atual->enunciado, 20, COR_TEXTO_CLARO, false);
    desenhar_respostas(tela, layout, fonte);

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
        else if (tela->questao_exibida == 13)
            mensagem = "Despertar Dracônico!";
        else
            mensagem = "Resposta correta!";
    }
    // Alternativa selecionada antes de confirmar
    else if (tela->alternativa_selecionada >= 0)
        mensagem = TextFormat("Alternativa '%c' selecionada.", 'A' + tela->alternativa_selecionada); // Em C, aspas simples representa um caractere e aspas duplas representa uma string, por isso deve ser 'A', e não "A".
    if (mensagem != NULL) {
        Vector2 posicao = {layout->painel.x, layout->mapa.y - 39}; // Mensagem abaixo do título e acima da caixa de perguntas.
        DrawTextEx(fonte, mensagem, posicao, 20, 1.0f, COR_TEXTO_CLARO);
    }

    const char *acao = tela->resposta_confirmada ? "Continuar" : "Confirmar";
    if (tela->faltam_perguntas)
        acao = "Voltar";
    else if (tela->partida.situacao == vitoria)
        acao = "Registrar nome";
    else if (tela->partida.situacao == derrota)
        acao = "Ver gruta";

    bool partida_em_andamento = tela->partida.situacao == andamento && !tela->faltam_perguntas;
    if (partida_em_andamento)
        desenhar_botao_com_estado(fonte, layout->retirada, "Retirada Tática", BOTAO_SAIDA, !tela->resposta_confirmada);

    bool acao_habilitada = tela->faltam_perguntas || (tela->alternativa_selecionada >= 0 && !em_transicao);
    desenhar_botao_com_estado(fonte, layout->acao_quiz, acao, BOTAO_PRIMARIO, acao_habilitada);
}
