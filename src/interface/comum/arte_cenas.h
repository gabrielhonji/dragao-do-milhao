/**
 * @file arte_cenas.h
 * @brief Declara o carregamento e o desenho dos cenários do jogo.
 */

#ifndef ARTE_CENAS_H
#define ARTE_CENAS_H

#include <raylib.h>

typedef enum
{
    CENA_JORNADA,
    CENA_BATALHA,
    CENA_VITORIA,
    CENA_DERROTA,
    CENA_DERROTA_JORNADA,
    CENA_RENASCIMENTO,
    CENA_ABERTURA,
    CENA_CHEGADA,
    CENA_DESPERTAR,
} CenaQuiz;

/** @brief Sprites pequenos usados na interface da partida. */
typedef enum
{
    ICONE_OVO,
    ICONE_BURRO,
    ICONE_DRAGAO,
    ICONE_PRINCESA,
} IconeQuiz;

typedef enum
{
    GRUTA_VAZIA,
    GRUTA_OURO_BAIXO,
    GRUTA_OURO_MEDIO,
    GRUTA_OURO_ALTO,
} EstadoGruta;

/** @brief Carrega as texturas depois de abrir a janela. */
void carregar_arte_cenas(void);

/** @brief Libera as texturas antes de fechar a janela. */
void descarregar_arte_cenas(void);

/** @brief Desenha o mapa, o véu e o dragão do menu. */
void desenhar_arte_menu(void);

/** @brief Desenha o mapa, o véu e o dragão dos créditos. */
void desenhar_arte_creditos(void);

/**
 * @brief Desenha o sprite de uma carta do Trote Certeiro.
 * @param area Retângulo da carta, usado para centralizar o sprite.
 * @param simbolo Tipo da carta revelada de 0 a 2 ou -1 para desenhar o verso.
 */
void desenhar_arte_carta_burro(Rectangle area, int simbolo);

/**
 * @brief Desenha o fundo e os personagens do trecho atual do quiz.
 * @param cena Composição correspondente ao progresso ou ao resultado da partida.
 * @param passos Progresso da Jornada, com a fração do passo em curso.
 */
void desenhar_arte_quiz(CenaQuiz cena, float passos);

/**
 * @brief Mostra o impacto de um acerto da Batalha sobre o príncipe.
 * @param avanco Fração de 0 a 1 da duração do impacto.
 * @param despertado Escolhe a faísca usada no Despertar Dracônico.
 */
void desenhar_impacto_batalha(float avanco, bool despertado);

/**
 * @brief Destaca o ovo conquistado no centro do cenário atual.
 * @param cena Área do cenário que recebe o véu e o sprite do ovo.
 */
void desenhar_ovo_marco(Rectangle cena);

/**
 * @brief Centraliza o sprite do HUD na área reservada.
 * @param icone Sprite a exibir.
 * @param area Espaço reservado, em coordenadas da interface.
 * @param cor Cor e opacidade do sprite.
 */
void desenhar_icone_quiz(IconeQuiz icone, Rectangle area, Color cor);

/**
 * @brief Desenha o cenário da gruta conforme o resultado da partida.
 * @param estado Tamanho da pilha de ouro.
 * @param filhotes Quantidade de filhotes a exibir, limitada a dois.
 * @param mostrar_dragao Indica se o dragão voltou vivo para a gruta.
 */
void desenhar_arte_gruta(EstadoGruta estado, int filhotes, bool mostrar_dragao);

/** @brief Desenha o pátio e o dragão na tela do Sopro Ancestral. */
void desenhar_arte_sopro(void);

/**
 * @brief Desenha o céu e o povoado atrás do percurso do Eco da Torre.
 * @param origem Canto superior esquerdo do campo.
 */
void desenhar_fundo_eco(Vector2 origem);

/**
 * @brief Desenha o sprite da pomba sobre sua posição no campo.
 * @param centro Centro do corpo da pomba na tela.
 * @param campo Limites onde o sprite pode aparecer.
 */
void desenhar_pomba_eco(Vector2 centro, Rectangle campo);

/**
 * @brief Reveste a área sólida do Eco com pedra e, na parte inferior, um telhado.
 * @param obstaculo Área sólida usada pela colisão.
 * @param campo Região visível do percurso.
 * @param tem_telhado Indica a parte de baixo do vão.
 */
void desenhar_obstaculo_eco(Rectangle obstaculo, Rectangle campo, bool tem_telhado);

#endif
