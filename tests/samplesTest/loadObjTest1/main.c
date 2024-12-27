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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>


PSP_MODULE_INFO("Cube Sample", 0, 1, 1);
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

int main(int argc, char* argv[])
{
	setup_callbacks();

	// setup GU
	int dbuff = 0;
	void* fbp0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
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

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	// run sample

	int val = 0;
    running = 1;
	while(running)
	{
		sceGuStart(GU_DIRECT,list);

		// clear screen

		unsigned int clearGrayColor = 0xff554433;
		sceGuClearColor(clearGrayColor);
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		// setup matrices for cube

		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		/*
		**********************
		Variaveis com nomes mais simples
		*/
		float ZDepth = 75.0f; // Responsavel por mostrar o quao perto da tela o cubo esta
		float flatCube =  16.0f/9.0f; // expreme o cubo
		float verticeProximityScreen = 0.5f; // proximidade c a tela (?)

		sceGumPerspective(ZDepth, flatCube, verticeProximityScreen, 1000.0f);

		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();

		sceGumMatrixMode(GU_MODEL);

		float GU_PI_CALC = (GU_PI/180.0f);

		// Variaveis da animação de rotacao
		float rotZConst = 1.32f;
		float rotYConst = 0.5f;
		float rotXConst = 0.9f;
		
		/*
		Função: sceGumLoadIdentity()
		Significado: Redefine a matriz atual para a matriz identidade.
		Uso típico: Antes de configurar transformações (como translação ou rotação) para evitar transformações acumuladas.
		*/
		sceGumLoadIdentity();
		{
			float positionInGeralPlanZ = -3.5f;
			float positionInGeralPlanY = 0.0f;
			float positionInGeralPlanX = 1.0f;
			ScePspFVector3 pos = { positionInGeralPlanX, positionInGeralPlanY, positionInGeralPlanZ };
			ScePspFVector3 rot = { val * rotXConst * GU_PI_CALC, val * rotYConst * GU_PI_CALC, val * rotZConst * GU_PI_CALC };
			sceGumTranslate(&pos);
			sceGumRotateXYZ(&rot);
		}

		// setup texture
		/*
		GU_PSM_8888, GU_PSM_4444, etc... formatos de pixel (ou formatos de textura) no PSP.
		GU_PSM_8888; Quando a precisão de cores e a qualidade visual são importantes.
		GU_PSM_4444: Quando você precisa de texturas com suporte a transparência (canal alfa),
		 mas quer economizar memória em relação a formatos mais ricos.

		Use GU_PSM_4444 ou GU_PSM_8888 se precisar de transparência.
		Use GU_PSM_5650 ou GU_PSM_5551 para texturas opacas e economizar memória.
		Use GU_PSM_T8 se estiver trabalhando com texturas indexadas e quiser economizar ainda mais memória.
		
		*/
		sceGuTexMode(GU_PSM_5650, 0,0,0);

		int textureImageWidth = 64;
		int textureImageHeight = 64;

		/*
		64 (Stride): Tamanho da linha em pixels. Normalmente igual à largura, 
		mas pode ser maior para texturas armazenadas de forma otimizada.
		*/
		int textureImageStride = 64;
		
		sceGuTexImage(0,textureImageWidth,textureImageHeight,textureImageStride,logo_start);
		sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB); // Define como a textura será combinada com a cor base dos vértices.

		/*
		sceGuTexEnvColor
		Função: Define a cor de ambiente para a textura.

		Parâmetros:

		0xffff00: Cor definida no formato hexadecimal ARGB:
		0xffff00 é amarelo puro (Alpha = FF, Vermelho = FF, Verde = FF, Azul = 00).
		Alteração:

		Mude a cor para alterar o tom ambiental da textura.

		*/
		sceGuTexEnvColor(0xffff00); // sinceramente, nao vi mt diferenca

		/*
			5. sceGuTexFilter
			sceGuTexFilter(GU_LINEAR, GU_LINEAR);
			Função: Define o método de filtragem da textura (como ela será renderizada em diferentes tamanhos).

			Parâmetros:

			GU_LINEAR (Minificação): Suaviza a textura ao reduzir.
			GU_LINEAR (Magnificação): Suaviza a textura ao ampliar.
			Alternativas incluem:
			GU_NEAREST: Usa o ponto mais próximo (menos suave, mas mais rápido).
			Alteração:

			Use GU_NEAREST para desempenho.
			Use GU_LINEAR para qualidade.
		
		*/
		sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		/*
			6. sceGuTexScale
			sceGuTexScale(1.0f, 1.0f);
			Função: Define a escala de repetição da textura.

			Parâmetros:

			1.0f (U): Escala horizontal.
			1.0f (V): Escala vertical.
			Valores menores que 1.0 aumentam o número de repetições da textura.
			Valores maiores que 1.0 reduzem o número de repetições.
			Alteração:

			Use valores diferentes para criar padrões repetitivos ou texturas esticadas.
		*/
		sceGuTexScale(1.0f,1.0f);
		/*
			7. sceGuTexOffset
			sceGuTexOffset(0.0f, 0.0f);
			Função: Define o deslocamento da textura.

			Parâmetros:

			0.0f (U): Deslocamento horizontal.
			0.0f (V): Deslocamento vertical.
			Valores diferentes de 0 deslocam a textura.
			Alteração:

			Altere para animar texturas ou criar efeitos de deslizamento.
		*/
		sceGuTexOffset(0.0f,0.0f);
		/*
			8. sceGuAmbientColor
			sceGuAmbientColor(0xffffffff);
			Função: Define a cor de iluminação ambiente aplicada a toda a cena.

			Parâmetros:

			0xffffffff: Cor no formato ARGB (branco puro).
			Alteração:

			Ajuste para alterar o tom geral de iluminação.

		*/
		sceGuAmbientColor(0xffffffff);

		// draw cube

		sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,vertices);
		/*
		1. sceGuFinish();
		O que faz: Finaliza a submissão de comandos de renderização para o pipeline gráfico.
		Detalhes:
		As funções de desenho (como sceGumDrawArray) enviam comandos para um buffer de comandos que a GPU do PSP processa.
		sceGuFinish marca o fim desse envio e sinaliza para o sistema que o buffer de comandos está pronto para ser processado.
		*/
		sceGuFinish();
		/*
			sceGuSync(0, 0);
			O que faz: Sincroniza a CPU com a GPU, esperando 
			que todos os comandos enviados para a GPU sejam concluídos antes de continuar.

		*/
		sceGuSync(0,0);

		/*
		3. sceDisplayWaitVblankStart();
		O que faz: Espera pelo início da próxima v-blank (intervalo vertical).
		Detalhes:
		A v-blank é o intervalo entre o término da renderização de um quadro e o início do próximo. Esse é o momento ideal para trocar buffers, pois evita "tearing" (corte visual).
		Essa função força o programa a aguardar até que a GPU termine de exibir o quadro atual na tela.
		*/
		sceDisplayWaitVblankStart();
		/*
			4. sceGuSwapBuffers();
			O que faz: Troca os buffers de exibição, alternando entre o buffer frontal (atualmente exibido) e o buffer traseiro (onde o próximo quadro está sendo renderizado).
			Detalhes:
			O PSP usa um esquema de double buffering:
			Enquanto a GPU está renderizando o próximo quadro no buffer traseiro, o quadro atual é exibido na tela a partir do buffer frontal.
			Após o término do desenho e sincronização, os buffers são trocados.
			Isso garante uma renderização suave e sem interrupções visuais.		
		*/
		sceGuSwapBuffers(); //alterna os buffers para exibir o novo quadro.

		val++;
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}