/**
 * @file telas.c
 * @author Gabriel Honji (gabrielhonji@alunos.utfpr.edu.br)
 * @brief Mantém a janela, o loop da raylib e as transições entre telas.
 * @version 0.1
 * @date 2026-11-15
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "telas.h"
#include "comum/arte_cenas.h"
#include "comum/audio.h"
#include "comum/componentes.h"
#include "comum/cores.h"
#include "../quiz/quiz.h"
#include "menu/tela_menu.h"
#include "menu/tela_creditos.h"
#include "ranking/tela_ranking.h"
#include "ranking/tela_registro.h"
#include "partida/tela_gruta.h"
#include "gerenciamento/tela_gerenciamento.h"
#include "partida/tela_quiz.h"
#include "partida/tela_retirada.h"
#include "partida/tela_transicao.h"
#include "minigames/tela_burro.h"
#include "minigames/tela_eco.h"
#include "minigames/tela_sopro.h"

#ifdef __linux__
// Xlib e raylib usam o nome Font para tipos diferentes.
#define Font X11Font
#include <X11/Xlib.h>
#undef Font

// A raylib 5.5 fornece a janela GLFW em GetWindowHandle() no Linux.
typedef struct GLFWwindow GLFWwindow;
extern Display *glfwGetX11Display(void);
extern Window glfwGetX11Window(GLFWwindow *janela);
#endif

typedef enum
{
    TELA_MENU,
    TELA_INTRODUCAO,
    TELA_CHEGADA_CASTELO,
    TELA_JORNADA,
    TELA_SOPRO,
    TELA_BURRO,
    TELA_ECO,
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
    MARGEM_BASE = 12,
};

/** @brief Alterna a tela cheia pelo gerenciador de janelas quando a sessão oferece esse recurso. */
static void alternar_tela_cheia(void)
{
#ifdef __linux__
    Display *tela = glfwGetX11Display();
    if (tela != NULL) {
        Atom estado = XInternAtom(tela, "_NET_WM_STATE", True);
        Atom tela_cheia = XInternAtom(tela, "_NET_WM_STATE_FULLSCREEN", True);
        if (estado != None && tela_cheia != None) {
            // O gerenciador cobre o monitor inteiro e depois restaura a geometria da janela.
            const long alternar_estado = 2;
            const long origem_aplicativo = 1;
            XEvent evento = {.xclient = {
                .type = ClientMessage,
                .window = glfwGetX11Window(GetWindowHandle()),
                .message_type = estado,
                .format = 32,
                .data.l = {alternar_estado, (long)tela_cheia, 0, origem_aplicativo, 0},
            }};
            if (XSendEvent(tela, DefaultRootWindow(tela), False,
                           SubstructureRedirectMask | SubstructureNotifyMask, &evento)) {
                XFlush(tela);
                return;
            }
        }
    }
#endif
    ToggleBorderlessWindowed();
}

/**
 * @brief Calcula a área da janela após reservar a mesma margem nas quatro bordas.
 */
static Rectangle calcular_area_interna(void)
{
    int largura_tela = GetScreenWidth();
    int altura_tela = GetScreenHeight();
    float escala_x = (float)largura_tela / LARGURA_BASE;
    float escala_y = (float)altura_tela / ALTURA_BASE;
    int margem = (int)(MARGEM_BASE * (escala_x < escala_y ? escala_x : escala_y));
    return (Rectangle){margem, margem, largura_tela - 2 * margem, altura_tela - 2 * margem};
}

/**
 * @brief Centraliza o jogo na área interna, preservando suas proporções.
 * @param area Área depois de aplicar as margens externas.
 * @return Retângulo de exibição do quadro na janela.
 */
static Rectangle calcular_area_exibicao(Rectangle area)
{
    float escala_x = area.width / LARGURA_BASE;
    float escala_y = area.height / ALTURA_BASE;
    float escala = escala_x < escala_y ? escala_x : escala_y;

    int largura = (int)(LARGURA_BASE * escala + 0.5f);
    int altura = (int)(ALTURA_BASE * escala + 0.5f);
    return (Rectangle){area.x + (int)(area.width - largura) / 2,
                       area.y + (int)(area.height - altura) / 2, largura, altura};
}

