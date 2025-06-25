#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspaudiolib.h>

// --- O nosso motor de áudio ---
// Guarda o som e o estado da reprodução
static struct {
    short *buffer;          // Buffer com os dados do som (PCM 16-bit)
    unsigned int size_bytes;   // Tamanho total do buffer em bytes
    unsigned int position;     // Posição atual da reprodução em bytes
    int is_playing;         // A nossa "bandeira" para tocar o som
} g_sound;

// --- O CORAÇÃO DA SOLUÇÃO: O CALLBACK DE ÁUDIO ---
// O hardware do PSP chama esta função automaticamente quando precisa de som.
void audio_callback(void *buf, unsigned int length, void *userdata)
{
    // Se a nossa bandeira "is_playing" não estiver levantada, não fazemos nada.
    // Apenas preenchemos o buffer do hardware com silêncio.
    if (!g_sound.is_playing) {
        memset(buf, 0, length * 2 * sizeof(short)); // length é em amostras, não bytes
        return;
    }

    // `buf` é o buffer que temos de preencher para o hardware.
    // `length` é o número de amostras que o hardware quer.
    short *output_buffer = (short *)buf;
    int i;

    for (i = 0; i < length; i++) {
        // Verifica se ainda temos som para tocar no nosso buffer
        if (g_sound.position < g_sound.size_bytes) {
            // Copia uma amostra (esquerda e direita) do nosso som para o buffer do hardware
            output_buffer[2 * i]     = g_sound.buffer[g_sound.position / 2];     // Canal Esquerdo
            output_buffer[2 * i + 1] = g_sound.buffer[g_sound.position / 2 + 1]; // Canal Direito
            g_sound.position += 4; // Avança 4 bytes (2 canais * 2 bytes/amostra)
        } else {
            // Se o som acabou, preenche o resto com silêncio
            output_buffer[2 * i]     = 0;
            output_buffer[2 * i + 1] = 0;
        }
    }

    // Se a reprodução terminou, baixa a nossa "bandeira".
    if (g_sound.position >= g_sound.size_bytes) {
        g_sound.is_playing = 0;
    }
}

int init_native_audio(const char *filename) {
    memset(&g_sound, 0, sizeof(g_sound));

    FILE *file = fopen(filename, "rb");
    if (!file) { return -1; }

    fseek(file, 44, SEEK_SET); // Pula o cabeçalho do WAV

    long start_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    g_sound.size_bytes = ftell(file) - start_pos;
    fseek(file, start_pos, SEEK_SET);

    g_sound.buffer = (short*)malloc(g_sound.size_bytes);
    if (!g_sound.buffer) {
        fclose(file);
        return -1;
    }

    fread(g_sound.buffer, 1, g_sound.size_bytes, file);
    fclose(file);

    // Inicializa a pspaudiolib e regista o nosso callback.
    pspAudioInit();
    // O canal 0 será gerido pela nossa função `audio_callback`.
    pspAudioSetChannelCallback(0, audio_callback, NULL);

    return 0;
}

void trigger_native_sound(void) {
    // Para "tocar" o som, simplesmente reiniciamos a posição
    // e levantamos a bandeira. O callback trata do resto.
    g_sound.position = 0;
    g_sound.is_playing = 1;
}

void cleanup_native_audio(void) {
    if (g_sound.buffer) {
        free(g_sound.buffer);
    }
    pspAudioEnd();
}