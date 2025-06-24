#ifndef AUDIO_H
#define AUDIO_H

// Inicializa o sistema de áudio e carrega todos os sons.
// Retorna 0 em caso de sucesso, -1 em caso de erro.
int  init_audio(void);

// Toca o nosso efeito sonoro carregado.
void play_laser_sound(void);

// Liberta todos os recursos de áudio.
void cleanup_audio(void);

#endif // AUDIO_H