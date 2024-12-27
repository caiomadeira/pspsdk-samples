/*
 * PSP Software Development Kit - Shadow Projection Sample
 * Organized into functions for clarity and reusability
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>

PSP_MODULE_INFO("Shadow Projection Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define printf pspDebugScreenPrintf
#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH * SCR_HEIGHT * 2)
#define GRID_COLUMNS 32
#define GRID_ROWS 32
#define GRID_SIZE 10.0f

static unsigned int __attribute__((aligned(16))) list[262144];
static unsigned short __attribute__((aligned(16))) grid_indices[(GRID_COLUMNS - 1) * (GRID_ROWS - 1) * 6];

// Structures for grid vertices and geometry
typedef struct {
    float nx, ny, nz; // Normal vector
    float x, y, z;    // Position
} Vertex_Normal;

static Vertex_Normal __attribute__((aligned(16))) grid_vertices[GRID_COLUMNS * GRID_ROWS];

typedef struct {
    ScePspFMatrix4 world; // World transformation matrix
    unsigned int count;   // Number of indices
    unsigned short *indices; // Indices array
    Vertex_Normal *vertices; // Vertices array
    unsigned int color;   // Geometry color
} Geometry;

// Function prototypes
int SetupCallbacks();
void InitGraphics();
void InitMatrices(ScePspFMatrix4 *identity, ScePspFMatrix4 *projection, ScePspFMatrix4 *view);
void genGrid(unsigned rows, unsigned columns, float size, Vertex_Normal *dstVertices, unsigned short *dstIndices);
void drawGeometry(Geometry *geom);

// Graphics initialization
void InitGraphics(void* frameBuffer0, void* frameBuffer1, void* depthBuffer) {
    sceGuInit();

    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, frameBuffer0, BUF_WIDTH); // sceGuDrawBuffer(GU_PSM_4444, frameBuffer0, BUF_WIDTH);
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, frameBuffer1, BUF_WIDTH); // sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void *)frameBuffer1, BUF_WIDTH);
    sceGuDepthBuffer(depthBuffer, BUF_WIDTH); // sceGuDepthBuffer((void *)depthBuffer, BUF_WIDTH);
    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Problema esta aqui
    sceGuDepthRange(0xc350, 0x2710);
    //sceGuDepthFunc(GU_GEQUAL);
    sceGuDisable(GU_DEPTH_TEST); // NESSA CONFIGURACAO, MANTER DESATIVASO O TESTE DE PROFUNDIDADE
    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_CULL_FACE);
    sceGuEnable(GU_TEXTURE_2D);
    //sceGuEnable(GU_DITHER);
    sceGuEnable(GU_CLIP_PLANES);


    sceGuFinish();
    sceGuSync(0, 0);
    // sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

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

            curr->x = ((i * (1.0f / (float)(columns - 1))) - 0.5f) * size;
            curr->y = 0;
            curr->z = ((j * (1.0f / (float)(rows - 1))) - 0.5f) * size;
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
    sceGuColor(geom->color);
    sceGuDrawArray(GU_TRIANGLES, GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_INDEX_16BIT | GU_TRANSFORM_3D, geom->count, geom->indices, geom->vertices);
}

// Entry point
int main(int argc, char *argv[]) {
    SetupCallbacks();

    void* frameBuffer0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888); //  (void*)0x0;
    void* frameBuffer1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888); // (void*)0x88000;
    void* depthBuffer = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444); // (void*)0x110000;
    // Initialize graphics and matrices
    InitGraphics(frameBuffer0, frameBuffer1, depthBuffer);

    ScePspFMatrix4 identity, projection, view;
    InitMatrices(&identity, &projection, &view);

    // Generate grid geometry
    genGrid(GRID_ROWS, GRID_COLUMNS, GRID_SIZE, grid_vertices, grid_indices);

    Geometry grid = {
        identity,
        sizeof(grid_indices) / sizeof(unsigned short),
        grid_indices,
        grid_vertices,
        0xff7777
    };

    for (;;) {
        // Update grid position
        ScePspFVector3 pos = {0, -1.5f, 0};
        gumLoadIdentity(&grid.world);
        gumTranslate(&grid.world, &pos);

        // Render the scene
        sceGuStart(GU_DIRECT, list);
        sceGuClearColor(0xff554433);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceGuSetMatrix(GU_PROJECTION, &projection);
        sceGuSetMatrix(GU_VIEW, &view);
        drawGeometry(&grid);
        sceGuFinish();
        sceGuSync(0, 0);

        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    sceGuTerm();
    sceKernelExitGame();
    return 0;
}

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

/* Setup callbacks */
int SetupCallbacks(void) {
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}
