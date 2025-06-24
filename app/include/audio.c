#include "audio.h"
#include <SDL3/SDL.h>

// Estrutura para guardar os nossos recursos de áudio
static struct {
    SDL_AudioDeviceID deviceId;
    SDL_AudioStream *stream;
    
    // Para o nosso efeito sonoro
    SDL_AudioSpec wavSpec;
    Uint8 *wavBuffer;
    Uint32 wavLength;
} audio_engine;

int init_audio(void) {
    // 1. Inicializar o subsistema de áudio do SDL3
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao iniciar áudio do SDL3: %s", SDL_GetError());
        return -1;
    }

    // 2. Abrir um dispositivo de áudio. Usamos as configurações padrão.
    audio_engine.deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (audio_engine.deviceId == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao abrir dispositivo de áudio: %s", SDL_GetError());
        return -1;
    }

    // 3. Carregar o nosso ficheiro .WAV
    if (SDL_LoadWAV("laser.wav", &audio_engine.wavSpec, &audio_engine.wavBuffer, &audio_engine.wavLength) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao carregar 'laser.wav': %s", SDL_GetError());
        SDL_CloseAudioDevice(audio_engine.deviceId);
        return -1;
    }

    // 4. Criar um AudioStream. A fonte (src) é o formato do nosso WAV,
    //    o destino (dst) é o formato do nosso dispositivo de áudio.
    SDL_AudioSpec deviceSpec;
    SDL_GetAudioDeviceFormat(audio_engine.deviceId, &deviceSpec, NULL);
    audio_engine.stream = SDL_CreateAudioStream(&audio_engine.wavSpec, &deviceSpec);
    if (!audio_engine.stream) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao criar AudioStream: %s", SDL_GetError());
        SDL_free(audio_engine.wavBuffer);
        SDL_CloseAudioDevice(audio_engine.deviceId);
        return -1;
    }

    // 5. Ligar o nosso stream ao dispositivo. Agora eles estão conectados.
    if (SDL_BindAudioStream(audio_engine.deviceId, audio_engine.stream) == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao ligar AudioStream: %s", SDL_GetError());
        SDL_DestroyAudioStream(audio_engine.stream);
        SDL_free(audio_engine.wavBuffer);
        SDL_CloseAudioDevice(audio_engine.deviceId);
        return -1;
    }
    
    // 6. O dispositivo começa "não-pausado" no SDL3, então não precisamos de chamar Resume.

    SDL_Log("Sistema de áudio SDL3 inicializado com sucesso!");
    return 0;
}

void play_laser_sound(void) {
    // Para tocar o som, simplesmente colocamos os dados do WAV no stream.
    // O SDL3 cuida do resto.
    if (audio_engine.stream && audio_engine.wavBuffer) {
        if (SDL_PutAudioStreamData(audio_engine.stream, audio_engine.wavBuffer, audio_engine.wavLength) == 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao colocar dados no stream: %s", SDL_GetError());
        }
    }
}

void cleanup_audio(void) {
    if (audio_engine.deviceId != 0) {
        SDL_UnbindAudioStream(audio_engine.stream);
        SDL_DestroyAudioStream(audio_engine.stream);
        SDL_CloseAudioDevice(audio_engine.deviceId);
    }
    if (audio_engine.wavBuffer) {
        SDL_free(audio_engine.wavBuffer);
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Log("Sistema de áudio SDL3 limpo.");
}