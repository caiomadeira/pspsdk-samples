/*
 * PSP Software Development Kit - https://github.com/pspdev
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>


PSP_MODULE_INFO("3D CUBE CONTROL Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

/*
unsigned int list[262144];: Define um array de 262.144 elementos, onde cada elemento é de 4 bytes (32 bits).
Isso resulta em 1 MB de memória (262.144 × 4 bytes = 1.048.576 bytes).
__attribute__((aligned(16))): Garante que a lista está alinhada em um limite de 16 bytes na memória. O hardware do PSP requer que as display lists estejam alinhadas para otimizar a leitura.
Cada comando gráfico que você chama, como sceGuStart, sceGuTexImage, sceGuDrawArray, etc., gera instruções que são adicionadas à display list.
Texturas maiores (ex.: 256×256 ao invés de 64×64) não necessariamente exigem uma 
display list maior, mas mais comandos ou configurações adicionais podem aumentar o espaço necessário.

Quando Devo Alterar o Tamanho da Display List?
Cenários Simples (textura única): A configuração de list[262144] geralmente é
 suficiente para renderizar cenas simples com algumas texturas.
Cenas Complexas: Se você renderizar muitas texturas, modelos 3D detalhados ou aplicar 
efeitos avançados, talvez precise aumentar o tamanho da display list.
*/
static unsigned int __attribute__((aligned(16))) list[262144];
extern unsigned char logo_start[];

struct Vertex
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

struct Vertex __attribute__((aligned(16))) vertices[12*3] =
{
	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 0, 0xff7f0000,-1, 1, 1}, // 4
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5

	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5
	{0, 1, 0xff7f0000, 1,-1, 1}, // 1

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 0, 0xff7f0000, 1,-1,-1}, // 2
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6
	{0, 1, 0xff7f0000,-1, 1,-1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 0, 0xff007f00, 1,-1, 1}, // 3
	{1, 1, 0xff007f00, 1, 1, 1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 1, 0xff007f00, 1, 1, 1}, // 7
	{0, 1, 0xff007f00, 1, 1,-1}, // 4

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 0, 0xff007f00,-1, 1,-1}, // 3
	{1, 1, 0xff007f00,-1, 1, 1}, // 7

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 1, 0xff007f00,-1, 1, 1}, // 7
	{0, 1, 0xff007f00,-1,-1, 1}, // 4

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 0, 0xff00007f, 1, 1,-1}, // 1
	{1, 1, 0xff00007f, 1, 1, 1}, // 2

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 1, 0xff00007f, 1, 1, 1}, // 2
	{0, 1, 0xff00007f,-1, 1, 1}, // 3

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 0, 0xff00007f,-1,-1, 1}, // 7
	{1, 1, 0xff00007f, 1,-1, 1}, // 6

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 1, 0xff00007f, 1,-1, 1}, // 6
	{0, 1, 0xff00007f, 1,-1,-1}, // 5
};

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

int running = 0;


int exit_callback(int arg1, int arg2, void *common) {
    running = 0;
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) 
{
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

// INIT GU CONTEXT
// Inicia o meu contexto grafico (renderizacao) da tela atual

static void initGuContext(void *list, void *fbp0, void *fbp1, void*zbp)
{
	unsigned int clearGrayColor = 0xff554433;

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuClearColor(clearGrayColor);

	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);
}


int main(int argc, char* argv[])
{
	pspDebugScreenInit();
	setup_callbacks();

	pspDebugScreenSetXY(26,15);
	pspDebugScreenPrintf("Test1 1: GU, Controls\n");
	pspDebugScreenSetXY(26,20);

	pspDebugScreenPrintf("By Caio Madeira\n");

	for(int i = 0; i < 300; i++)
	{
		sceDisplayWaitVblankStart();
	}
	// setup GU
	// fbp = frame buffer pointer
	void* fbp0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	// init gu
	sceGuInit(); // default psp
	initGuContext(list, fbp0, fbp1, zbp); // my context
	//sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	// debug
    //pspDebugScreenInitEx(NULL, PSP_DISPLAY_PIXEL_FORMAT_8888, 0);
    // pspDebugScreenEnableBackColor(0);

	// sceDisplayWaitVblankStart();
	// sceGuDisplay(GU_TRUE);
	
	// run sample

	int textureImageWidth = 64;
	int textureImageHeight = 64;
	int textureImageStride = 64;

	float posX = 0.0f, posY = 0.0f, posZ = -3.5f;

	// Variaveis da animação de rotacao
	float rotZConst = 1.32f;
	float rotYConst = 0.5f;
	float rotXConst = 0.9f;
	/*
	**********************
	Variaveis com nomes mais simples
	**********************
	*/
	float ZDepth = 75.0f; // Responsavel por mostrar o quao perto da tela o cubo esta
	float flatCube =  16.0f/9.0f; // expreme o cubo
	float verticeProximityScreen = 0.5f; // proximidade c a tela (?)

	// control stuff

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	void* buffer = 0;

    running = 1;

	while(running)
	{			
		// control stuff
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);


		sceGuStart(GU_DIRECT,list);
		// clear screen
		//sceGuClearColor(0);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		// cube perspective and pos and rot
		sceGumPerspective(ZDepth, flatCube, verticeProximityScreen, 1000.0f);
		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		{
			// Initial pos
            ScePspFVector3 pos = { posX, posY, posZ };
			sceGumTranslate(&pos);
		}

		// // setup texture and draw cube
		sceGuTexMode(GU_PSM_5650, 0,0,0);
		sceGuTexImage(0,textureImageWidth,textureImageHeight,textureImageStride,logo_start);
		sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB); // Define como a textura será combinada com a cor base dos vértices.
		sceGuTexEnvColor(0xffff00); // sinceramente, nao vi mt diferenca
		sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		sceGuTexScale(1.0f,1.0f);
		sceGuTexOffset(0.0f,0.0f);
		sceGuAmbientColor(0xffffffff);
		sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,vertices);
		sceGuFinish();
		sceGuSync(0,0);

		// pspDebugScreenSetOffset((int)fbp0);
		// pspDebugScreenSetXY(0, 2);
		// pspDebugScreenPrintf("TESTEEE");

		//for(int i = 0; i < 100; i++)
		sceDisplayWaitVblankStart();
		fbp1 = fbp0;
		fbp0 = sceGuSwapBuffers();

		if (pad.Buttons & PSP_CTRL_RIGHT)
		{
            posX++;
			//sceGumTranslate(&pos);
		}
		if (pad.Buttons & PSP_CTRL_LEFT)
		{
            posX--;
		}
		if (pad.Buttons & PSP_CTRL_UP)
		{
            posZ--;
		}
		if (pad.Buttons & PSP_CTRL_DOWN)
		{
            posZ++;
		}
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}