/**
 * @brief Prepara a textura em que o quadro inteiro será desenhado.
 * @param escala Fator inteiro aplicado à resolução lógica da interface.
 * @return Textura de renderização. A validade deve ser verificada antes do uso.
 */
static RenderTexture2D carregar_quadro(int escala)
{
    RenderTexture2D quadro = LoadRenderTexture(LARGURA_BASE * escala, ALTURA_BASE * escala);
    if (IsRenderTextureValid(quadro))
        SetTextureFilter(quadro.texture, TEXTURE_FILTER_POINT);
    return quadro;
}

/**
 * @brief Prolonga a vinheta do sopro pelas faixas internas do enquadramento.
 * @param textura Quadro renderizado com a vinheta.
 * @param quadro Retângulo ocupado pelo jogo na janela.
 * @param limite Área interna até onde a vinheta deve chegar.
 */
static void estender_bordas_sopro(Texture2D textura, Rectangle quadro, Rectangle limite)
{
    const Vector2 origem = {0, 0};
    if (quadro.width < limite.width) {
        Rectangle esquerda = {limite.x, quadro.y, quadro.x - limite.x, quadro.height};
        float fim = quadro.x + quadro.width;
        Rectangle direita = {fim, quadro.y, limite.x + limite.width - fim, quadro.height};
        DrawTexturePro(textura, (Rectangle){0, 0, 1, -textura.height}, esquerda, origem, 0, WHITE);
        DrawTexturePro(textura, (Rectangle){textura.width - 1, 0, 1, -textura.height}, direita, origem, 0, WHITE);
    }
    else if (quadro.height < limite.height) {
        Rectangle topo = {quadro.x, limite.y, quadro.width, quadro.y - limite.y};
        float fim = quadro.y + quadro.height;
        Rectangle base = {quadro.x, fim, quadro.width, limite.y + limite.height - fim};
        DrawTexturePro(textura, (Rectangle){0, textura.height - 1, textura.width, 1}, topo, origem, 0, WHITE);
        DrawTexturePro(textura, (Rectangle){0, 0, textura.width, 1}, base, origem, 0, WHITE);
    }
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
 * @brief Escolhe a música de acordo com a tela e a etapa exibida.
 * @param tela Tela ativa no quadro.
 * @param quiz Estado da pergunta, usado para distinguir a Jornada da Batalha.
 * @return Trilha da Batalha no quiz do castelo. A trilha da Jornada cobre as demais telas.
 */
static TrilhaAudio trilha_da_tela(Tela tela, const TelaQuiz *quiz)
{
    if (tela == TELA_CHEGADA_CASTELO || ((tela == TELA_JORNADA || tela == TELA_CONFIRMAR_RETIRADA) && quiz_em_batalha(quiz)))
        return TRILHA_BATALHA;

    // Compartilhar a trilha evita reiniciar a música ao abrir Créditos, Ranking ou outras telas.
    return TRILHA_JORNADA;
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

    TelaQuiz tela_quiz = {0};
    MinigameSopro sopro = {0}; // É reiniciado ao abrir o desafio, sem trocar a pergunta.
    MinigameBurro burro = {0};
    FlappyBird eco = {0};
    const LayoutQuiz layout_quiz = montar_layout_quiz(mapa, quiz, voltar);

    unsigned int flags = FLAG_VSYNC_HINT;
#ifdef __linux__
    // O gerenciador pode recusar tela cheia para uma janela de tamanho fixo.
    flags |= FLAG_WINDOW_RESIZABLE;
#endif
    SetConfigFlags(flags);
    InitWindow(LARGURA_BASE, ALTURA_BASE, "Dragão do Milhão");
    Image icone_janela = LoadImage("assets/visual/icones/dragao_app.png");
    if (IsImageValid(icone_janela)) {
        SetWindowIcon(icone_janela);
        UnloadImage(icone_janela); // A janela já recebeu uma cópia do ícone.
    }
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

    RenderTexture2D quadro = carregar_quadro(1);
    if (!IsRenderTextureValid(quadro)) {
        TraceLog(LOG_ERROR, "Falha ao preparar a textura da interface");
        if (quadro.id != 0)
            UnloadRenderTexture(quadro);
        UnloadFont(fonte_titulo);
        UnloadFont(fonte);
        CloseWindow();
        return;
    }

    carregar_arte_cenas();
    iniciar_audio();

    int escala_do_quadro = 1;
    int ultima_escala_solicitada = 1;

    while (!WindowShouldClose()) {
        bool janela_em_foco = IsWindowFocused();
        if (janela_em_foco && IsKeyPressed(KEY_F11))
            alternar_tela_cheia();

        // Sem uma área útil, avança o quadro sem desenhar a interface.
        Rectangle area_interna = calcular_area_interna();
        Rectangle area_exibida = calcular_area_exibicao(area_interna);
        if (area_exibida.width <= 0 || area_exibida.height <= 0) {
            // Mesmo quando pulamos o desenho do quadro, a música precisa receber mais amostras.
            atualizar_audio(trilha_da_tela(tela_atual, &tela_quiz));
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

        // Atualiza a textura e rasteriza as fontes na escala em pixels do quadro.
        if (escala_dos_pixels != ultima_escala_solicitada) {
            ultima_escala_solicitada = escala_dos_pixels;

            if (escala_dos_pixels != escala_do_quadro) {
                RenderTexture2D novo_quadro = carregar_quadro(escala_dos_pixels);
                Font novo_texto = carregar_fonte_interface(tamanho_corpo * escala_dos_pixels);
                Font novo_titulo = carregar_fonte_interface(tamanho_titulo * escala_dos_pixels);

                if (IsRenderTextureValid(novo_quadro) && fonte_carregada(novo_texto) && fonte_carregada(novo_titulo)) {
                    UnloadRenderTexture(quadro);
                    UnloadFont(fonte);
                    UnloadFont(fonte_titulo);
                    quadro = novo_quadro;
                    fonte = novo_texto;
                    fonte_titulo = novo_titulo;
                    escala_do_quadro = escala_dos_pixels;
                    TraceLog(LOG_INFO, "Fontes atualizadas: texto %d px, titulo %d px",
                             fonte.baseSize, fonte_titulo.baseSize);
                }
                else {
                    if (novo_quadro.id != 0)
                        UnloadRenderTexture(novo_quadro);
                    if (fonte_carregada(novo_texto))
                        UnloadFont(novo_texto);
                    if (fonte_carregada(novo_titulo))
                        UnloadFont(novo_titulo);
                    TraceLog(LOG_WARNING, "Mantendo a textura e as fontes da escala anterior");
                }
            }
        }

        bool confirmou = janela_em_foco && IsKeyPressed(KEY_ENTER); // Aceita Enter enquanto o jogo está em foco.
        /* Bloco de validação de ações por telas */
        Tela tela_anterior = tela_atual;
        if (tela_atual == TELA_MENU) {
            AcaoMenu acao = ler_acao_menu(&botoes_menu, janela_em_foco, confirmou);
            if (acao == MENU_PREVIA_GRUTA) {
                abrir_previa_gruta(&tela_gruta);
                tela_atual = TELA_GRUTA;
            }
            else if (acao == MENU_CREDITOS)
                tela_atual = TELA_CREDITOS;
            else if (acao == MENU_ALTERNAR_SOM) {
                alternar_som();
                tocar_efeito(SOM_BOTAO); // Ao religar o som, o clique já pode ser ouvido.
            }
            else if (acao == MENU_RANKING) {
                carregar_ranking(&tela_ranking);
                tela_atual = TELA_RANKING;
            }
            else if (acao == MENU_GERENCIAR) {
                dificuldade_em_falta = 0;
                tela_quiz.faltam_perguntas = false;
                abrir_gerenciamento(&tela_gerenciamento, voltar);
                tela_atual = TELA_GERENCIAMENTO;
            }
            else if (acao == MENU_SAIR) // Encerra o while principal.
                break;
            else if (acao == MENU_JOGAR) {
                // A reserva de perguntas de cada dificuldade cobre também os renascimentos.
                dificuldade_em_falta = verifica_banco_para_partida(banco);
                if (dificuldade_em_falta == 0)
                    tela_atual = TELA_INTRODUCAO;
                else
                    tocar_efeito(SOM_ERRO);
            }
        }
        else if (tela_atual == TELA_INTRODUCAO) {
            AcaoTransicao acao = ler_acao_transicao(TRANSICAO_ABERTURA, confirmou);
            if (acao == TRANSICAO_VOLTAR)
                tela_atual = TELA_MENU;
            else if (acao == TRANSICAO_AVANCAR) {
                tela_registro = (TelaRegistro){0};
                tela_atual = iniciar_quiz(&tela_quiz, banco) ? TELA_JORNADA : TELA_MENU;
            }
        }
        else if (tela_atual == TELA_CHEGADA_CASTELO) {
            if (ler_acao_transicao(TRANSICAO_CHEGADA, confirmou) == TRANSICAO_AVANCAR)
                tela_atual = TELA_JORNADA;
        }
        // Cancelar preserva a pergunta aberta. Confirmar encerra a tentativa no último marco.
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            AcaoRetirada acao = ler_acao_retirada();
            if (acao == RETIRADA_CANCELAR)
                tela_atual = TELA_JORNADA;
            else if (acao == RETIRADA_CONFIRMAR) {
                int marco = confirma_retirada(&tela_quiz.partida);
                if (marco >= 0) {
                    preparar_gruta_retirada(&tela_gruta, marco);
                    // O resultado precisa ser salvo antes da gruta quando entra no ranking.
                    tela_atual = partida_entra_no_ranking(&tela_quiz.partida) ? TELA_REGISTRAR_RESULTADO : TELA_GRUTA;
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
            if (atualizar_registro(&tela_registro, &tela_quiz.partida, janela_em_foco, confirmou))
                tela_atual = TELA_GRUTA;
        }
        else if (tela_atual == TELA_JORNADA) {
            AcaoQuiz acao = atualizar_quiz(&tela_quiz, &layout_quiz, banco, confirmou);
            if (acao == QUIZ_VOLTAR_MENU)
                tela_atual = TELA_MENU;
            else if (acao == QUIZ_PEDIR_RETIRADA)
                tela_atual = TELA_CONFIRMAR_RETIRADA;
            else if (acao == QUIZ_CHEGOU_CASTELO)
                tela_atual = TELA_CHEGADA_CASTELO;
            else if (acao == QUIZ_ABRIR_SOPRO) {
                iniciarMinigameSopro(&sopro);
                tela_atual = TELA_SOPRO;
            }
            else if (acao == QUIZ_ABRIR_BURRO) {
                iniciarMinigameBurro(&burro);
                tela_atual = TELA_BURRO;
            }
            else if (acao == QUIZ_ABRIR_ECO) {
                iniciarMinigame_flappybird(&eco);
                tela_atual = TELA_ECO;
            }
            else if (acao == QUIZ_MOSTRAR_RESULTADO) {
                preparar_gruta_final(&tela_gruta, tela_quiz.partida.situacao);
                tela_atual = partida_entra_no_ranking(&tela_quiz.partida) ? TELA_REGISTRAR_RESULTADO : TELA_GRUTA;
            }
        }
        else if (tela_atual == TELA_SOPRO) {
            if (atualizar_tela_sopro(&sopro, janela_em_foco)) {
                // O uso foi gasto ao abrir. A vitória libera o efeito na pergunta que ficou aberta.
                if (sopro.resultado == 1)
                    aplicar_efeito_ajuda_quiz(&tela_quiz, AJUDA_DRAGAO);
                tela_atual = TELA_JORNADA;
            }
        }
        else if (tela_atual == TELA_BURRO) {
            if (atualizar_tela_burro(&burro)) {
                if (burro.resultado == 1)
                    aplicar_efeito_ajuda_quiz(&tela_quiz, AJUDA_BURRO);
                tela_atual = TELA_JORNADA;
            }
        }
        else if (tela_atual == TELA_ECO) {
            if (atualizar_tela_eco(&eco, janela_em_foco)) {
                // A rodada usa o mesmo estado do quiz. Assim o gabarito chega à pergunta que ficou aberta.
                if (eco.resultado == 1)
                    aplicar_efeito_ajuda_quiz(&tela_quiz, AJUDA_PRINCESA);
                tela_atual = TELA_JORNADA;
            }
        }
        else if (tela_atual == TELA_GRUTA) {
            if (botao_clicado(voltar))
                tela_atual = TELA_MENU;
            else
                atualizar_previa_gruta(&tela_gruta, janela_em_foco);
        }

        // Um único clique acompanha a mudança de tela, inclusive quando o jogador usa Enter.
        if (tela_atual != tela_anterior)
            tocar_efeito(SOM_BOTAO);
        // A nova tela determina qual música vai tocar neste quadro.
        atualizar_audio(trilha_da_tela(tela_atual, &tela_quiz));

        Color fundo = COR_FUNDO_MENU;
        // As bordas da textura limitam o desenho nas coordenadas de 960 × 540.
        BeginTextureMode(quadro);
        ClearBackground(fundo);
        BeginMode2D((Camera2D){.zoom = escala_do_quadro});

        /* Desenha as telas e seus elementos. */
        if (tela_atual == TELA_MENU) {
            const char *nivel = dificuldade_em_falta == 0 ? NULL : nomes_dificuldade[dificuldade_em_falta];
            desenhar_menu(&botoes_menu, fonte, fonte_titulo, tamanho_titulo, nivel, tela_quiz.faltam_perguntas, som_esta_ligado());
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
            desenhar_quiz(&tela_quiz, &layout_quiz, fonte, tamanho_corpo);
        }
        else if (tela_atual == TELA_INTRODUCAO || tela_atual == TELA_CHEGADA_CASTELO) {
            TransicaoPartida transicao = tela_atual == TELA_INTRODUCAO ? TRANSICAO_ABERTURA : TRANSICAO_CHEGADA;
            desenhar_transicao(transicao, fonte, fonte_titulo, mapa, quiz);
        }
        else if (tela_atual == TELA_SOPRO) {
            desenhar_sopro(&sopro, fonte, fonte_titulo, tamanho_corpo, tamanho_titulo, LARGURA_BASE, ALTURA_BASE);
        }
        else if (tela_atual == TELA_BURRO) {
            desenhar_burro(&burro, fonte, fonte_titulo, tamanho_corpo, tamanho_titulo);
        }
        else if (tela_atual == TELA_ECO) {
            desenhar_eco(&eco, fonte, fonte_titulo, tamanho_corpo, tamanho_titulo);
        }
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            // A pergunta continua atrás do diálogo, enquanto só o diálogo recebe os cliques.
            desenhar_quiz(&tela_quiz, &layout_quiz, fonte, tamanho_corpo);
            DrawRectangle(0, 0, LARGURA_BASE, ALTURA_BASE, Fade(BLACK, 0.67f));
            desenhar_retirada(fonte, fonte_titulo, tamanho_corpo);
        }
        else if (tela_atual == TELA_REGISTRAR_RESULTADO) {
            desenhar_registro(&tela_registro, fonte, fonte_titulo);
        }
        else if (tela_atual == TELA_GRUTA) {
            desenhar_tela_gruta(&tela_gruta, &tela_quiz.partida, fonte, voltar, quiz.x, tamanho_corpo);
        }

        EndMode2D();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleRec(area_interna, fundo);
        if (tela_atual == TELA_SOPRO)
            estender_bordas_sopro(quadro.texture, area_exibida, area_interna);
        // A textura renderizada no OpenGL tem a origem na parte de baixo.
        Rectangle origem = {0, 0, quadro.texture.width, -quadro.texture.height};
        DrawTexturePro(quadro.texture, origem, area_exibida, (Vector2){0, 0}, 0, WHITE);
        EndDrawing();
    }

    liberar_ranking(&tela_ranking);
    descarregar_arte_cenas();
    encerrar_audio();

    // Libera a textura do quadro e as fontes antes de fechar a janela.
    UnloadRenderTexture(quadro);
    UnloadFont(fonte_titulo);
    UnloadFont(fonte);
    CloseWindow();
}
