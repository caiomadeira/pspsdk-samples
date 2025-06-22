// psp_ir_stubs.c — stubs para funções não implementadas no PSPSDK
int pspIrKeybInit(void) { return 0; }
int pspIrKeybFinish(void) { return 0; }
int pspIrKeybOutputMode(int mode) { return 0; }
int pspIrKeybReadinput(char *dst) { (void)dst; return 0; }

int sceHprmPeekCurrentKey(void) { return 0; }