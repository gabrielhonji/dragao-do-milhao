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
#include "minigames/tela_burro.h"
#include "minigames/tela_eco.h"
#include "minigames/tela_sopro.h"

typedef enum
{
    TELA_MENU,
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
 * @brief Escolhe a música de acordo com a tela e a etapa exibida.
 * @param tela Tela ativa no quadro.
 * @param quiz Estado da pergunta, usado para distinguir a Jornada da Batalha.
 * @return Trilha da Batalha no quiz do castelo. A trilha da Jornada cobre as demais telas.
 */
static TrilhaAudio trilha_da_tela(Tela tela, const TelaQuiz *quiz)
{
    if (tela == TELA_JORNADA && quiz_em_batalha(quiz))
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
    const Rectangle salvar_ranking = layout_quiz.acao_quiz; // Posição compartilhada no rodapé.

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
    iniciar_audio();

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
        /* Bloco de validação de ações por telas */
        if (tela_atual == TELA_MENU) {
            AcaoMenu acao = ler_acao_menu(&botoes_menu, janela_em_foco, confirmou);
            if (acao == MENU_PREVIA_GRUTA) {
                abrir_previa_gruta(&tela_gruta);
                tela_atual = TELA_GRUTA;
            }
            else if (acao == MENU_CREDITOS)
                tela_atual = TELA_CREDITOS;
            else if (acao == MENU_ALTERNAR_SOM)
                alternar_som();
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
                if (dificuldade_em_falta == 0) {
                    tela_registro = (TelaRegistro){0};
                    if (iniciar_quiz(&tela_quiz, banco))
                        tela_atual = TELA_JORNADA;
                }
            }
        }
        // Cancelar preserva a pergunta aberta. Confirmar encerra a tentativa no último marco.
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            AcaoRetirada acao = ler_acao_retirada(voltar, layout_quiz.acao_quiz);
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
            if (atualizar_registro(&tela_registro, &tela_quiz.partida, salvar_ranking, janela_em_foco, confirmou))
                tela_atual = TELA_GRUTA;
        }
        else if (tela_atual == TELA_JORNADA) {
            AcaoQuiz acao = atualizar_quiz(&tela_quiz, &layout_quiz, banco, confirmou);
            if (acao == QUIZ_VOLTAR_MENU)
                tela_atual = TELA_MENU;
            else if (acao == QUIZ_PEDIR_RETIRADA)
                tela_atual = TELA_CONFIRMAR_RETIRADA;
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

        // As ações já trocaram a tela e a pergunta. Usa a mesma etapa que será desenhada neste quadro.
        atualizar_audio(trilha_da_tela(tela_atual, &tela_quiz));

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
            desenhar_retirada(fonte, fonte_titulo, tamanho_corpo, tamanho_titulo, voltar, layout_quiz.acao_quiz);
        }
        else if (tela_atual == TELA_REGISTRAR_RESULTADO) {
            desenhar_registro(&tela_registro, fonte, fonte_titulo, salvar_ranking);
        }
        else if (tela_atual == TELA_GRUTA) {
            desenhar_tela_gruta(&tela_gruta, &tela_quiz.partida, fonte, voltar, quiz.x, tamanho_corpo);
        }

        // Sai das coordenadas do jogo, libera o recorte e apresenta o quadro pronto.
        EndMode2D();
        EndScissorMode();
        EndDrawing();
    }

    liberar_ranking(&tela_ranking);
    descarregar_arte_cenas();
    encerrar_audio();

    // Descarrega as fontes.
    UnloadFont(fonte_titulo);
    UnloadFont(fonte);
    CloseWindow();
}
