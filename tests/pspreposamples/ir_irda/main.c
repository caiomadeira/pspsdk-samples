/*
 * PSP Software Development Kit - https://github.com/pspdev
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - IrDA example
 *
 * Copyright (c) 2005 Frank Buss <fb@frank-buss.de> (aka Shine)
 *
 */
/*
Esse código é um exemplo de comunicação via IrDA (Infrared Data Association) entre dois dispositivos PSP, usando o PSP SDK. O código permite que dois PSPs se comuniquem por meio de infravermelho, trocando informações sobre os botões pressionados nos controles de cada dispositivo.

Aqui está uma explicação detalhada do que o código faz:

1. Configuração inicial:
O código começa incluindo várias bibliotecas do PSP SDK, como pspkernel.h, pspdebug.h, pspdisplay.h, entre outras, para acessar funcionalidades do PSP, como controle, exibição de tela e comunicação.
A função PSP_MODULE_INFO define o nome e a versão do módulo, e a macro PSP_MAIN_THREAD_ATTR define os atributos do thread principal.
A macro #define printf pspDebugScreenPrintf facilita a impressão de texto na tela de debug do PSP.
2. Função de saída (exit_callback):
A função exit_callback chama sceKernelExitGame() para sair do jogo quando o PSP for desligado ou o jogo for encerrado.
3. Thread de Callback:
A função CallbackThread cria e registra a exit_callback para que o PSP termine o jogo quando solicitado. Em seguida, faz a thread dormir até que o jogo seja fechado.
4. Configuração da thread de callback:
A função SetupCallbacks cria e inicia a thread de callback.
5. Função principal (main):
A função main configura o ambiente do PSP:
Inicializa a tela de debug com pspDebugScreenInit().
Configura o controle para usar o modo digital com sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL).
Abre a comunicação IrDA com sceIoOpen("irda0:", PSP_O_RDWR, 0), que permite ler e escrever dados pela porta IrDA.
Em seguida, entra em um loop onde:
Lê os botões pressionados no controle com sceCtrlReadBufferPositive e envia o estado dos botões via IrDA para o outro PSP.
Recebe os dados dos botões pressionados do outro PSP (se houver) e imprime na tela de debug qual botão foi pressionado.
Usa sceDisplayWaitVblankStart() para sincronizar a exibição.
6. Comportamento do código:
Quando um botão é pressionado no controle do PSP, o código envia a informação via IrDA para o outro PSP.
O outro PSP recebe essa informação e exibe qual botão foi pressionado.
A comunicação é realizada via o dispositivo de IrDA, especificado como "irda0:", permitindo uma troca de dados entre os dispositivos.
Resumo:
Este código é um exemplo simples de como usar a comunicação IrDA entre dois PSPs, onde cada dispositivo envia e recebe informações sobre os botões pressionados, mostrando essas informações na tela.

*/
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspsircs.h>
#include <stdlib.h>
#include <string.h>

/* Define the module info section */
PSP_MODULE_INFO("IRDA", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

int main(void)
{
	SceCtrlData pad;
	u32 buttonsold = 0;

	SetupCallbacks();
	pspDebugScreenInit();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	printf ("IrDA PSP-2-PSP Example by Shine\n");
        printf ("-------------------------------\n");
	printf ("This example can be used with two PSPs\n\n");
        printf ("Press CROSS, SQUARE, CIRCLE or TRIANGLE\n");
        printf ("and it will be displayed at the other PSP\n\n");

	int fd = sceIoOpen("irda0:", PSP_O_RDWR, 0);

	while (1) {
		// read pad and send it
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons != buttonsold) {
			unsigned char padHighbyte = pad.Buttons >> 8;
			sceIoWrite(fd, &padHighbyte, 1);
			buttonsold = pad.Buttons;
		}

		// check for pad info from other PSP
		unsigned char data;
		int len = sceIoRead(fd, &data, 1);
		int otherPad = data << 8;
		if (len == 1) {
			if (otherPad & PSP_CTRL_CIRCLE) printf ("CIRCLE pressed\n");
			if (otherPad & PSP_CTRL_CROSS) printf ("CROSS pressed\n");
			if (otherPad & PSP_CTRL_SQUARE) printf ("SQUARE pressed\n");
			if (otherPad & PSP_CTRL_TRIANGLE) printf ("TRIANGLE pressed\n");
		}
		
		sceDisplayWaitVblankStart(); 
	}

	return 0;
}