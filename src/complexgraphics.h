#ifndef COMPLEXGRAPHICS_H
#define COMPLEXGRAPHICS_H

#include "common.h"
#include "graphic.h"

#define GRID_COLUMNS 32
#define GRID_ROWS 32
#define GRID_SIZE 10.0f

// Estrutura para descrever um vértice com normal e posição
typedef struct Vertex_Normal {
    float nx, ny, nz; // Vetor normal
    float x, y, z;    // Posição
} Vertex_Normal;

// Estrutura para descrever a geometria de um objeto
typedef struct Geometry {
    ScePspFMatrix4 world;  // Matriz de transformação do mundo
    unsigned int count;    // Número de índices
    unsigned short *indices; // Array de índices
    Vertex_Normal *vertices; // Array de vértices
    unsigned int color;    // Cor do objeto
} Geometry;

// Declaração de variáveis externas
extern Vertex_Normal __attribute__((aligned(16))) grid_vertices[GRID_COLUMNS * GRID_ROWS];
extern unsigned short __attribute__((aligned(16))) grid_indices[(GRID_COLUMNS - 1) * (GRID_ROWS - 1) * 6];

// Declaração de funções
void InitMatrices(ScePspFMatrix4 *identity, ScePspFMatrix4 *projection, ScePspFMatrix4 *view);
void genGrid(unsigned rows, unsigned columns, float size, Vertex_Normal *dstVertices, unsigned short *dstIndices);
void drawGeometry(Geometry *geom);

#endif
