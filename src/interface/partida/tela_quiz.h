/**
 * @file tela_quiz.h
 * @brief Atualiza e desenha as perguntas da Jornada e da Batalha.
 */

#ifndef TELA_QUIZ_H
#define TELA_QUIZ_H

#include <raylib.h>
#include "../../quiz/quiz.h"

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
    float tempo_efeito; // Tempo da transição após um acerto, fora do relógio do ranking.
    int dano_batalha;    // PV perdidos na resposta atual, para exibir durante o impacto.
    int usos_ajuda[TOTAL_AJUDAS]; // Usos restantes de cada ajuda nesta partida.
    DicasPergunta dicas;
} TelaQuiz;

typedef struct
{
    Rectangle mapa;
    Rectangle painel;
    Rectangle retirada;
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
    QUIZ_ABRIR_BURRO,
    QUIZ_ABRIR_ECO,
    QUIZ_MOSTRAR_RESULTADO,
    QUIZ_CHEGOU_CASTELO,
} AcaoQuiz;

/**
 * @brief Monta as áreas usadas tanto pelos cliques quanto pelo desenho.
 * @param mapa Painel usado pelo cenário e como referência para as ajudas.
 * @param painel Área reservada ao enunciado e às alternativas.
 * @param rodape Retângulo de referência para a posição vertical e a altura dos botões.
 * @return Áreas do quiz na resolução lógica da interface.
 */
LayoutQuiz montar_layout_quiz(Rectangle mapa, Rectangle painel, Rectangle rodape);

/**
 * @brief Inicia uma partida, sorteia a primeira pergunta e prepara os usos das ajudas.
 * @param tela Estado do quiz que receberá a nova partida.
 * @param banco Banco consultado no sorteio inicial.
 * @return Verdadeiro quando a primeira pergunta foi sorteada e a partida pode ser exibida.
 */
bool iniciar_quiz(TelaQuiz *tela, const Banco_perguntas *banco);

/**
 * @brief Atualiza cronômetro, ajudas, seleção e respostas e informa se é preciso trocar de tela.
 * @param tela Estado do quiz que será atualizado neste quadro.
 * @param layout Áreas usadas para detectar cliques.
 * @param banco Banco consultado ao sortear a próxima pergunta.
 * @param confirmou Indica se Enter foi pressionado neste quadro.
 * @return Ação para o controlador das telas ou QUIZ_SEM_ACAO quando deve permanecer no quiz.
 */
AcaoQuiz atualizar_quiz(TelaQuiz *tela, const LayoutQuiz *layout, const Banco_perguntas *banco, bool confirmou);

/**
 * @brief Aplica a ajuda liberada à pergunta atual.
 * @param tela Quiz com a pergunta, as dicas e a alternativa selecionada.
 * @param ajuda Índice do botão cujo efeito foi liberado.
 */
void aplicar_efeito_ajuda_quiz(TelaQuiz *tela, int ajuda);

/**
 * @brief Indica se a pergunta exibida pertence à Batalha.
 * @param tela Quiz com o número da questão mostrado ao jogador.
 * @return Verdadeiro a partir da primeira pergunta no castelo.
 */
bool quiz_em_batalha(const TelaQuiz *tela);

/**
 * @brief Desenha a cena, os indicadores da partida e a pergunta atual.
 * @param tela Estado exibido do quiz.
 * @param layout Áreas do cenário, das ajudas e da pergunta.
 * @param fonte Fonte dos textos do quiz.
 * @param tamanho_corpo Tamanho preferido do texto em pixels lógicos.
 */
void desenhar_quiz(const TelaQuiz *tela, const LayoutQuiz *layout, Font fonte, int tamanho_corpo);

#endif
