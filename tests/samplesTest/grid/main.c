#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <pspgum.h>
#include <stdlib.h>
#include <math.h>

PSP_MODULE_INFO("Grid", 0, 1, 1); // Nome do módulo, flags, versão principal, versão secundária
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU); // Atributos da thread principal

#define GRID_COLUMNS 32
#define GRID_ROWS 32
#define GRID_SIZE 10.0f

typedef struct Vertex_Normal {
    float nx, ny, nz;
    float x, y, z;
} Vertex_Normal;

static unsigned int __attribute__((aligned(16))) list[262144];
Vertex_Normal __attribute__((aligned(16))) grid_vertices[GRID_COLUMNS * GRID_ROWS];
unsigned short __attribute__((aligned(16))) grid_indices[(GRID_COLUMNS - 1) * (GRID_ROWS - 1) * 6];



void genGrid(unsigned rows, unsigned columns, float size, Vertex_Normal* dstVertices, unsigned short* dstIndices) {
    unsigned int i, j;

    // Gerar vértices do grid
    for (j = 0; j < rows; ++j) {
        for (i = 0; i < columns; ++i) {
            Vertex_Normal* curr = &dstVertices[i + j * columns];

            curr->nx = 0.0f;
            curr->ny = 1.0f;
            curr->nz = 0.0f;

            curr->x = ((i * (1.0f / ((float)columns))) - 0.5f) * size;
            curr->y = 0;
            curr->z = ((j * (1.0f / ((float)columns))) - 0.5f) * size;
        }
    }

    // Gerar índices para os triângulos
    for (j = 0; j < rows - 1; ++j) {
        for (i = 0; i < columns - 1; ++i) {
            unsigned short* curr = &dstIndices[(i + (j * (columns - 1))) * 6];

            *curr++ = i + j * columns;
            *curr++ = (i + 1) + j * columns;
            *curr++ = i + (j + 1) * columns;

            *curr++ = (i + 1) + j * columns;
            *curr++ = (i + 1) + (j + 1) * columns;
            *curr++ = i + (j + 1) * columns;
        }
    }
}

// Define a função para sair
int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

// Registra o callback de saída
int CallbackThread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

// Cria o thread de callback
int SetupCallbacks(void) {
    int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }
    return thid;
}

int main(int argc, char* argv[]) {
    SetupCallbacks();
    sceGuInit();
    sceGuStart(GU_DIRECT, list);

    // Configuração básica do display
    sceGuDrawBuffer(GU_PSM_8888, (void*)0, 512);
    sceGuDispBuffer(480, 272, (void*)0x88000, 512);
    sceGuDepthBuffer((void*)0x110000, 512);
    sceGuOffset(2048 - (480 / 2), 2048 - (272 / 2));
    sceGuViewport(2048, 2048, 480, 272);
    sceGuDepthRange(0xc350, 0x2710);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuFinish();
    sceGuSync(0, 0);
    sceGuDisplay(GU_TRUE);

    // Gerar o grid
    genGrid(GRID_ROWS, GRID_COLUMNS, GRID_SIZE, grid_vertices, grid_indices);

    while (1) {
        sceGuStart(GU_DIRECT, list);

        // Limpar tela
        sceGuClearColor(0xff554433);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        // Configurar matriz
        ScePspFMatrix4 identity;
        gumLoadIdentity(&identity);
        sceGuSetMatrix(GU_MODEL, &identity);

        // Desenhar o grid
        sceGuDrawArray(
            GU_TRIANGLES,
            GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_INDEX_16BIT | GU_TRANSFORM_3D,
            (GRID_COLUMNS - 1) * (GRID_ROWS - 1) * 6,
            grid_indices,
            grid_vertices
        );

        sceGuFinish();
        sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    sceGuTerm();
    return 0;
}
