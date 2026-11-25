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
#include "telas.h"
#include "componentes.h"
#include "../quiz/quiz.h"

typedef enum
{
    TELA_MENU,
    TELA_JORNADA,
    TELA_CONFIRMAR_RETIRADA,
    TELA_GRUTA,
} Tela;
typedef enum
{
    GRUTA_VAZIA,
    GRUTA_OURO_BAIXO,
    GRUTA_OURO_MEDIO,
    GRUTA_OURO_ALTO,
} EstadoGruta;
enum
{
    LARGURA_BASE = 960,
    ALTURA_BASE = 540
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
    const float margem = 24.0f;                                                // Margem lateral e recuo da mensagem dentro da gruta.
    const float altura_chao = 52.0f;                                           // Altura da faixa cinza no fundo do cenário.
    const Rectangle cenario = {margem, 80, LARGURA_BASE - 2.0f * margem, 360}; // A largura desconta as duas margens.
    const float centro_x = cenario.x + cenario.width / 2.0f;                   // Centro do cenário na horizontal.
    const float chao_y = cenario.y + cenario.height - altura_chao;             // Sobe 52 px da borda inferior até o topo do chão.
    const float tamanho_texto = 24.0f;                                         // Tamanho dos rótulos da prévia.

    DrawTextEx(fonte, "Gruta", (Vector2){24, 24}, tamanho_texto, 1, BLACK);
    DrawRectangleRec(cenario, DARKGRAY);
    DrawRectangleRec((Rectangle){cenario.x, chao_y, cenario.width, altura_chao}, GRAY);

    if (!mostrar_dragao) {
        DrawTextEx(fonte, "A gruta está vazia.", (Vector2){cenario.x + margem, cenario.y + margem}, tamanho_texto, 1, RAYWHITE);
        return;
    }

    const float altura_ouro = 28.0f * estado;                                                                 // Cada tamanho acrescenta 28 px de altura.
    const float largura_ouro = 150.0f + 48.0f * estado;                                                       // Base de 150 px mais 48 px por tamanho.
    const Rectangle ouro = {centro_x - largura_ouro / 2.0f, chao_y - altura_ouro, largura_ouro, altura_ouro}; // Vai do centro à borda esquerda e do chão ao topo.

    const float largura_dragao = 120.0f;                                                                                // Largura do bloco provisório do dragão.
    const float altura_dragao = 64.0f;                                                                                  // Altura do bloco provisório do dragão.
    const Rectangle dragao = {centro_x - largura_dragao / 2.0f, ouro.y - altura_dragao, largura_dragao, altura_dragao}; // Centraliza o dragão e apoia sua base sobre o ouro.
    if (estado != GRUTA_VAZIA)
        DrawRectangleRec(ouro, GOLD);
    DrawRectangleRec(dragao, MAROON);
    const Vector2 medida_rotulo = MeasureTextEx(fonte, "Dragão", tamanho_texto, 1);
    const Vector2 posicao_rotulo = {dragao.x + (dragao.width - medida_rotulo.x) / 2.0f, dragao.y + (dragao.height - medida_rotulo.y) / 2.0f}; // Divide o espaço livre para centralizar o rótulo.
    DrawTextEx(fonte, "Dragão", posicao_rotulo, tamanho_texto, 1, RAYWHITE);

    const float largura_filhote = 44.0f;     // Largura do bloco de cada filhote.
    const float altura_filhote = 32.0f;      // Altura do bloco de cada filhote.
    const float espaco_ouro_filhote = 24.0f; // Distância entre a pilha e o filhote.
    const float posicoes_filhotes[] = {
        ouro.x - espaco_ouro_filhote - largura_filhote, // Borda esquerda do filhote à esquerda da pilha.
        ouro.x + ouro.width + espaco_ouro_filhote,      // Borda esquerda do filhote à direita da pilha.
    };
    for (int i = 0; i < ovos_restantes && i < 2; i++) {
        Rectangle filhote = {posicoes_filhotes[i], chao_y - altura_filhote, largura_filhote, altura_filhote}; // Apoia a base do filhote no chão.
        DrawRectangleRec(filhote, ORANGE);
    }
}

