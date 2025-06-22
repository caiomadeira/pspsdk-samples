#include <pspkernel.h>
#include <pspdebug.h>

PSP_MODULE_INFO("HelloPSP", 0,1,0);

int main()
{
    pspDebugScreenInit();
    pspDebugScreenPrintf("Hello from PdSP!\n");

    sceKernelSleepThread();
    return 0; 
}