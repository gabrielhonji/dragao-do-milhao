/**
 * @file editor_perguntas.h
 * @brief Declara o formulário e a confirmação de exclusão do gerenciador visual.
 */

#ifndef EDITOR_PERGUNTAS_H
#define EDITOR_PERGUNTAS_H

#include <raylib.h>
#include "../../perguntas/banco_perguntas.h"

typedef enum
{
    EDITOR_FECHADO,
    EDITOR_NOVA,
    EDITOR_ALTERAR,
    EDITOR_EXCLUIR,
} ModoEditorPerguntas;

typedef struct
{
    ModoEditorPerguntas modo; // Diz qual formulário ou confirmação está aberta.
    Pergunta pergunta;        // Rascunho independente da pergunta no banco.
    int campo_ativo;          // 0 é o enunciado. De 1 a 4 são as alternativas A a D.
    bool selecionar_texto;    // Após Ctrl+A, a próxima digitação substitui o campo.
    const char *erro;         // Mensagem exibida no formulário, ou NULL.
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
 * @param editor Formulário aberto e seus dados em edição.
 * @param banco Banco atualizado após uma gravação bem-sucedida.
 * @return true quando uma operação foi gravada e a listagem precisa ser atualizada.
 */
bool atualizar_editor_perguntas(EditorPerguntas *editor, Banco_perguntas *banco);

/**
 * @brief Desenha o formulário ou a confirmação conforme o modo atual.
 * @param editor Estado do formulário, com o rascunho e a mensagem de erro.
 * @param fonte Fonte dos campos, rótulos e botões.
 * @param titulo Fonte do título do formulário.
 */
void desenhar_editor_perguntas(const EditorPerguntas *editor, Font fonte, Font titulo);

#endif
