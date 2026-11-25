/**
 * @file tela_quiz.h
 * @brief Atualiza e desenha as perguntas da Jornada e da Batalha.
 */

#ifndef TELA_QUIZ_H
#define TELA_QUIZ_H

#include <raylib.h>
#include "../quiz/quiz.h"

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


typedef struct
{
    Partida partida;
    const Pergunta *pergunta_atual;
    int questao_exibida; // Mantém o número na tela até o próximo sorteio.
    bool faltam_perguntas;
    int alternativa_selecionada;
    bool resposta_confirmada;
    Resultado_resposta resultado_resposta;
    int usos_ajuda[TOTAL_AJUDAS];
    DicasPergunta dicas;
} TelaQuiz;

typedef struct
{
    Rectangle mapa;
    Rectangle painel;
    Rectangle voltar;
    Rectangle acao_quiz;
    float y_ajudas;
    Rectangle ajuda_rec[TOTAL_AJUDAS];
    Rectangle enunciado_quiz;
    Rectangle alternativa_rec[4];
} LayoutQuiz;

typedef enum
{
    QUIZ_SEM_ACAO,
    QUIZ_VOLTAR_MENU,
    QUIZ_PEDIR_RETIRADA,
    QUIZ_ABRIR_SOPRO,
    QUIZ_MOSTRAR_RESULTADO,
} AcaoQuiz;

/** @brief Monta as áreas usadas tanto pelos cliques quanto pelo desenho. */
LayoutQuiz montar_layout_quiz(Rectangle mapa, Rectangle painel, Rectangle voltar);

/** @return Verdadeiro quando a primeira pergunta foi sorteada e a partida pode ser exibida. */
bool iniciar_quiz(TelaQuiz *tela, const Banco_perguntas *banco);

/** @brief Atualiza cronômetro, ajudas, seleção e respostas e informa se é preciso trocar de tela. */
AcaoQuiz atualizar_quiz(TelaQuiz *tela, const LayoutQuiz *layout, const Banco_perguntas *banco, bool confirmou);

/**
 * @brief Aplica a ajuda liberada à pergunta atual.
 * @param tela Quiz com a pergunta, as dicas e a alternativa selecionada.
 * @param ajuda Índice do botão cujo efeito foi liberado.
 */
void aplicar_efeito_ajuda_quiz(TelaQuiz *tela, int ajuda);

/** @brief Desenha a cena, os indicadores da partida e a pergunta atual. */
void desenhar_quiz(const TelaQuiz *tela, const LayoutQuiz *layout, Font fonte, int tamanho_corpo);

#endif
