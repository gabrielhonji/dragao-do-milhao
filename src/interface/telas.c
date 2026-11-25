/**
 * @file telas.c
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Abre a janela de teste e alterna entre menu, quiz e gruta.
 * @version 0.1
 * @date 2026-11-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include <string.h>
#include <stdlib.h>
#include "telas.h"
#include "arte_cenas.h"
#include "componentes.h"
#include "cores.h"
#include "../quiz/quiz.h"
#include "../quiz/arquivo_partidas.h"
#include "tela_menu.h"
#include "tela_creditos.h"
#include "tela_ranking.h"
#include "tela_registro.h"
#include "tela_gruta.h"
#include "tela_gerenciamento.h"

typedef enum
{
    TELA_MENU,
    TELA_JORNADA,
    TELA_CONFIRMAR_RETIRADA,
    TELA_GRUTA,
    TELA_CREDITOS,
    TELA_GERENCIAMENTO,
    TELA_RANKING,
    TELA_REGISTRAR_RESULTADO,
} Tela;
enum
{
    LARGURA_BASE = 960,
    ALTURA_BASE = 540,
};

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
    if (escala >= 1.0f)
        escala = (int)escala;

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
    for (int codigo = 32; codigo <= 126; codigo++)
        caracteres[total++] = codigo;
    // Carregamento de caracteres da extensão Latin-1 (ex: 'á', 'õ', 'ç').
    for (int codigo = 160; codigo <= 255; codigo++)
        caracteres[total++] = codigo;

    Font fonte = LoadFontEx("assets/fontes/PixelifySans.ttf", tamanho, caracteres, total);
    if (fonte.texture.id != 0 && fonte.texture.id != GetFontDefault().texture.id)
        SetTextureFilter(fonte.texture, TEXTURE_FILTER_POINT);
    return fonte;
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

enum
{
    AJUDA_BURRO,
    AJUDA_DRAGAO,
    AJUDA_PRINCESA,
    TOTAL_AJUDAS,
};

typedef struct
{
    bool eliminadas[4];
    char palpite_burro;
    bool burro_usado; // O uso continua gasto se o Dragão apagar o palpite.
    bool resposta_revelada;
} DicasPergunta;

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

    // Os valores de 1 a 60 representam os 60% de chance de o Burro acertar.
    if (ajuda == AJUDA_BURRO && GetRandomValue(1, 100) <= 60) {
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

void executar_tela_teste(Banco_perguntas *banco)
{
    // Navegação e botão compartilhado pelas telas.
    Tela tela_atual = TELA_MENU;
    const Rectangle voltar = {24, 464, 200, 52};

    Dificuldade dificuldade_em_falta = 0;
    const BotoesMenu botoes_menu = montar_botoes_menu();

    // Nomes usados no menu e na lista de perguntas.
    const char *nomes_dificuldade[] = {
        "Desconhecida",
        "Muito fácil",
        "Fácil",
        "Média",
        "Difícil",
        "Muito difícil",
    };

    // Ranking e formulário de registro.
    TelaRanking tela_ranking = {0};
    TelaRegistro tela_registro = {0};

    TelaGruta tela_gruta = {.estado = GRUTA_VAZIA};

    TelaGerenciamento tela_gerenciamento = {0};

    // Painéis reaproveitados pelo quiz e pelos créditos.
    const Rectangle mapa = {24, 88, 492, 325};
    const Rectangle quiz = {544, 88, 392, 325};

    // Partida e resposta da pergunta que está na tela.
    Partida partida;
    const Pergunta *pergunta_atual = NULL;
    int questao_exibida = 0; // Mantém o número na tela até o próximo sorteio, mesmo após o renascimento.
    bool faltam_perguntas = false;
    int alternativa_selecionada = -1;
    bool resposta_confirmada = false;
    Resultado_resposta resultado_resposta = 0; // Só é consultado depois de confirmar.
    const Rectangle acao_quiz = {quiz.x, voltar.y, 200, voltar.height};
    const Rectangle salvar_ranking = acao_quiz; // Usa a mesma posição de confirmação no rodapé.

    // Ajudas da partida e seus efeitos na pergunta atual.
    // A linha de ajudas começa 5 px abaixo da borda inferior da cena.
    const float y_ajudas = mapa.y + mapa.height + 5;
    // Retângulos, nomes e usos seguem a mesma ordem dos índices das ajudas.
    const Rectangle ajuda_rec[TOTAL_AJUDAS] = {
        {mapa.x + 16, y_ajudas, 148, 44},           // Recuo lateral de 16 px.
        {mapa.x + 16 + 156, y_ajudas, 148, 44},     // Cada passo soma 148 px de largura e 8 px de espaço.
        {mapa.x + 16 + 2 * 156, y_ajudas, 148, 44}, // Dois passos a partir do mesmo recuo.
    };
    const char *nomes_ajuda[TOTAL_AJUDAS] = {"Burro", "Dragão", "Princesa"};
    int usos_ajuda[TOTAL_AJUDAS] = {0}; // Usos restantes em toda a partida.
    DicasPergunta dicas = {0};          // Efeitos que valem para a pergunta exibida.

    // Distribuição do enunciado e das alternativas.
    const float margem = 16.0f; // Recuo lateral dos painéis e superior do enunciado.
    const float altura_enunciado = 76.0f;
    const float espaco_apos_enunciado = 12.0f;
    const float altura_alternativa = 44.0f;
    const float espaco_entre_alternativas = 8.0f;

    const Rectangle enunciado_quiz = {quiz.x + margem, quiz.y + margem, quiz.width - 2.0f * margem, altura_enunciado};

    Rectangle alternativa_rec[4];
    float y_alternativa = enunciado_quiz.y + enunciado_quiz.height + espaco_apos_enunciado;

    // A mesma área serve para desenhar a alternativa e detectar o clique.
    for (int i = 0; i < 4; i++) {
        alternativa_rec[i] =
            (Rectangle){enunciado_quiz.x, y_alternativa, enunciado_quiz.width, altura_alternativa};
        y_alternativa += altura_alternativa + espaco_entre_alternativas;
    }

    SetConfigFlags(FLAG_VSYNC_HINT); // Pede que a apresentação dos quadros acompanhe a atualização do monitor.
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
    const int tamanho_titulo = 58;
    Font fonte_titulo = carregar_fonte_interface(tamanho_titulo);
    if (!fonte_carregada(fonte_titulo)) {
        TraceLog(LOG_ERROR, "Falha ao carregar a fonte do título");
        UnloadFont(fonte);
        CloseWindow();
        return;
    }

    carregar_arte_cenas();

    int escala_da_fonte = 1;
    int ultima_escala_solicitada = 1;

    while (!WindowShouldClose()) {
        bool janela_em_foco = IsWindowFocused();
        // A raylib alterna o modo sem bordas e restaura o tamanho e a posição anteriores.
        if (janela_em_foco && IsKeyPressed(KEY_F11))
            ToggleBorderlessWindowed();

        // Sem uma área útil, avança o quadro sem desenhar a interface.
        Rectangle area_exibida = calcular_area_exibicao();
        if (area_exibida.width <= 0 || area_exibida.height <= 0) {
            BeginDrawing();
            EndDrawing();
            continue;
        }

        // Para o mouse, fazemos o caminho janela → jogo: tiramos a margem e dividimos pela escala.
        // Assim GetMousePosition() pode ser comparado diretamente com os retângulos de 960 × 540.
        SetMouseOffset(-(int)area_exibida.x, -(int)area_exibida.y);
        SetMouseScale(LARGURA_BASE / area_exibida.width, ALTURA_BASE / area_exibida.height);

        float escala = area_exibida.width / LARGURA_BASE;
        int escala_dos_pixels = (int)(escala * GetRenderWidth() / GetScreenWidth() + 0.5f);
        if (escala_dos_pixels < 1)
            escala_dos_pixels = 1;

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
                }
                else {
                    if (fonte_carregada(novo_texto))
                        UnloadFont(novo_texto);
                    if (fonte_carregada(novo_titulo))
                        UnloadFont(novo_titulo);
                    TraceLog(LOG_WARNING, "Mantendo as fontes da escala anterior");
                }
            }
        }

        bool confirmou = janela_em_foco && IsKeyPressed(KEY_ENTER); // Aceita Enter enquanto o jogo está em foco.
        int alternativa_clicada = -1;                               // Índice clicado neste quadro, ou -1 sem clique válido.
        int ajuda_clicada = -1;                                     // Índice da ajuda clicada neste quadro, ou -1.

        if (tela_atual == TELA_JORNADA && !resposta_confirmada) {
            // Acumula a duração do último quadro enquanto a pergunta está aberta.
            if (partida.situacao == andamento && pergunta_atual && !faltam_perguntas)
                partida.tempo_ativo += GetFrameTime();
            // Só registra um clique em alternativa que ainda pode ser escolhida.
            for (int i = 0; i < 4; i++) {
                if (!dicas.eliminadas[i] && botao_clicado(alternativa_rec[i])) {
                    alternativa_clicada = i;
                    break;
                }
            }
            // Só registra um clique em ajuda que ainda tem usos disponíveis.
            for (int i = 0; i < TOTAL_AJUDAS; i++) {
                if (ajuda_disponivel(i, usos_ajuda[i], &dicas, resposta_confirmada) && botao_clicado(ajuda_rec[i])) {
                    ajuda_clicada = i;
                    break;
                }
            }
        }

        // Compara o clique com a seleção que já existia antes dele.
        bool repetiu_alternativa = alternativa_clicada >= 0 && alternativa_clicada == alternativa_selecionada;

        /* Bloco de validação de ações por telas */
        if (tela_atual == TELA_MENU) {
            AcaoMenu acao = ler_acao_menu(&botoes_menu, janela_em_foco, confirmou);
            if (acao == MENU_PREVIA_GRUTA) {
                abrir_previa_gruta(&tela_gruta);
                tela_atual = TELA_GRUTA;
            }
            else if (acao == MENU_CREDITOS)
                tela_atual = TELA_CREDITOS;
            else if (acao == MENU_RANKING) {
                carregar_ranking(&tela_ranking);
                tela_atual = TELA_RANKING;
            }
            else if (acao == MENU_GERENCIAR) {
                dificuldade_em_falta = 0;
                faltam_perguntas = false;
                abrir_gerenciamento(&tela_gerenciamento, voltar);
                tela_atual = TELA_GERENCIAMENTO;
            }
            else if (acao == MENU_SAIR) // Encerra o while principal.
                break;
            else if (acao == MENU_JOGAR) {
                // A reserva de perguntas de cada dificuldade cobre também os renascimentos.
                dificuldade_em_falta = verifica_banco_para_partida(banco);
                if (dificuldade_em_falta == 0) {
                    inicializa_partida(&partida);
                    // Cada tentativa começa com o formulário de registro limpo.
                    tela_registro = (TelaRegistro){0};
                    pergunta_atual = sorteia_pergunta(banco, &partida);
                    faltam_perguntas = pergunta_atual == NULL;

                    if (!faltam_perguntas) {
                        questao_exibida = partida.questao_atual;
                        usos_ajuda[AJUDA_BURRO] = 2;
                        usos_ajuda[AJUDA_DRAGAO] = 1;
                        usos_ajuda[AJUDA_PRINCESA] = 1;
                        dicas = (DicasPergunta){0};
                        alternativa_selecionada = -1;
                        resposta_confirmada = false;
                        tela_atual = TELA_JORNADA;
                    }
                }
            }
        }
        // Cancelar preserva a pergunta aberta. Confirmar encerra a tentativa no último marco.
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            if (botao_clicado(voltar)) // Voltar nessa tela tem ação de cancelar
                tela_atual = TELA_JORNADA;
            else if (botao_clicado(acao_quiz)) { // Confirma a retirada
                int marco = confirma_retirada(&partida);
                if (marco >= 0) {
                    preparar_gruta_retirada(&tela_gruta, marco);
                    // O resultado precisa ser salvo antes da gruta quando entra no ranking.
                    tela_atual = partida_entra_no_ranking(&partida) ? TELA_REGISTRAR_RESULTADO : TELA_GRUTA;
                }
            }
        }
        else if (tela_atual == TELA_RANKING) {
            if (botao_clicado(voltar)) {
                liberar_ranking(&tela_ranking);
                tela_atual = TELA_MENU;
            }
        }
        else if (tela_atual == TELA_CREDITOS) {
            if (botao_clicado(voltar))
                tela_atual = TELA_MENU;
        }
        else if (tela_atual == TELA_GERENCIAMENTO) {
            if (atualizar_gerenciamento(&tela_gerenciamento, banco, janela_em_foco))
                tela_atual = TELA_MENU;
        }
        else if (tela_atual == TELA_REGISTRAR_RESULTADO) {
            if (atualizar_registro(&tela_registro, &partida, salvar_ranking, janela_em_foco, confirmou))
                tela_atual = TELA_GRUTA;
        }
        // Ações do botão de voltar em cada tela.
        else if (botao_clicado(voltar)) {
            if (tela_atual == TELA_GRUTA || faltam_perguntas)
                tela_atual = TELA_MENU;
            else if (!resposta_confirmada)
                tela_atual = TELA_CONFIRMAR_RETIRADA;
            else if (partida.situacao == derrota || partida.situacao == vitoria) {
                // O jogador já leu o resultado. Só a vitória leva ouro para a gruta.
                preparar_gruta_final(&tela_gruta, partida.situacao);
                tela_atual = partida_entra_no_ranking(&partida) ? TELA_REGISTRAR_RESULTADO : TELA_GRUTA;
            }
        }
        else if (tela_atual == TELA_GRUTA) {
            atualizar_previa_gruta(&tela_gruta, janela_em_foco);
        }
        // Consome o uso e aplica o efeito à pergunta que já está na tela.
        else if (ajuda_clicada >= 0) {
            usos_ajuda[ajuda_clicada]--;
            if (ajuda_clicada == AJUDA_BURRO)
                dicas.burro_usado = true;
            // Na versão final aqui o minigame vai validar se realmente a ajuda vai ser aplicada.
            aplicar_ajuda(ajuda_clicada, pergunta_atual, &dicas);

            if (alternativa_selecionada >= 0 && dicas.eliminadas[alternativa_selecionada])
                alternativa_selecionada = -1;
        }
        // Enter, o botão de ação ou repetir o clique na alternativa confirmam a escolha.
        else if (!faltam_perguntas && alternativa_selecionada >= 0 &&
                 (confirmou || botao_clicado(acao_quiz) || repetiu_alternativa)) {
            if (!resposta_confirmada) {
                char resposta = 'A' + alternativa_selecionada;
                // O quiz aplica as regras da partida. A tela guarda o resultado para dar o retorno ao jogador.
                resultado_resposta = processa_resposta(&partida, pergunta_atual, resposta);
                resposta_confirmada = true;
            }
            // Acertos, marcos e renascimentos deixam a partida em andamento.
            else if (partida.situacao == andamento) {
                const Pergunta *proxima = sorteia_pergunta(banco, &partida);
                if (proxima == NULL)
                    faltam_perguntas = true;
                else {
                    // Troca a pergunta só depois de um sorteio bem sucedido.
                    pergunta_atual = proxima;
                    questao_exibida = partida.questao_atual;
                    alternativa_selecionada = -1;
                    resposta_confirmada = false;
                    // Efeitos somem ao trocar a pergunta. Os usos restantes resetam por partida.
                    dicas = (DicasPergunta){0};
                }
            }
        }
        // Um clique válido muda a escolha. Nos demais quadros, a seleção continua guardada.
        else if (alternativa_clicada >= 0) {
            alternativa_selecionada = alternativa_clicada;
        }

        // Para desenhar, a câmera faz o caminho jogo → janela: amplia pelo zoom e soma a margem.
        Camera2D camera = {.offset = {area_exibida.x, area_exibida.y}, .zoom = escala};
        BeginDrawing();

        ClearBackground(BLACK); // Limpa a janela inteira, inclusive as barras que sobram ao redor do jogo.
        // O recorte usa coordenadas da janela. Tudo que cair fora dessa área deixa as barras intactas.
        BeginScissorMode((int)area_exibida.x, (int)area_exibida.y, (int)area_exibida.width, (int)area_exibida.height);
        BeginMode2D(camera); // Daqui até EndMode2D, podemos desenhar nas coordenadas de 960 × 540.
        bool usa_fundo_escuro = tela_atual == TELA_MENU ||
                                tela_atual == TELA_CREDITOS ||
                                tela_atual == TELA_GERENCIAMENTO ||
                                tela_atual == TELA_RANKING ||
                                tela_atual == TELA_REGISTRAR_RESULTADO;
        Color fundo = usa_fundo_escuro ? COR_FUNDO_MENU : RAYWHITE;
        DrawRectangle(0, 0, LARGURA_BASE, ALTURA_BASE, fundo);

        /* Desenha as telas e seus elementos. */
        if (tela_atual == TELA_MENU) {
            const char *nivel = dificuldade_em_falta == 0 ? NULL : nomes_dificuldade[dificuldade_em_falta];
            desenhar_menu(&botoes_menu, fonte, fonte_titulo, tamanho_titulo, nivel, faltam_perguntas);
        }
        else if (tela_atual == TELA_RANKING) {
            desenhar_ranking(&tela_ranking, fonte, fonte_titulo, voltar);
        }
        else if (tela_atual == TELA_CREDITOS) {
            desenhar_creditos(fonte, fonte_titulo, mapa, quiz, voltar);
        }
        else if (tela_atual == TELA_GERENCIAMENTO) {
            desenhar_gerenciamento(&tela_gerenciamento, banco, fonte, fonte_titulo, nomes_dificuldade);
        }
        else if (tela_atual == TELA_JORNADA) {
            const int questoes_jornada = 8;
            const int total_questoes = 15;
            bool em_batalha = questao_exibida > questoes_jornada;
            bool renascendo = resposta_confirmada && resultado_resposta == resposta_renascimento;

            const char *nome_etapa = em_batalha ? "Batalha" : "Jornada";
            const char *titulo_quiz = TextFormat("%s - Questão %d de %d", nome_etapa, questao_exibida, total_questoes);
            DrawTextEx(fonte, titulo_quiz, (Vector2){24, 24}, tamanho_corpo, 1, BLACK);

            // Alinha o contador de ovos à direita da cena, na mesma linha do título.
            const char *ovos = TextFormat("Ovos: %d", partida.ovos_restantes);
            float largura_ovos = MeasureTextEx(fonte, ovos, 20, 1).x;
            DrawTextEx(fonte, ovos, (Vector2){mapa.x + mapa.width - largura_ovos, 24}, 20, 1, BLACK);

            CenaQuiz cena = em_batalha ? CENA_BATALHA : CENA_JORNADA;
            if (partida.situacao == derrota)
                cena = em_batalha ? CENA_DERROTA : CENA_DERROTA_JORNADA; // Exibe o dragão caído no castelo ou na jornada dependendo até onde ele chegou.
            else if (partida.situacao == vitoria)
                cena = CENA_VITORIA;

            desenhar_painel(mapa, COR_ESCURO);
            desenhar_arte_quiz(cena);

            const char *titulo_cenario = em_batalha ? "Castelo do príncipe" : "Mapa aéreo";
            if (cena == CENA_DERROTA_JORNADA)
                titulo_cenario = "Estrada para o castelo";
            DrawTextEx(fonte, titulo_cenario, (Vector2){quiz.x, 24}, tamanho_corpo, 1, BLACK);

            const Vector2 posicao_progresso = {mapa.x, mapa.y - 39}; // Rótulo na segunda linha acima da cena.
            if (renascendo) {
                // Enquanto a pergunta anterior aparece, o progresso da partida já está no marco.
                const char *marco = TextFormat("Marco: %d", partida.checkpoint);
                DrawTextEx(fonte, marco, posicao_progresso, 20, 1, BLACK);
            }
            else {
                int valor = em_batalha ? calcula_pv_principe(&partida) : partida.acertos;
                int limite = em_batalha ? PV_MAX_PRINCIPE : questoes_jornada;
                const char *progresso = TextFormat("%s: %d/%d", em_batalha ? "PV" : "Etapas", valor, limite);
                DrawTextEx(fonte, progresso, posicao_progresso, 20, 1, BLACK);

                // A fração do valor que resta define a largura colorida da barra.
                const float espaco_progresso = 164;                                                                 // Reserva a coluna do rótulo à esquerda da barra.
                Rectangle fundo_vida = {mapa.x + espaco_progresso, mapa.y - 33, mapa.width - espaco_progresso, 16}; // Centraliza a barra no rótulo.
                Rectangle preenchimento_vida = fundo_vida;
                preenchimento_vida.width *= (float)valor / limite;

                DrawRectangleRec(fundo_vida, RAYWHITE);
                DrawRectangleRec(preenchimento_vida, em_batalha ? MAROON : DARKGREEN);
                DrawRectangleLinesEx(fundo_vida, 2, DARKGRAY);
            }

            // Mostra os usos restantes e apaga os botões indisponíveis.
            for (int i = 0; i < TOTAL_AJUDAS; i++) {
                const char *rotulo = TextFormat("%s (%d)", nomes_ajuda[i], usos_ajuda[i]);
                bool habilitado = ajuda_disponivel(i, usos_ajuda[i], &dicas, resposta_confirmada);
                desenhar_botao_com_estado(fonte, ajuda_rec[i], rotulo, BOTAO_SECUNDARIO, habilitado);
            }

            // Mostra a revelação da Princesa quando ativa. Caso contrário, mostra o palpite do Burro.
            const char *texto_dica = NULL;
            if (dicas.resposta_revelada)
                texto_dica = TextFormat("Princesa: resposta %c.", pergunta_atual->resposta);
            else if (dicas.palpite_burro != '\0')
                texto_dica = TextFormat("Burro: apostaria na %c.", dicas.palpite_burro);
            if (texto_dica != NULL) {
                Vector2 posicao_dica = {quiz.x, y_ajudas}; // Dica na mesma altura das ajudas, à direita da cena.
                DrawTextEx(fonte, texto_dica, posicao_dica, 20, 1, BLACK);
            }

            // Desenha a caixa do quiz e o enunciado da questão.
            DrawRectangleRec(quiz, LIGHTGRAY);
            desenhar_enunciado(fonte, enunciado_quiz, pergunta_atual);

            // Define os rótulos das alternativas e as desenha.
            const char *rotulos[] = {pergunta_atual->alternativa.A, pergunta_atual->alternativa.B,
                                     pergunta_atual->alternativa.C, pergunta_atual->alternativa.D};
            for (int i = 0; i < 4; i++) {
                // Mantém o texto da alternativa para o jogador ver o que o Dragão apagou.
                char letra = 'A' + i;
                bool selecionado = i == alternativa_selecionada;
                bool habilitado = !dicas.eliminadas[i] && !resposta_confirmada;
                desenhar_alternativa(fonte, alternativa_rec[i], rotulos[i], selecionado, habilitado);

                // Desenha a marca da ajuda por cima do cartão e preserva a cor da seleção.
                if (dicas.resposta_revelada && letra == pergunta_atual->resposta)
                    DrawRectangleLinesEx(alternativa_rec[i], 4.0f, DARKGREEN);
                else if (!dicas.resposta_revelada && letra == dicas.palpite_burro)
                    DrawRectangleLinesEx(alternativa_rec[i], 4.0f, BROWN);
            }

            const char *mensagem = NULL; // Porque antes de selecionar não há mensagem para exibir.
            if (faltam_perguntas)
                mensagem = "Sem perguntas para avançar.";
            // Define a mensagem dependendo se errou, acertou ou ganhou;
            else if (resposta_confirmada) {
                if (renascendo)
                    mensagem = TextFormat("Ovo usado. Próxima: questão %d.", partida.checkpoint + 1);
                else if (resultado_resposta == resposta_marco)
                    mensagem = "Marco alcançado! Você ganhou um ovo.";
                else if (resultado_resposta == resposta_derrota)
                    mensagem = "Resposta incorreta.";
                else if (resultado_resposta == resposta_vitoria)
                    mensagem = "Vitória! A princesa foi salva.";
                else
                    mensagem = "Resposta correta!";
            }
            // Alternativa selecionada antes de confirmar
            else if (alternativa_selecionada >= 0)
                mensagem = TextFormat("Alternativa '%c' selecionada.", 'A' + alternativa_selecionada); // Em C, aspas simples representa um caractere e aspas duplas representa uma string, por isso deve ser 'A', e não "A".
            if (mensagem != NULL) {
                Vector2 posicao = {quiz.x, mapa.y - 39}; // Mensagem abaixo do título e acima da caixa de perguntas.
                DrawTextEx(fonte, mensagem, posicao, 20, 1.0f, BLACK);
            }

            if (alternativa_selecionada >= 0 && !faltam_perguntas && partida.situacao == andamento) {
                const char *rotulo = resposta_confirmada ? "Continuar" : "Confirmar";
                desenhar_botao(fonte, acao_quiz, rotulo);
            }

            if (faltam_perguntas)
                desenhar_botao(fonte, voltar, "Voltar");
            else if (partida.situacao == vitoria)
                desenhar_botao(fonte, voltar, "Registrar nome");
            else if (partida.situacao == derrota)
                desenhar_botao(fonte, voltar, "Ver gruta");
            else
                // O !resposta_confimada significa que o botão desabilita após o usuário confirmar uma alternativa.
                desenhar_botao_com_estado(fonte, voltar, "Retirada Tática", BOTAO_SECUNDARIO, !resposta_confirmada);
        }
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            DrawTextEx(fonte_titulo, "Retirada Tática", (Vector2){48, 96}, tamanho_titulo, 1, BLACK);
            DrawTextEx(fonte, "Encerrar a tentativa e voltar à gruta?", (Vector2){48, 184}, tamanho_corpo, 1, BLACK);
            desenhar_botao(fonte, voltar, "Cancelar");
            desenhar_botao(fonte, acao_quiz, "Retirar");
        }
        else if (tela_atual == TELA_REGISTRAR_RESULTADO) {
            desenhar_registro(&tela_registro, fonte, fonte_titulo, salvar_ranking);
        }
        else if (tela_atual == TELA_GRUTA) {
            desenhar_tela_gruta(&tela_gruta, &partida, fonte, voltar, quiz.x, tamanho_corpo);
        }

        // Sai das coordenadas do jogo, libera o recorte e apresenta o quadro pronto.
        EndMode2D();
        EndScissorMode();
        EndDrawing();
    }

    liberar_ranking(&tela_ranking);
    descarregar_arte_cenas();

    // Descarrega as fontes.
    UnloadFont(fonte_titulo);
    UnloadFont(fonte);
    CloseWindow();
}
