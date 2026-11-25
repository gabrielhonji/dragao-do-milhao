/**
 * @file tela_gerenciamento.h
 * @brief Exibe a lista de perguntas e coordena a busca, os filtros e o editor.
 */

#ifndef TELA_GERENCIAMENTO_H
#define TELA_GERENCIAMENTO_H

#include <raylib.h>
#include "editor_perguntas.h"

typedef struct
{
    int pagina_perguntas;
    int id_selecionado; // -1 indica que nenhuma pergunta está selecionada.
    int filtro_dificuldade; // 0 mostra todas as dificuldades.
    char busca_id[5]; // Quatro dígitos e o '\0'.
    bool busca_em_foco;
    EditorPerguntas editor;
    const char *retorno_edicao;

    Rectangle voltar;
    Rectangle campo_busca;
    Rectangle botao_dificuldade;
    Rectangle lista_perguntas;
    Rectangle pagina_anterior;
    Rectangle pagina_seguinte;
    Rectangle nova_pergunta;
    Rectangle editar_pergunta;
    Rectangle remover_pergunta;
} TelaGerenciamento;

/** @brief Inicia a tela com a lista sem filtros e configura as áreas clicáveis. */
void abrir_gerenciamento(TelaGerenciamento *tela, Rectangle voltar);

/** @return Verdadeiro quando o jogador pede para voltar ao menu. */
bool atualizar_gerenciamento(TelaGerenciamento *tela, Banco_perguntas *banco, bool janela_em_foco);

/** @brief Desenha o editor ou a lista com os filtros e paginação. */
void desenhar_gerenciamento(const TelaGerenciamento *tela, const Banco_perguntas *banco,
                            Font fonte, Font fonte_titulo, const char *const nomes_dificuldade[]);

#endif
