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
#include "editor_perguntas.h"
#include "../quiz/quiz.h"
#include "../quiz/arquivo_partidas.h"
#include "tela_menu.h"
#include "tela_creditos.h"
#include "tela_ranking.h"

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
    PERGUNTAS_POR_PAGINA = 4,
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

/**
 * @brief Reúne os índices das perguntas que aparecem na página filtrada.
 *
 * @param banco Banco de perguntas consultado.
 * @param busca ID digitado, ou string vazia para exibir qualquer ID.
 * @param dificuldade Dificuldade desejada, ou 0 para exibir todas.
 * @param pagina Índice da página, começando em zero.
 * @param indices Vetor preenchido com os índices das perguntas visíveis.
 * @return Total de perguntas que passam pela busca e pelo filtro.
 */
static int coletar_pagina_perguntas(const Banco_perguntas *banco, const char *busca, int dificuldade, int pagina, int indices[PERGUNTAS_POR_PAGINA])
{
    const bool busca_ativa = busca[0] != '\0'; // Só ativa a busca se tiver algo escrito no input.
    // atoi converte os dígitos do campo em int, então "007" e "7" buscam o mesmo ID.
    const int id_procurado = atoi(busca);
    const int inicio = pagina * PERGUNTAS_POR_PAGINA;
    int total = 0;

    for (int i = 0; i < banco->quant_perguntas; i++) {
        const Pergunta *pergunta = &banco->perguntas[i];
        // Com a busca preenchida, só passam perguntas cujo ID é igual ao digitado.
        if (busca_ativa && pergunta->identificador != id_procurado)
            continue;
        // O filtro 0 aceita todas. Qualquer outro valor exige que a dificuldade coincida.
        if (dificuldade != 0 && (int)pergunta->dificuldade != dificuldade)
            continue;

        // total é o índice deste resultado na lista filtrada, antes de contar a pergunta atual.
        int linha = total - inicio;
        if (linha >= 0 && linha < PERGUNTAS_POR_PAGINA)
            indices[linha] = i;

        total++;
    }

    return total;
}
/**
 * @brief Calcula a área usada para desenhar e clicar em uma linha.
 * @param painel Área que contém a listagem.
 * @param linha Posição na página, começando em zero.
 * @return Retângulo da linha na tela lógica.
 */
static Rectangle faixa_pergunta(Rectangle painel, int linha)
{
    const float recuo = 12;    // Margem esquerda e direita das faixas.
    const float topo = 49;     // Espaço entre o topo do painel e a primeira faixa.
    const float altura = 43;   // Altura de uma faixa.
    const float intervalo = 6; // Espaço livre entre duas faixas.

    return (Rectangle){painel.x + recuo, painel.y + topo + linha * (altura + intervalo),
                       painel.width - 2 * recuo, altura};
}

/**
 * @brief Lê dígitos e Backspace para atualizar o campo de busca.
 * @param busca Texto editável do campo.
 * @param capacidade Tamanho do vetor, incluindo espaço para o '\0'.
 * @return Verdadeiro se o conteúdo foi alterado.
 */
