/**
 * @file tela_gerenciamento.c
 * @brief Exibe a lista de perguntas e coordena a busca, os filtros e o editor.
 */

#include <stdlib.h>
#include <string.h>
#include "tela_gerenciamento.h"
#include "../comum/audio.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

enum {PERGUNTAS_POR_PAGINA = 4};

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


void abrir_gerenciamento(TelaGerenciamento *tela, Rectangle voltar)
{
    *tela = (TelaGerenciamento){
        .id_selecionado = -1,
        .voltar = voltar,
        .campo_busca = {24, 76, 360, 44},
        .botao_dificuldade = {400, 76, 229, 44},
        .lista_perguntas = {24, 138, 912, 274},
        .pagina_anterior = {650, voltar.y, 136, voltar.height},
        .pagina_seguinte = {800, voltar.y, 136, voltar.height},
        .editar_pergunta = {246, voltar.y, 150, voltar.height},
        .remover_pergunta = {410, voltar.y, 150, voltar.height},
    };
    tela->nova_pergunta = (Rectangle){680, tela->campo_busca.y, 256, tela->campo_busca.height};
}

bool atualizar_gerenciamento(TelaGerenciamento *tela, Banco_perguntas *banco, bool janela_em_foco)
{
    // Enquanto o formulário estiver aberto, ele recebe os cliques e o teclado da tela.
    if (tela->editor.modo != EDITOR_FECHADO) {
        ModoEditorPerguntas modo = tela->editor.modo;
        if (atualizar_editor_perguntas(&tela->editor, banco)) {
            // A gravação pode mudar a quantidade e a ordem das linhas. Recomeça a lista.
            tela->pagina_perguntas = 0;
            tela->id_selecionado = -1;
            tela->retorno_edicao = modo == EDITOR_EXCLUIR ? "Pergunta excluída." : "Pergunta salva.";
            if (modo == EDITOR_NOVA) {
                // Limpa os filtros da busca para incluir a nova pergunta nos resultados.
                tela->busca_id[0] = '\0';
                tela->filtro_dificuldade = 0;
            }
        }
    }
    else if (botao_clicado(tela->voltar)) {
        return true;
    }
    else if (botao_clicado(tela->nova_pergunta)) {
        tela->busca_em_foco = false;
        tela->retorno_edicao = NULL;
        abrir_editor_perguntas(&tela->editor, EDITOR_NOVA, NULL);
        tocar_efeito(SOM_BOTAO);
    }
    else if (tela->id_selecionado >= 0 &&
             (botao_clicado(tela->editar_pergunta) || botao_clicado(tela->remover_pergunta))) {
        // Busca pelo ID selecionado. O índice do vetor pode mudar após outra operação.
        int indice = pesquisar_pergunta(banco, tela->id_selecionado);
        if (indice >= 0) {
            ModoEditorPerguntas modo = botao_clicado(tela->editar_pergunta) ? EDITOR_ALTERAR : EDITOR_EXCLUIR;
            tela->busca_em_foco = false;
            tela->retorno_edicao = NULL;
            abrir_editor_perguntas(&tela->editor, modo, &banco->perguntas[indice]);
            tocar_efeito(SOM_BOTAO);
        }
    }
    else {
        if (janela_em_foco && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            tela->busca_em_foco = botao_clicado(tela->campo_busca);

        // Só lê o teclado se o campo e a janela estiverem em foco.
        // A leitura devolve true quando um dígito foi escrito ou apagado.
        bool mudou_busca = tela->busca_em_foco && janela_em_foco && atualizar_busca_id(tela->busca_id, sizeof tela->busca_id);
        bool mudou_filtro = botao_clicado(tela->botao_dificuldade);

        if (mudou_filtro) {
            tela->filtro_dificuldade = tela->filtro_dificuldade == muito_dificil ? 0 : tela->filtro_dificuldade + 1;
            tocar_efeito(SOM_BOTAO);
        }

        if (mudou_busca || mudou_filtro) {
            tela->pagina_perguntas = 0;
            tela->id_selecionado = -1;
        }

        int indices[PERGUNTAS_POR_PAGINA];
        int total = coletar_pagina_perguntas(banco, tela->busca_id, tela->filtro_dificuldade, tela->pagina_perguntas, indices);
        int inicio = tela->pagina_perguntas * PERGUNTAS_POR_PAGINA;

        // Navegação das páginas.
        if (tela->pagina_perguntas > 0 && botao_clicado(tela->pagina_anterior)) {
            tela->pagina_perguntas--;
            tocar_efeito(SOM_BOTAO);
            tela->id_selecionado = -1;
        }
        else if (inicio + PERGUNTAS_POR_PAGINA < total && botao_clicado(tela->pagina_seguinte)) {
            tela->pagina_perguntas++;
            tocar_efeito(SOM_BOTAO);
            tela->id_selecionado = -1;
        }
        else {
            for (int linha = 0; linha < PERGUNTAS_POR_PAGINA && inicio + linha < total; linha++) {
                if (botao_clicado(faixa_pergunta(tela->lista_perguntas, linha))) {
                    // A posição no vetor serve para ler a pergunta. O ID identifica o registro escolhido.
                    tela->id_selecionado = banco->perguntas[indices[linha]].identificador;
                    tocar_efeito(SOM_BOTAO);
                    break;
                }
            }
        }
    }
    return false;
}

void desenhar_gerenciamento(const TelaGerenciamento *tela, const Banco_perguntas *banco,
                            Font fonte, Font fonte_titulo, const char *const nomes_dificuldade[])
{
    if (tela->editor.modo != EDITOR_FECHADO) {
        desenhar_editor_perguntas(&tela->editor, fonte, fonte_titulo);
        return;
    }

    const float x_id = tela->lista_perguntas.x + 24;
    const float x_enunciado = tela->lista_perguntas.x + 110;
    const float x_dificuldade = tela->lista_perguntas.x + 712;
    const float y_cabecalho = tela->lista_perguntas.y + 18;
    const float largura_enunciado = x_dificuldade - x_enunciado - 16; // Deixa 16 px antes da dificuldade.

    DrawTextEx(fonte_titulo, "Gerenciar perguntas", (Vector2){tela->lista_perguntas.x, 16}, 34, 1, COR_TEXTO_CLARO);
    Color fundo_busca = tela->busca_em_foco ? COR_FACE_SELECIONADA : COR_ESCURO;
    desenhar_painel(tela->campo_busca, fundo_busca);
    const char *rotulo_busca = tela->busca_id[0] == '\0' ? "Buscar por ID" : tela->busca_id;
    Vector2 posicao_busca = {tela->campo_busca.x + 12, tela->campo_busca.y + 13}; // Margem à esquerda e texto centralizado na altura.
    DrawTextEx(fonte, rotulo_busca, posicao_busca, 18, 1, COR_TEXTO_CLARO);

    const char *rotulo_filtro = tela->filtro_dificuldade == 0 ? "Dificuldade: todas" : nomes_dificuldade[tela->filtro_dificuldade];
    desenhar_botao(fonte, tela->botao_dificuldade, rotulo_filtro);
    desenhar_botao_com_estado(fonte, tela->nova_pergunta, "Nova pergunta", BOTAO_PRIMARIO, true);

    desenhar_painel(tela->lista_perguntas, COR_FACE_BOTAO);

    // Escreve os cabeçalhos.
    DrawTextEx(fonte, "ID", (Vector2){x_id, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, "Enunciado", (Vector2){x_enunciado, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, "Dificuldade", (Vector2){x_dificuldade, y_cabecalho}, 18, 1, COR_TEXTO_CLARO);

    int indices[PERGUNTAS_POR_PAGINA];
    int total = coletar_pagina_perguntas(banco, tela->busca_id, tela->filtro_dificuldade, tela->pagina_perguntas, indices);
    int inicio = tela->pagina_perguntas * PERGUNTAS_POR_PAGINA;

    // Desenha os mesmos resultados e retângulos que a lógica de clique usa.
    for (int linha = 0; linha < PERGUNTAS_POR_PAGINA && inicio + linha < total; linha++) {
        const Pergunta *pergunta = &banco->perguntas[indices[linha]];
        Rectangle faixa = faixa_pergunta(tela->lista_perguntas, linha);
        float y_texto = faixa.y + 12; // Recuo vertical do texto dentro da faixa.

        bool selecionada = pergunta->identificador == tela->id_selecionado;
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
        DrawTextEx(fonte, mensagem_lista, (Vector2){x_id, faixa_pergunta(tela->lista_perguntas, 0).y}, 18, 1, COR_TEXTO_CLARO);
    }

    bool tem_anterior = tela->pagina_perguntas > 0;
    bool tem_proxima = inicio + PERGUNTAS_POR_PAGINA < total;
    float y_pagina = tela->lista_perguntas.y + tela->lista_perguntas.height + 16; // Indicador abaixo do painel da lista.

    DrawTextEx(fonte, TextFormat("Página %d", tela->pagina_perguntas + 1), (Vector2){tela->lista_perguntas.x, y_pagina}, 18, 1, COR_TEXTO_CLARO);
    if (tela->retorno_edicao != NULL)
        DrawTextEx(fonte, tela->retorno_edicao, (Vector2){180, y_pagina}, 18, 1, COR_TEXTO_CLARO);
    desenhar_botao(fonte, tela->voltar, "Voltar");
    desenhar_botao_com_estado(fonte, tela->editar_pergunta, "Editar", BOTAO_SECUNDARIO, tela->id_selecionado >= 0);
    desenhar_botao_com_estado(fonte, tela->remover_pergunta, "Excluir", BOTAO_SAIDA, tela->id_selecionado >= 0);
    desenhar_botao_com_estado(fonte, tela->pagina_anterior, "Anterior", BOTAO_SECUNDARIO, tem_anterior);
    desenhar_botao_com_estado(fonte, tela->pagina_seguinte, "Próxima", BOTAO_SECUNDARIO, tem_proxima);
}
