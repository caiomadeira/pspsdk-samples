#include "complexgraphics.h"

static unsigned int __attribute__((aligned(16))) list[262144];

// Definição das variáveis globais
Vertex_Normal __attribute__((aligned(16))) grid_vertices[GRID_COLUMNS * GRID_ROWS];
unsigned short __attribute__((aligned(16))) grid_indices[(GRID_COLUMNS - 1) * (GRID_ROWS - 1) * 6];


// Initialize matrices for projection and view
void InitMatrices(ScePspFMatrix4 *identity, ScePspFMatrix4 *projection, ScePspFMatrix4 *view) {
    gumLoadIdentity(identity);
    
    gumLoadIdentity(projection);
    gumPerspective(projection, 75.0f, 16.0f / 9.0f, 0.5f, 1000.0f);

    ScePspFVector3 pos = {0, 0, -5.0f};
    gumLoadIdentity(view);
    gumTranslate(view, &pos);
}

// Generate grid vertices and indices
void genGrid(unsigned rows, unsigned columns, float size, Vertex_Normal *dstVertices, unsigned short *dstIndices) {
    unsigned int i, j;

    for (j = 0; j < rows; ++j) {
        for (i = 0; i < columns; ++i) {
            Vertex_Normal *curr = &dstVertices[i + j * columns];

            curr->nx = 0.0f;
            curr->ny = 1.0f;
            curr->nz = 0.0f;

            curr->x = ((i * (1.0f / (float)columns)) - 0.5f) * size;
            curr->y = 0;
            curr->z = ((j * (1.0f / (float)columns)) - 0.5f) * size;
        }
    }

    for (j = 0; j < rows - 1; ++j) {
        for (i = 0; i < columns - 1; ++i) {
            unsigned short *curr = &dstIndices[(i + (j * (columns - 1))) * 6];

            *curr++ = i + j * columns;
            *curr++ = (i + 1) + j * columns;
            *curr++ = i + (j + 1) * columns;

            *curr++ = (i + 1) + j * columns;
            *curr++ = (i + 1) + (j + 1) * columns;
            *curr++ = i + (j + 1) * columns;
        }
    }
}

// Draw geometry on the screen
void drawGeometry(Geometry *geom) {

    sceGuSetMatrix(GU_MODEL, &geom->world);

    sceGuDisable(GU_TEXTURE_2D); // Desabilite texturas se não forem usadas
    sceGuColor(geom->color);
    sceGuDrawArray(GU_TRIANGLES, GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_INDEX_16BIT | GU_TRANSFORM_3D, geom->count, geom->indices, geom->vertices);
}