static bool atualizar_busca_id(char *busca, size_t capacidade)
{
    size_t tamanho = strlen(busca); // Usa o tipo retornado por strlen e também usado por sizeof.
    bool mudou = false;

    if (tamanho > 0 && IsKeyPressed(KEY_BACKSPACE)) {
        busca[--tamanho] = '\0';
        mudou = true;
    }

    // GetCharPressed lê o próximo caractere digitado. O laço consome a fila até receber 0.
    for (int tecla = GetCharPressed(); tecla != 0; tecla = GetCharPressed()) {
        if (tecla < '0' || tecla > '9' || tamanho + 1 >= capacidade)
            continue;

        busca[tamanho] = (char)tecla;
        tamanho++;             // Avança para a posição onde vai ficar o terminador.
        busca[tamanho] = '\0'; // Mantém a string terminada após cada dígito.
        mudou = true;
    }

    return mudou;
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
    DrawTextEx(fonte, "Gruta", (Vector2){24, 24}, 24, 1, BLACK);
    desenhar_arte_gruta(estado, ovos_restantes, mostrar_dragao);
    if (!mostrar_dragao)
        DrawTextEx(fonte, "A gruta está vazia.", (Vector2){48, 104}, 24, 1, RAYWHITE);
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
    char nome_ranking[41] = "";                // Até 40 bytes de nome e o '\0'.
    bool selecionar_nome = false;
    const char *erro_ranking = NULL;

    // Resultado e prévia da gruta.
    EstadoGruta estado_gruta = GRUTA_VAZIA;
    bool previa_gruta = false; // Indica se a gruta foi aberta pelo F2 sem uma partida.
    const int teclas_gruta[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR}; // Uma tecla para cada estado da prévia.
    const int ovos_teste[] = {0, 0, 1, 2}; // Quantidade de filhotes para cada estado da prévia.

    // Elementos da tela de gerenciamento de perguntas.
    int pagina_perguntas = 0;
    int id_selecionado = -1;    // -1 indica que nenhuma pergunta foi selecionada.
    int filtro_dificuldade = 0; // 0 mostra todas. As dificuldades válidas vão de 1 a 5.
    char busca_id[5] = "";      // Quatro dígitos do ID e o '\0' no final.
    bool busca_em_foco = false; // O campo só recebe texto depois de um clique nele.
    EditorPerguntas editor = {0};
    const char *retorno_edicao = NULL;

    const Rectangle campo_busca = {24, 76, 360, 44};        // Área da busca no Figma.
    const Rectangle botao_dificuldade = {400, 76, 229, 44}; // Área do filtro no Figma.
    const Rectangle lista_perguntas = {24, 138, 912, 274};  // Painel da lista no Figma.
    const Rectangle pagina_anterior = {650, voltar.y, 136, voltar.height};
    const Rectangle pagina_seguinte = {800, voltar.y, 136, voltar.height};
    const Rectangle nova_pergunta = {680, campo_busca.y, 256, campo_busca.height};
    const Rectangle editar_pergunta = {246, voltar.y, 150, voltar.height};
    const Rectangle remover_pergunta = {410, voltar.y, 150, voltar.height};

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
                previa_gruta = true;
                estado_gruta = GRUTA_VAZIA;
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
                pagina_perguntas = 0;
                id_selecionado = -1;
                filtro_dificuldade = 0;
                busca_id[0] = '\0';
                busca_em_foco = false;
                retorno_edicao = NULL;
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
                    nome_ranking[0] = '\0';
                    selecionar_nome = false;
                    erro_ranking = NULL;
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
                    // O ouro corresponde ao último marco conquistado na partida.
                    if (marco == 10)
                        estado_gruta = GRUTA_OURO_MEDIO;
                    else if (marco == 5)
                        estado_gruta = GRUTA_OURO_BAIXO;
                    else
                        estado_gruta = GRUTA_VAZIA;
                    previa_gruta = false;
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
            // Enquanto o formulário estiver aberto, ele recebe os cliques e o teclado da tela.
            if (editor.modo != EDITOR_FECHADO) {
                ModoEditorPerguntas modo = editor.modo;
                if (atualizar_editor_perguntas(&editor, banco)) {
                    // A gravação pode mudar a quantidade e a ordem das linhas. Recomeça a lista.
                    pagina_perguntas = 0;
                    id_selecionado = -1;
                    retorno_edicao = modo == EDITOR_EXCLUIR ? "Pergunta excluída." : "Pergunta salva.";
                    if (modo == EDITOR_NOVA) {
                        // Limpa os filtros da busca para incluir a nova pergunta nos resultados.
                        busca_id[0] = '\0';
                        filtro_dificuldade = 0;
                    }
                }
            }
            else if (botao_clicado(voltar)) {
                tela_atual = TELA_MENU;
            }
            else if (botao_clicado(nova_pergunta)) {
                busca_em_foco = false;
                retorno_edicao = NULL;
                abrir_editor_perguntas(&editor, EDITOR_NOVA, NULL);
            }
            else if (id_selecionado >= 0 &&
                     (botao_clicado(editar_pergunta) || botao_clicado(remover_pergunta))) {
                // Busca pelo ID selecionado. O índice do vetor pode mudar após outra operação.
                int indice = pesquisar_pergunta(banco, id_selecionado);
                if (indice >= 0) {
                    ModoEditorPerguntas modo = botao_clicado(editar_pergunta) ? EDITOR_ALTERAR : EDITOR_EXCLUIR;
                    busca_em_foco = false;
                    retorno_edicao = NULL;
                    abrir_editor_perguntas(&editor, modo, &banco->perguntas[indice]);
                }
            }
            else {
                if (janela_em_foco && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    busca_em_foco = botao_clicado(campo_busca);

                // Só lê o teclado se o campo e a janela estiverem em foco.
                // A leitura devolve true quando um dígito foi escrito ou apagado.
                bool mudou_busca = busca_em_foco && janela_em_foco && atualizar_busca_id(busca_id, sizeof busca_id);
                bool mudou_filtro = botao_clicado(botao_dificuldade);

                if (mudou_filtro)
                    filtro_dificuldade = filtro_dificuldade == muito_dificil ? 0 : filtro_dificuldade + 1;

                if (mudou_busca || mudou_filtro) {
                    pagina_perguntas = 0;
                    id_selecionado = -1;
                }

                int indices[PERGUNTAS_POR_PAGINA];
                int total = coletar_pagina_perguntas(banco, busca_id, filtro_dificuldade, pagina_perguntas, indices);
                int inicio = pagina_perguntas * PERGUNTAS_POR_PAGINA;

                // Navegação das páginas.
                if (pagina_perguntas > 0 && botao_clicado(pagina_anterior)) {
                    pagina_perguntas--;
                    id_selecionado = -1;
                }
                else if (inicio + PERGUNTAS_POR_PAGINA < total && botao_clicado(pagina_seguinte)) {
                    pagina_perguntas++;
                    id_selecionado = -1;
                }
                else {
                    for (int linha = 0; linha < PERGUNTAS_POR_PAGINA && inicio + linha < total; linha++) {
                        if (botao_clicado(faixa_pergunta(lista_perguntas, linha))) {
                            // A posição no vetor serve para ler a pergunta. O ID identifica o registro escolhido.
                            id_selecionado = banco->perguntas[indices[linha]].identificador;
                            break;
                        }
                    }
                }
            }
        }
        else if (tela_atual == TELA_REGISTRAR_RESULTADO) {
            // Lê a digitação, o Backspace e o Ctrl+A do campo de nome.
            if (janela_em_foco)
                editar_texto(nome_ranking, sizeof nome_ranking, &selecionar_nome);

            if (confirmou || botao_clicado(salvar_ranking)) {
                // Pula espaços, tabs e quebras de linha. Chegar ao '\0' indica que o nome está em branco.
                if (nome_ranking[strspn(nome_ranking, " \t\r\n")] == '\0') {
                    erro_ranking = "Digite um nome para registrar o resultado.";
                }
                else {
                    Registro_partida registro = {0};
                    FILE *arquivo_ranking = NULL;

                    // As duas funções retornam 0 no sucesso e um valor negativo na falha.
                    if (preenche_registro_partida(&registro, &partida, nome_ranking) != 0)
                        erro_ranking = "Não foi possível preparar o resultado.";
                    else if (salvar_registro_partida(&registro, &arquivo_ranking) != 0)
                        erro_ranking = "Falha ao salvar. Tente novamente.";
                    else {
                        // Sair do formulário após gravar impede salvar a mesma tentativa duas vezes.
                        erro_ranking = NULL;
                        tela_atual = TELA_GRUTA;
                    }
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
                tela_atual = partida_entra_no_ranking(&partida) ? TELA_REGISTRAR_RESULTADO : TELA_GRUTA;
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
        else if (tela_atual == TELA_GERENCIAMENTO && editor.modo != EDITOR_FECHADO) {
            desenhar_editor_perguntas(&editor, fonte, fonte_titulo);
        }
        else if (tela_atual == TELA_GERENCIAMENTO) {
            const float x_id = lista_perguntas.x + 24;
            const float x_enunciado = lista_perguntas.x + 110;
            const float x_dificuldade = lista_perguntas.x + 712;
            const float y_cabecalho = lista_perguntas.y + 18;
            const float largura_enunciado = x_dificuldade - x_enunciado - 16; // Deixa 16 px antes da dificuldade.

            DrawTextEx(fonte_titulo, "Gerenciar perguntas", (Vector2){lista_perguntas.x, 13}, 34, 1, COR_TEXTO_CLARO);
            Color fundo_busca = busca_em_foco ? COR_FACE_SELECIONADA : COR_ESCURO;
            desenhar_painel(campo_busca, fundo_busca);
            const char *rotulo_busca = busca_id[0] == '\0' ? "Buscar por ID" : busca_id;
            Vector2 posicao_busca = {campo_busca.x + 12, campo_busca.y + 13}; // Margem à esquerda e texto centralizado na altura.
            DrawTextEx(fonte, rotulo_busca, posicao_busca, 18, 1, COR_TEXTO_CLARO);

            const char *rotulo_filtro = filtro_dificuldade == 0 ? "Dificuldade: todas" : nomes_dificuldade[filtro_dificuldade];
            desenhar_botao(fonte, botao_dificuldade, rotulo_filtro);
            desenhar_botao_com_estado(fonte, nova_pergunta, "Nova pergunta", BOTAO_PRIMARIO, true);

            desenhar_painel(lista_perguntas, COR_FACE_BOTAO);

            // Escreve os cabeçalhos.
            DrawTextEx(fonte, "ID", (Vector2){x_id, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
            DrawTextEx(fonte, "Enunciado", (Vector2){x_enunciado, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
            DrawTextEx(fonte, "Dificuldade", (Vector2){x_dificuldade, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);

            int indices[PERGUNTAS_POR_PAGINA];
            int total = coletar_pagina_perguntas(banco, busca_id, filtro_dificuldade, pagina_perguntas, indices);
            int inicio = pagina_perguntas * PERGUNTAS_POR_PAGINA;

            // Desenha os mesmos resultados e retângulos que a lógica de clique usa.
            for (int linha = 0; linha < PERGUNTAS_POR_PAGINA && inicio + linha < total; linha++) {
                const Pergunta *pergunta = &banco->perguntas[indices[linha]];
                Rectangle faixa = faixa_pergunta(lista_perguntas, linha);
                float y_texto = faixa.y + 12; // Recuo vertical do texto dentro da faixa.

                bool selecionada = pergunta->identificador == id_selecionado;
                DrawRectangleRec(faixa, selecionada ? COR_FACE_SELECIONADA : COR_ESCURO);
                if (selecionada)
                    DrawRectangleLinesEx(faixa, 2, COR_BORDA_SELECIONADA); // Contorno de 2 px para destacar a escolha.

                int dificuldade = pergunta->dificuldade;
                if (dificuldade < muito_facil || dificuldade > muito_dificil)
                    dificuldade = 0; // Usa o rótulo "Desconhecida" para valores fora da escala.

                // %03d exibe o ID com pelo menos três dígitos, preenchendo com zeros à esquerda.
                DrawTextEx(fonte, TextFormat("%03d", pergunta->identificador), (Vector2){x_id, y_texto}, 18, 1, COR_TEXTO_CLARO);
                desenhar_previa_texto(fonte, pergunta->enunciado, (Vector2){x_enunciado, y_texto}, largura_enunciado);
                DrawTextEx(fonte, nomes_dificuldade[dificuldade], (Vector2){x_dificuldade, y_texto}, 18, 1, COR_TEXTO_CLARO);
            }

            if (total == 0) {
                const char *mensagem_lista = banco->quant_perguntas == 0 ? "Nenhuma pergunta cadastrada." : "Nenhuma pergunta encontrada.";
                DrawTextEx(fonte, mensagem_lista, (Vector2){x_id, faixa_pergunta(lista_perguntas, 0).y}, 18, 1, COR_TEXTO_CLARO);
            }

            bool tem_anterior = pagina_perguntas > 0;
            bool tem_proxima = inicio + PERGUNTAS_POR_PAGINA < total;
            float y_pagina = lista_perguntas.y + lista_perguntas.height + 16; // Indicador abaixo do painel da lista.

            DrawTextEx(fonte, TextFormat("Página %d", pagina_perguntas + 1), (Vector2){lista_perguntas.x, y_pagina}, 18, 1, COR_TEXTO_CLARO);
            if (retorno_edicao != NULL)
                DrawTextEx(fonte, retorno_edicao, (Vector2){180, y_pagina}, 18, 1, COR_TEXTO_CLARO);
            desenhar_botao(fonte, voltar, "Voltar");
            desenhar_botao_com_estado(fonte, editar_pergunta, "Editar", BOTAO_SECUNDARIO, id_selecionado >= 0);
            desenhar_botao_com_estado(fonte, remover_pergunta, "Excluir", BOTAO_SECUNDARIO, id_selecionado >= 0);
            desenhar_botao_com_estado(fonte, pagina_anterior, "Anterior", BOTAO_SECUNDARIO, tem_anterior);
            desenhar_botao_com_estado(fonte, pagina_seguinte, "Próxima", BOTAO_SECUNDARIO, tem_proxima);
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
            const Rectangle campo_nome = {160, 180, 640, 48};
            const Vector2 posicao_rotulo = {campo_nome.x, campo_nome.y - 40}; // Deixa o rótulo acima da caixa de texto.

            DrawTextEx(fonte_titulo, "Registrar resultado", (Vector2){24, 24}, 34, 1, COR_TEXTO_CLARO);
            DrawTextEx(fonte, "A lenda vai se lembrar de você como...", posicao_rotulo, 24, 1, COR_TEXTO_CLARO);
            desenhar_campo(fonte, campo_nome, nome_ranking, true, selecionar_nome);
            desenhar_botao_com_estado(fonte, salvar_ranking, "Salvar", BOTAO_PRIMARIO, true);

            if (erro_ranking != NULL) {
                const Vector2 posicao_erro = {campo_nome.x, campo_nome.y + campo_nome.height + 12}; // Deixa 12 px abaixo do campo.
                DrawTextEx(fonte, erro_ranking, posicao_erro, 20, 1, COR_AVISO);
            }
        }
        else if (tela_atual == TELA_GRUTA) {
            int filhotes = previa_gruta ? ovos_teste[estado_gruta] : partida.ovos_restantes;
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
                else if (partida.checkpoint > 0)
                    texto_gruta = TextFormat("Retirada no marco %d.", partida.checkpoint);
                else
                    texto_gruta = "Retirada antes do primeiro marco.";
            }
            // Divide o espaço vertical livre para alinhar o texto ao botão Voltar.
            const Vector2 posicao_texto = {quiz.x, voltar.y + (voltar.height - tamanho_corpo) / 2.0f};
            DrawTextEx(fonte, texto_gruta, posicao_texto, tamanho_corpo, 1, BLACK);
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
