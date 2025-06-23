#include "include/common.h"
#include "include/player.h"

//#include <SDL3_image/SDL_image.h> // A de imagem está na sua própria pasta!
//#include <SDL3/SDL_oldnames.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>


typedef struct app {
	SDL_Window *window;
	SDL_GLContext context;
    GameState currentstate;
    SceCtrlData pad;
    Player *player;
    //SDL_Texture *background_texture;
} app_t;
/*
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *  ON READY
 * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	app_t *a;

	// Ao inves de SDL_NIT_VIDEO | SDL_INIT_GAMEPAD apenas uso o INIT_VIDEO pois os controles usarei a lib manual do psp
	if (!SDL_Init(SDL_INIT_VIDEO))
    {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
        return SDL_APP_FAILURE;
    }


    // INicia a imagem de fundo
    //if (IMG_Init(IMG_INIT_PNG) == 0) return SDL_APP_FAILURE;

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


	//a->window = SDL_CreateWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL); // janela de renderizacao c open_gl
	a->window = SDL_CreateWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL); // janela de renderizacao c open_gl
    if (a->window == NULL)
    {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
        return SDL_APP_FAILURE;
    }


	// Config basica do OpenGL (1.2)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	a->context = SDL_GL_CreateContext(a->window);
	if (a->context == NULL)
    {
        pspDebugScreenInit();
        pspDebugScreenClear();
        pspDebugScreenSetXY(20, 7);
        pspDebugScreenPrintf(SDL_GetError());
        SDL_Delay(4000);
        return SDL_APP_FAILURE;
    }

	// ativa o v-sync
	SDL_GL_SetSwapInterval(1);

    // Inicializando o controle nativo
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    // drawStr("Teste teste", 10, (WINDOW_HEIGHT / 2) + 20, (unsigned int) RAW_BLACK, 0);

    // // Carrega img de fundo
    // SDL_Renderer *renderer = SDL_GetRenderer(a->window);
    // // carrega a img
    // SDL_Surface *bg_surface = IMG_Load("background.png");
    // if (!bg_surface) {
    //     pspDebugScreenInit();
    //     pspDebugScreenClear();
    //     //pspDebugScreenSetXY(20, 7);
    //     pspDebugScreenPrintf(SDL_GetError());
    //     SDL_Delay(4000);
            
    //     return SDL_APP_FAILURE;
    // }
    // aqui importante: converte a superficie numa texture otimizada pra a GPU e guarda em um buffer
    // a->background_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    // SDL_DestroySurface(bg_surface); // libera a superficie da mem. ja que n precisamos mais

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


	/* return app state */
	*appstate = (void *)a;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	app_t *a = (app_t *)appstate;
	SDL_DestroyWindow(a->window);
	SDL_GL_DestroyContext(a->context);
	SDL_free(a);
	SDL_Quit();
}

/*
 *:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 *  GAME LOOP
 * :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    app_t *a = (app_t *)appstate;

    // --- LOGICA DE CONTROLES
    readButtonState(&a->pad, 1);
    float analog_x = (a->pad.Lx - 128) / 128.0f;    // Normaliza o valor do analógico (0-255 -> -1.0 a 1.0)
    float analog_y = (a->pad.Ly - 128) / 128.0f;
    if (fabs(analog_x) < 0.2f) analog_x = 0.0f;    // "Zona morta" para evitar movimento com o analógico parado
    if (fabs(analog_y) < 0.2f) analog_y = 0.0f;
    a->player->x += analog_x * a->player->speed;    // Atualiza a posição do jogador
    a->player->y -= analog_y * a->player->speed;
    if (a->pad.Buttons & PSP_CTRL_START)  return SDL_APP_SUCCESS;    // Se o botão START for pressionado, fecha o aplicativo
    // ----------------------------------------------------
    // --- LÓGICA DE RENDERIZAÇÃO ---

    // SDL_Renderer *renderer = SDL_GetRenderer(a->window);
    // SDL_SetRenderDrawColor(renderer, 0,0,0, 255);
    // SDL_RenderClear(renderer);

    // SDL_RenderTexture(renderer, a->background_texture, NULL, NULL);
    // SDL_FRect player_rect = { a->player->x, a->player->y, 20, 20 };

    // // Centraliza o retângulo na posição do jogador
    // player_rect.x = a->player->x * 480 + (480 - player_rect.w) / 2;
    // player_rect.y = a->player->y * 272 + (272 - player_rect.h) / 2;

    // // Desenha o retângulo do jogador
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Branco
    // SDL_RenderFillRect(renderer, &player_rect);
    
    // // Mostra na tela tudo o que foi desenhado
    // SDL_RenderPresent(renderer);
    int w, h;
    float aspect;
    SDL_GetWindowSizeInPixels(a->window, &w, &h);
    aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Usamos uma visão ortográfica 2D simples
    glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Limpa a tela com um fundo azul escuro
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Salva a matriz de transformação atual
    glPushMatrix();
    {
        // Move o "pincel" do OpenGL para a posição do nosso jogador
        glTranslatef(a->player->x, a->player->y, 0.0f);

        // Desenha o retângulo branco
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-0.05f,  0.05f);
            glVertex2f( 0.05f,  0.05f);
            glVertex2f( 0.05f, -0.05f);
            glVertex2f(-0.05f, -0.05f);
        glEnd();
    }
    // Restaura a matriz de transformação original
    glPopMatrix();
    // Mostra na tela o que foi desenhado
    SDL_GL_SwapWindow(a->window);
    
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
