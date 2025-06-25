#ifndef NATIVE_AUDIO_H
#define NATIVE_AUDIO_H

// Inicializa o sistema de áudio e carrega o nosso ficheiro de som.
int  init_native_audio(const char *filename);

// "Arma" o sistema para tocar o som na próxima vez que o hardware pedir.
void trigger_native_sound(void);

// Liberta todos os recursos de áudio ao sair.
void cleanup_native_audio(void);

#endif