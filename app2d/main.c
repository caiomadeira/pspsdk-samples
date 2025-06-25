#include "include/common.h"
#include "include/player.h"
#include "include/audio.h"

#include <unistd.h> // Necessário para a função chdir()
#include <string.h> // Necessário para a função strrchr()
#include <SDL3_image/SDL_image.h> // A de imagem está na sua própria pasta!
#include <SDL3/SDL_oldnames.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include<pspaudio.h>
#include<pspaudiolib.h>

#define INIT_OPEN_GL_WINDOW false

typedef struct app {
	SDL_Window *window;
	SDL_GLContext context;
    GameState currentstate;
    SceCtrlData pad;
    Player *player;
    SDL_Texture *background_texture;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SceCtrlData prev_pad;
} app_t;

// Pega o caminho do executável e muda o diretório de trabalho
void get_binary_path(int argc, char *argv[])
{
    if (argc > 0 && argv[0] != NULL)
    {
        char* path = strdup(argv[0]);
        char* last_path = strrchr(path, '/');
        if (last_path)
        {
            *last_path = '\0';
            chdir(path);
        }
        free(path);
    }
}

/*
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *  ON READY
 * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	app_t *a;
    get_binary_path(argc, argv);

    // --- PASSO 1 (CRÍTICO): Inicializar o SDL SEM o subsistema de áudio ---
    // A pspaudiolib irá controlar o hardware de som diretamente.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init(VIDEO) falhou: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // --- PASSO 2: Chamar a nossa função de inicialização de áudio nativo ---
    if (init_native_audio("test.wav") != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "native_audio_init_and_load falhou.");
        // Pode adicionar aqui um pspDebugScreenPrintf para ver o erro no PSP.
        return SDL_APP_FAILURE;
    }
    
    // if (init_audio() != 0) {
    //     return SDL_APP_FAILURE;
    // }
    
    if (TTF_Init() == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "falha ao iniciar SDL_ttf: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	a = (app_t*)SDL_calloc(1, sizeof(app_t)); // usando calloc p/ zerar a memoria
	if (a == NULL)
    {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
        return SDL_APP_FAILURE;
    }

    SDL_zero(a->prev_pad);

    a->window = SDL_CreateWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, INIT_OPEN_GL_WINDOW); // janela de renderizacao c open_gl
    if (a->window == NULL)
    {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
        return SDL_APP_FAILURE;
    }

    a->renderer = SDL_CreateRenderer(a->window, NULL);
    if (a->renderer == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao criar renderer: %s", SDL_GetError());
        SDL_DestroyWindow(a->window);
        SDL_free(a);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }

    a->font = TTF_OpenFont("roasted-chicken.ttf", 30);
    if (a->font == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FALHA AO CARREGAR A FONTE: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	// ativa o v-sync
	//SDL_GL_SetSwapInterval(1);

    // Inicializando o controle nativo
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    SDL_Surface *bg_surface = IMG_Load("background.png");
    if (!bg_surface) {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
            
        return SDL_APP_FAILURE;
    }
    // aqui importante: converte a superficie numa texture otimizada pra a GPU e guarda em um buffer
    a->background_texture = SDL_CreateTextureFromSurface(a->renderer, bg_surface);
    SDL_DestroySurface(bg_surface); // libera a superficie da mem. ja que n precisamos mais
    if (!a->background_texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao criar textura do background: %s", SDL_GetError());
        SDL_DestroyRenderer(a->renderer);
        SDL_DestroyWindow(a->window);
        SDL_free(a);
        SDL_Quit();
        return SDL_APP_FAILURE;
    }
    
    // // load audio
    // load_audiof();
    // play_background_music();

    a->player = init_player();
    if (a->player == NULL)
    {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
        return SDL_APP_FAILURE;
    }

	*appstate = (void *)a;
	return SDL_APP_CONTINUE;
}

/*
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *  GAME LOOP
 * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    app_t *a = (app_t *)appstate;
    a->prev_pad = a->pad;

    // --- LOGICA DE CONTROLES
    readButtonState(&a->pad, 1);
    float analog_x = (a->pad.Lx - 128) / 128.0f;    // Normaliza o valor do analógico (0-255 -> -1.0 a 1.0)
    float analog_y = (a->pad.Ly - 128) / 128.0f;
    if (fabs(analog_x) < 0.2f) analog_x = 0.0f;    // "Zona morta" para evitar movimento com o analógico parado
    if (fabs(analog_y) < 0.2f) analog_y = 0.0f;
    a->player->x += analog_x * a->player->speed;    // Atualiza a posição do jogador
    a->player->y -= analog_y * a->player->speed;
    if (a->pad.Buttons & PSP_CTRL_START)  return SDL_APP_SUCCESS;    // Se o botão START for pressionado, fecha o aplicativo
    
    
    if ((a->pad.Buttons & PSP_CTRL_CROSS) && !(a->prev_pad.Buttons & PSP_CTRL_CROSS)) {
        trigger_native_sound();
    }

    // ----------------------------------------------------
    // --- LÓGICA DE RENDERIZAÇÃO ---

    //SDL_Renderer *renderer = SDL_GetRenderer(a->window);
    SDL_SetRenderDrawColor(a->renderer, 0,0,0, 255);
    SDL_RenderClear(a->renderer);

    SDL_RenderTexture(a->renderer, a->background_texture, NULL, NULL);
    // ---------- DRAWING TEXT
    SDL_Color white = { 255, 255, 255, 255 };

    SDL_Surface* textSurface = TTF_RenderText_Solid(a->font, "Hello PSP!", strlen("Hello PSP!"), white);
    if (textSurface == NULL)
    {
        SDL_Log("Nao foi possivel criar a surperficie do texto: %s", SDL_GetError());
    } else {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(a->renderer, textSurface);
        if (textTexture == NULL)
        {
            SDL_Log("Nao foi possivel criar a texture do texto: %s", SDL_GetError());
        } else {
            SDL_FRect textRect;
            textRect.x = 20;
            textRect.y = 20;
            textRect.w = textSurface->w;
            textRect.h = textSurface->h;
            SDL_RenderTexture(a->renderer, textTexture, NULL, &textRect);

            SDL_DestroyTexture(textTexture);
        }
        SDL_DestroySurface(textSurface);
    }

    // ----------------------
    SDL_FRect player_rect = { a->player->x, a->player->y, 20, 20 };

    // Centraliza o retângulo na posição do jogador
    player_rect.x = a->player->x * 480 + (480 - player_rect.w) / 2;
    player_rect.y = a->player->y * 272 + (272 - player_rect.h) / 2;

    // Desenha o retângulo do jogador
    SDL_SetRenderDrawColor(a->renderer, 120, 255, 255, 255); // Branco
    SDL_RenderFillRect(a->renderer, &player_rect);
    
    // Mostra na tela tudo o que foi desenhado
    SDL_RenderPresent(a->renderer);
    //SDL_GL_SwapWindow(a->window);
    
    return SDL_APP_CONTINUE;
}

/*
::::::::::::::::::::::::::::::::::::::
LÓGICA DE INTERAÇÃO COM EVENTOS SDL
::::::::::::::::::::::::::::::::::::::
*/
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // A única coisa com que nos importamos aqui é se a SDL quer fechar o aplicativo
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }


    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	app_t *a = (app_t *)appstate;
	SDL_DestroyWindow(a->window);
	//SDL_GL_DestroyContext(a->context);
    //cleanup_audio();
    cleanup_native_audio();
	SDL_free(a);
	SDL_Quit();
}