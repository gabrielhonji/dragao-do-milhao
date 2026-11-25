/**
 * @file tela_registro.c
 * @brief Edita o nome e grava o resultado da tentativa.
 */

#include <string.h>
#include "tela_registro.h"
#include "../../quiz/arquivo_partidas.h"
#include "../comum/componentes.h"
#include "../comum/cores.h"

bool atualizar_registro(TelaRegistro *tela, const Partida *partida, Rectangle salvar,
                       bool janela_em_foco, bool confirmou)
{
    // Lê a digitação, o Backspace e o Ctrl+A do campo de nome.
    if (janela_em_foco)
        editar_texto(tela->nome, sizeof tela->nome, &tela->selecionar_nome);

    if (confirmou || botao_clicado(salvar)) {
        // Pula espaços, tabs e quebras de linha. Chegar ao '\0' indica que o nome está em branco.
        if (tela->nome[strspn(tela->nome, " \t\r\n")] == '\0') {
            tela->erro = "Digite um nome para registrar o resultado.";
        }
        else {
            Registro_partida registro = {0};
            FILE *arquivo_ranking = NULL;

            // As duas funções retornam 0 no sucesso e um valor negativo na falha.
            if (preenche_registro_partida(&registro, partida, tela->nome) != 0)
                tela->erro = "Não foi possível preparar o resultado.";
            else if (salvar_registro_partida(&registro, &arquivo_ranking) != 0)
                tela->erro = "Falha ao salvar. Tente novamente.";
            else {
                // Sair do formulário após gravar impede salvar a mesma tentativa duas vezes.
                tela->erro = NULL;
                return true;
            }
        }
    }
    return false;
}

void desenhar_registro(const TelaRegistro *tela, Font fonte, Font fonte_titulo, Rectangle salvar)
{
    const Rectangle campo_nome = {160, 180, 640, 48};
    const Vector2 posicao_rotulo = {campo_nome.x, campo_nome.y - 40}; // Deixa o rótulo acima da caixa de texto.

    DrawTextEx(fonte_titulo, "Registrar resultado", (Vector2){24, 24}, 34, 1, COR_TEXTO_CLARO);
    DrawTextEx(fonte, "A lenda vai se lembrar de você como...", posicao_rotulo, 24, 1, COR_TEXTO_CLARO);
    desenhar_campo(fonte, campo_nome, tela->nome, true, tela->selecionar_nome);
    desenhar_botao_com_estado(fonte, salvar, "Salvar", BOTAO_PRIMARIO, true);

    if (tela->erro != NULL) {
        const Vector2 posicao_erro = {campo_nome.x, campo_nome.y + campo_nome.height + 12}; // Deixa 12 px abaixo do campo.
        DrawTextEx(fonte, tela->erro, posicao_erro, 20, 1, COR_AVISO);
    }
}