void executar_tela_teste(const Banco_perguntas *banco)
{
    Tela tela_atual = TELA_MENU;
    EstadoGruta estado_gruta = GRUTA_VAZIA;
    bool previa_gruta = false;                                          // Indica se a gruta foi aberta pelo F2 para testar os estados.
    const int teclas_gruta[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR}; // Uma tecla por estado para testar sem precisar ficar jogando milhares de vezes.
    const int ovos_teste[] = {0, 0, 1, 2};                              // Filhotes para cada teste.
    Partida partida;
    const Pergunta *pergunta_atual = NULL;
    Dificuldade dificuldade_em_falta = 0;
    bool faltam_perguntas = false;
    Resultado_resposta retorno_resposta;
    int retorno_retirada;

    // Botões.
    const Rectangle jogar = {48, 188, 280, 52};
    const Rectangle voltar = {24, 464, 200, 52};
    // Containers.
    const Rectangle mapa = {24, 88, 492, 325};
    const Rectangle quiz = {544, 88, 392, 325};

    // Elementos do quiz.
    const Rectangle acao_quiz = {quiz.x, voltar.y, 200, voltar.height}; // Botão de confirmar.
    int alternativa_selecionada = -1;
    bool resposta_confirmada = false;
    bool resposta_correta = false;

    // Retângulos, nomes e usos seguem a mesma ordem dos índices das ajudas.
    const Rectangle ajuda_rec[TOTAL_AJUDAS] = {
        {mapa.x + 16, mapa.y + 220, 148, 44},           // Recuo lateral de 16 px e linha de ajudas 220 px abaixo do topo.
        {mapa.x + 16 + 156, mapa.y + 220, 148, 44},     // Um passo horizontal de 156 px: largura de 148 px mais 8 px de espaço.
        {mapa.x + 16 + 2 * 156, mapa.y + 220, 148, 44}, // Dois passos horizontais a partir do recuo de 16 px, na mesma linha.
    };
    const char *nomes_ajuda[TOTAL_AJUDAS] = {"Burro", "Dragão", "Princesa"};
    int usos_ajuda[TOTAL_AJUDAS] = {0}; // Usos restantes em toda a partida.
    DicasPergunta dicas = {0};          // Efeitos que valem para a pergunta exibida.

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
    const int tamanho_titulo = 48;
    Font fonte_titulo = carregar_fonte_interface(tamanho_titulo);
    if (!fonte_carregada(fonte_titulo)) {
        TraceLog(LOG_ERROR, "Falha ao carregar a fonte do título");
        UnloadFont(fonte);
        CloseWindow();
        return;
    }

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

        // Converte as coordenadas do mouse para bater com a nova escala da janela.
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
            if ((partida.situacao == andamento) && pergunta_atual && !faltam_perguntas)
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
        // Verifica ações na tela do MENU.
        if (tela_atual == TELA_MENU) {
            if (janela_em_foco && IsKeyPressed(KEY_F2)) {
                previa_gruta = true;
                estado_gruta = GRUTA_VAZIA;
                tela_atual = TELA_GRUTA;
            }
            else if (confirmou || botao_clicado(jogar)) {
                dificuldade_em_falta = verifica_banco_para_partida(banco);
                if (dificuldade_em_falta == 0) { //verificando se não falta perguntas nas dificuldades
                    inicializa_partida(&partida);
                    pergunta_atual = sorteia_pergunta(banco, &partida);
                    faltam_perguntas = pergunta_atual == NULL;

                    // Define a quantidade de ajudas disponiveis para cada tipo de ajuda.
                    if (!faltam_perguntas) {
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
        // Cancelar preserva a pergunta aberta. Confirmar leva a recompensa dos marcos à gruta.
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            if (botao_clicado(voltar)) // Voltar nessa tela tem ação de cancelar
                tela_atual = TELA_JORNADA;
            else if (botao_clicado(acao_quiz)) {                // Confirma a retirada
                retorno_retirada = confirma_retirada(&partida); // Chama a função que recebe o checkpoint da retirada
                previa_gruta = false;
                if (retorno_retirada >= 0) { //confirmando se a retirada tática funcionou
                    if (retorno_retirada >= 10)
                        estado_gruta = GRUTA_OURO_MEDIO;
                    else if (retorno_retirada >= 5)
                        estado_gruta = GRUTA_OURO_BAIXO;
                    else
                        estado_gruta = GRUTA_VAZIA;
                    tela_atual = TELA_GRUTA;
                }
            }
        }
        // Ações do botão de voltar em cada tela.
        else if (botao_clicado(voltar)) {
            if (tela_atual == TELA_GRUTA || faltam_perguntas)
                tela_atual = TELA_MENU;
            else if (!resposta_confirmada)
                tela_atual = TELA_CONFIRMAR_RETIRADA;
            else if (partida.situacao == derrota || partida.situacao == vitoria) {
                // O jogador já leu o resultado. Só a vitória leva ouro para a gruta.
                estado_gruta = partida.situacao == vitoria ? GRUTA_OURO_ALTO : GRUTA_VAZIA;
                previa_gruta = false;
                tela_atual = TELA_GRUTA;
            }
        }
        else if (tela_atual == TELA_GRUTA) {
            // Cada tecla escolhe o estado com o mesmo índice no enum.
            for (int i = 0; i < 4; i++) {
                if (previa_gruta && janela_em_foco && IsKeyPressed(teclas_gruta[i])) {
                    estado_gruta = (EstadoGruta)i;
                    break;
                }
            }
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
        else if (alternativa_selecionada >= 0 && (confirmou || botao_clicado(acao_quiz) || repetiu_alternativa)) {
            // Confirma a resposta.
            if (!resposta_confirmada) {
                char resposta = 'A' + alternativa_selecionada; // Transforma o índice das alternativas em letras.
                resposta_confirmada = true;

                retorno_resposta = processa_resposta(&partida, pergunta_atual, resposta);

                if (retorno_resposta == resposta_acerto || retorno_resposta == resposta_marco || retorno_resposta == resposta_vitoria)
                    resposta_correta = true;
                else
                    resposta_correta = false;
            }
            // Só avança em uma nova ação, depois de mostrar o resultado da resposta.
            else if ((resposta_correta || retorno_resposta == resposta_renascimento) && partida.questao_atual < 15 && !faltam_perguntas) {
                const Pergunta *proxima = sorteia_pergunta(banco, &partida);
                if (proxima == NULL)
                    faltam_perguntas = true;
                else {
                    // Troca a pergunta só depois de um sorteio bem sucedido.
                    pergunta_atual = proxima;
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

        // A câmera converte as coordenadas lógicas em coordenadas da janela.
        Camera2D camera = {.offset = {area_exibida.x, area_exibida.y}, .zoom = escala};
        BeginDrawing();

        ClearBackground(BLACK);
        BeginScissorMode((int)area_exibida.x, (int)area_exibida.y, (int)area_exibida.width,
                         (int)area_exibida.height);
        BeginMode2D(camera);
        DrawRectangle(0, 0, LARGURA_BASE, ALTURA_BASE, RAYWHITE);

        if (tela_atual == TELA_MENU) {
            DrawTextEx(fonte_titulo, "Dragão do Milhão", (Vector2){48, 96}, tamanho_titulo, 1,
                       BLACK);
            desenhar_botao(fonte, jogar, "Jogar");

            if (dificuldade_em_falta != 0) {
                switch (dificuldade_em_falta) {
                    case muito_facil:
                        DrawTextEx(fonte, "O banco precisa de pelo menos 2 perguntas muito fáceis.", (Vector2){48, 260},
                                   tamanho_corpo, 1, MAROON);
                        break;

                    case facil:
                        DrawTextEx(fonte, "O banco precisa de pelo menos 2 perguntas fáceis.", (Vector2){48, 260},
                                   tamanho_corpo, 1, MAROON);
                        break;

                    case medio:
                        DrawTextEx(fonte, "O banco precisa de pelo menos 7 perguntas médias.", (Vector2){48, 260},
                                   tamanho_corpo, 1, MAROON);
                        break;

                    case dificil:
                        DrawTextEx(fonte, "O banco precisa de pelo menos 8 perguntas difíceis.", (Vector2){48, 260},
                                   tamanho_corpo, 1, MAROON);
                        break;

                    case muito_dificil:
                        DrawTextEx(fonte, "O banco precisa de pelo menos 9 perguntas muito difíceis.", (Vector2){48, 260},
                                   tamanho_corpo, 1, MAROON);
                        break;

                    default:
                        break;
                }
            }
            else if (faltam_perguntas)
                DrawTextEx(fonte, "Sem perguntas para iniciar.", (Vector2){48, 260},
                           tamanho_corpo, 1, MAROON);
        }
        else if (tela_atual == TELA_JORNADA) {
            const int questoes_jornada = 8;
            const int total_questoes = 15;
            bool em_batalha = partida.questao_atual > questoes_jornada;

            const char *nome_etapa = em_batalha ? "Batalha" : "Jornada";
            const char *titulo_quiz;
            if (resposta_confirmada && retorno_resposta == resposta_renascimento)
                titulo_quiz = TextFormat("Renascimento.");
            else
                titulo_quiz = TextFormat("%s - Questão %d de %d", nome_etapa, partida.questao_atual, total_questoes);
            DrawTextEx(fonte, titulo_quiz, (Vector2){24, 24}, tamanho_corpo, 1, BLACK);

            DrawRectangleRec(mapa, LIGHTGRAY);
            const char *titulo_cenario = em_batalha ? "Castelo do príncipe" : "Mapa aéreo";
            const Vector2 posicao_titulo = {mapa.x + margem, mapa.y + 24}; // Título 24 px abaixo do topo do painel.
            DrawTextEx(fonte, titulo_cenario, posicao_titulo, tamanho_corpo, 1, BLACK);

            // Na batalha, desconta os 8 acertos ja feitos na jornada.
            int acertos_etapa = em_batalha ? partida.acertos - questoes_jornada : partida.acertos;
            int total_etapa = em_batalha ? total_questoes - questoes_jornada : questoes_jornada;
            const char *progresso = TextFormat("%s: %d/%d", em_batalha ? "Golpes" : "Etapas", acertos_etapa, total_etapa);
            const Vector2 posicao_progresso = {mapa.x + margem, mapa.y + 108}; // Indicador de progresso 108 px abaixo do topo.
            DrawTextEx(fonte, progresso, posicao_progresso, tamanho_corpo, 1, BLACK);

            // Copia a posição e altura da barra e reduz apenas a largura do preenchimento dela.
            Rectangle fundo_vida = {mapa.x + margem, mapa.y + 142, mapa.width - 2.0f * margem, 20}; // Barra a 142 px do topo, entre as duas margens.
            Rectangle preenchimento_vida = fundo_vida;
            preenchimento_vida.width *= (float)acertos_etapa / total_etapa;

            // Barra de vida do boss ou progressão
            DrawRectangleRec(fundo_vida, RAYWHITE);
            DrawRectangleRec(preenchimento_vida, em_batalha ? MAROON : DARKGREEN);
            DrawRectangleLinesEx(fundo_vida, 2, DARKGRAY);

            // Mostra os usos restantes e apaga os botões indisponíveis.
            for (int i = 0; i < TOTAL_AJUDAS; i++) {
                const char *rotulo = TextFormat("%s (%d)", nomes_ajuda[i], usos_ajuda[i]);
                bool habilitado = ajuda_disponivel(i, usos_ajuda[i], &dicas, resposta_confirmada);
                desenhar_botao_com_estado(fonte, ajuda_rec[i], rotulo, false, habilitado);
            }

            // Mostra a revelação da Princesa quando ativa. Caso contrário, mostra o palpite do Burro.
            const char *texto_dica = NULL;
            if (dicas.resposta_revelada)
                texto_dica = TextFormat("Princesa: resposta %c.", pergunta_atual->resposta);
            else if (dicas.palpite_burro != '\0')
                texto_dica = TextFormat("Burro: apostaria na %c.", dicas.palpite_burro);
            if (texto_dica != NULL) {
                Vector2 posicao_dica = {mapa.x + margem, mapa.y + 276}; // Reserva a linha a 276 px do topo, abaixo dos botões.
                DrawTextEx(fonte, texto_dica, posicao_dica, tamanho_corpo, 1, BLACK);
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
                desenhar_botao_com_estado(fonte, alternativa_rec[i], rotulos[i], selecionado, habilitado);

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
                if (retorno_resposta == resposta_renascimento) //Caso em que o jogador erra a
                    mensagem = TextFormat("Você renascerá na questão %d.", partida.checkpoint + 1);
                else if (!resposta_correta)
                    mensagem = "Resposta incorreta.";
                else if (partida.questao_atual == 15)
                    mensagem = "Quiz concluído, a princesa foi salva.";
                else
                    mensagem = "Resposta correta!";
            }
            // Alternativa selecionada antes de confirmar
            else if (alternativa_selecionada >= 0)
                mensagem = TextFormat("Alternativa '%c' selecionada.", 'A' + alternativa_selecionada); // Em C, aspas simples representa um caractere e aspas duplas representa uma string, por isso deve ser 'A', e não "A".
            if (mensagem != NULL) {
                Vector2 posicao = {mapa.x + margem, mapa.y + 64.0f}; // Mensagem de seleção ou resultado, abaixo do título.
                DrawTextEx(fonte, mensagem, posicao, tamanho_corpo, 1.0f, BLACK);
            }

            bool pode_continuar = resposta_confirmada && ((retorno_resposta == resposta_renascimento) || resposta_correta) && partida.questao_atual < 15;
            if (alternativa_selecionada >= 0 && !faltam_perguntas && (!resposta_confirmada || pode_continuar)) {
                const char *rotulo = resposta_confirmada ? "Continuar" : "Confirmar";
                desenhar_botao(fonte, acao_quiz, rotulo);
            }

            if (faltam_perguntas)
                desenhar_botao(fonte, voltar, "Voltar");
            else if (partida.situacao == derrota || partida.situacao == vitoria)
                desenhar_botao(fonte, voltar, "Ver gruta");
            else
                // O !resposta_confimada significa que o botão desabilita após o usuário confirmar uma alternativa.
                desenhar_botao_com_estado(fonte, voltar, "Retirada Tática", false, !resposta_confirmada);
        }
        else if (tela_atual == TELA_CONFIRMAR_RETIRADA) {
            DrawTextEx(fonte_titulo, "Retirada Tática", (Vector2){48, 96}, tamanho_titulo, 1, BLACK);
            DrawTextEx(fonte, "Encerrar a tentativa e voltar à gruta?", (Vector2){48, 184}, tamanho_corpo, 1, BLACK);
            desenhar_botao(fonte, voltar, "Cancelar");
            desenhar_botao(fonte, acao_quiz, "Retirar");
        }
        else if (tela_atual == TELA_GRUTA) {
            int filhotes = previa_gruta ? ovos_teste[estado_gruta] : 0;
            // Uma retirada antes do primeiro marco deixa o dragão vivo no chão, sem pilha.
            bool mostrar_dragao = estado_gruta != GRUTA_VAZIA || (!previa_gruta && partida.situacao == retirada);
            desenhar_gruta(fonte, estado_gruta, filhotes, mostrar_dragao);
            desenhar_botao(fonte, voltar, "Voltar");

            const char *texto_gruta = "Prévia: teclas 1 a 4";
            if (!previa_gruta) {
                if (partida.situacao == vitoria)
                    texto_gruta = "Vitória! A princesa foi salva.";
                else if (partida.situacao == derrota)
                    texto_gruta = "Derrota! A gruta está vazia.";
                else
                    texto_gruta = TextFormat("Retirada: %d acertos", partida.acertos);
            }
            // Divide o espaço vertical livre para alinhar o texto ao botão Voltar.
            const Vector2 posicao_texto = {quiz.x, voltar.y + (voltar.height - tamanho_corpo) / 2.0f};
            DrawTextEx(fonte, texto_gruta, posicao_texto, tamanho_corpo, 1, BLACK);
        }

        EndMode2D();
        EndScissorMode();
        EndDrawing();
    }

    UnloadFont(fonte_titulo);
    UnloadFont(fonte);
    CloseWindow();
}
