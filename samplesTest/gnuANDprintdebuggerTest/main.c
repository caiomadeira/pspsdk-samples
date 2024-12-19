#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspctrl.h>
#include <stdio.h>

// Informações do módulo
PSP_MODULE_INFO("DebugGU", 0, 1, 0);

// Inicializar o GU
void initGU() {
    sceGuInit();
    sceGuStart(GU_DIRECT, sceGuGetMemory(0));
    sceGuDrawBuffer(GU_PSM_8888, (void*)0x04000000, 512);
    sceGuDispBuffer(480, 272, (void*)0x04088000, 512);
    sceGuDepthBuffer((void*)0x04110000, 512);
    sceGuOffset(2048 - (480 / 2), 2048 - (272 / 2));
    sceGuViewport(2048, 2048, 480, 272);
    sceGuDepthRange(65535, 0);
    sceGuScissor(0, 0, 480, 272);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

int main(void) {
    // Inicializar depuração na tela
    pspDebugScreenInit();
    pspDebugScreenPrintf("Inicializando o aplicativo...\n");

    // Inicializar o GU
    initGU();

    // Renderizar uma tela com o GU
    sceGuStart(GU_DIRECT, sceGuGetMemory(0));
    sceGuClearColor(0xFF0000FF); // Azul
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
    sceGuFinish();
    sceGuSync(0, 0);

    // Pausa para depuração
    //sceKernelDelayThread(5 * 1000000); // 5 segundos

    // Mensagem final no console
    while(1)
    {
        pspDebugScreenPrintf("Finalizando o aplicativo.\n");

    }
    //sceKernelSleepThread();
    return 0;
}
