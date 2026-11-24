/**
 * @file cores.h
 * @brief Reúne as cores usadas nas telas e nos componentes.
 */
#ifndef CORES_H
#define CORES_H

#include <raylib.h>

// Cada Color contém vermelho, verde, azul e opacidade, de 0 a 255.
#define COR_FUNDO_MENU       ((Color){21, 29, 33, 255})
#define COR_MOLDURA_MAPA     ((Color){87, 109, 98, 255})
#define COR_ESCURO           ((Color){16, 24, 28, 255})
#define COR_VEU_MAPA         ((Color){19, 35, 36, 77})
#define COR_TEXTO_CLARO      ((Color){245, 235, 216, 255})
#define COR_AVISO            ((Color){230, 19, 103, 255})

#define COR_FACE_BOTAO       ((Color){41, 54, 58, 255})
#define COR_BORDA_BOTAO      ((Color){81, 98, 107, 255})
#define COR_ARESTA_BOTAO     ((Color){54, 71, 78, 255})
#define COR_FACE_PRIMARIA    ((Color){218, 171, 120, 255})
#define COR_BORDA_PRIMARIA   ((Color){240, 205, 163, 255})
#define COR_ARESTA_PRIMARIA  ((Color){148, 103, 67, 255})
#define COR_FACE_SELECIONADA ((Color){26, 39, 54, 255})
#define COR_BORDA_SELECIONADA ((Color){138, 175, 206, 255})
#define COR_FACE_BLOQUEADA   ((Color){37, 45, 48, 255})
#define COR_TEXTO_BLOQUEADO  ((Color){150, 163, 154, 255})

#endif
