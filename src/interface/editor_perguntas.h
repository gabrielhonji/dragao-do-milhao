/**
 * @file editor_perguntas.h
 * @brief Declara o formulário e a confirmação de exclusão do gerenciador visual.
 */

#ifndef EDITOR_PERGUNTAS_H
#define EDITOR_PERGUNTAS_H

#include <raylib.h>
#include "../perguntas/banco_perguntas.h"

typedef enum {
    EDITOR_FECHADO,
    EDITOR_NOVA,
    EDITOR_ALTERAR,
    EDITOR_EXCLUIR,
} ModoEditorPerguntas;

typedef struct {
    ModoEditorPerguntas modo;
    Pergunta pergunta;          // Rascunho independente da pergunta no banco.
    int campo_ativo;
    bool selecionar_texto;
    const char *erro;
} EditorPerguntas;

/**
 * @brief Abre uma edição com a pergunta selecionada ou com um rascunho vazio.
 * @param editor Estado da tela de edição.
 * @param modo Operação de cadastro, alteração ou exclusão.
 * @param pergunta Pergunta existente, ou NULL para cadastro.
 */
void abrir_editor_perguntas(EditorPerguntas *editor, ModoEditorPerguntas modo, const Pergunta *pergunta);

/**
 * @brief Processa o teclado e os botões da tela de edição.
 * @return true quando uma operação foi gravada e a listagem precisa ser atualizada.
 */
bool atualizar_editor_perguntas(EditorPerguntas *editor, Banco_perguntas *banco);

/** @brief Desenha o formulário ou a confirmação conforme o modo atual. */
void desenhar_editor_perguntas(const EditorPerguntas *editor, Font fonte, Font titulo);

#endif
