#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "include/tinyobj_loader_c.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include<stdio.h> // usado p/ ler o arquivo do modelo

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

typedef struct model_s {
	tinyobj_attrib_t attrib;
	tinyobj_shape_t *shapes;
	tinyobj_material_t *materials;
	size_t num_shapes;
	size_t num_materials;
} model_t;

// strucct principal do app + variavel de rotacao + variavel do modelo
typedef struct app {
	SDL_Window *window;
	SDL_GLContext context;
	model_t model;
	float rotation_y;
} app_t;

// le o modelo p/ memoria
static void read_model(const char* filename, char** contents, size_t* contents_len)
{
	long length = 0;
	FILE *f = fopen(filename, "rb");
	*contents = NULL;
	*contents_len = 0;

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		*contents = (char*)SDL_malloc(length+1);
		if (*contents)
		{
			fread(*contents, 1, length, f);
			(*contents)[length] = '\0';
			*contents_len = length;
		}
		fclose(f);
	}
}


/*
::::::::::::::::::::::::::::::::::::::
GAME INIT
::::::::::::::::::::::::::::::::::::::
*/
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    app_t *a;

    /* init sdl */
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        return SDL_APP_FAILURE;

    /* allocate app state */
    a = (app_t*)SDL_calloc(1, sizeof(app_t));
    if (a == NULL)
        return SDL_APP_FAILURE;

    /* create window */
    a->window = SDL_CreateWindow("Modelo 3D - Kenney.nl", 480, 272, SDL_WINDOW_OPENGL);
    if (a->window == NULL)
        return SDL_APP_FAILURE;

    /* init gl */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    a->context = SDL_GL_CreateContext(a->window);
    if (a->context == NULL)
        return SDL_APP_FAILURE;

    /* set swap interval (vsync) */
    SDL_GL_SetSwapInterval(1);

    // --- CONFIGURAÇÃO INICIAL DO OPENGL PARA 3D ---
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // --- CARREGAMENTO DO MODELO .OBJ (CHAMADA CORRIGIDA) ---
    const char *filename = "assets/altar-stone.obj";
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;

    // A função é chamada com o NOME do arquivo.
    // Passamos NULL para `file_reader` e `ctx` para que a biblioteca use seu leitor de arquivo padrão.
    // Esta chamada tem 9 argumentos, como esperado pelo compilador.
    int ret = tinyobj_parse_obj(&a->model.attrib, &a->model.shapes, &a->model.num_shapes,
                                &a->model.materials, &a->model.num_materials,
                                filename, NULL, NULL, flags);

    if (ret != TINYOBJ_SUCCESS) {
        SDL_Log("Falha ao carregar 'app/assets/altar-stone.obj'. Certifique-se de que ele está na mesma pasta que o EBOOT.PBP.");
    }
    // ---------------------------------------------------------

    /* return app state */
    *appstate = (void *)a;

    return SDL_APP_CONTINUE;
}

/*
::::::::::::::::::::::::::::::::::::::
FINALIZACAO DO APP
::::::::::::::::::::::::::::::::::::::
*/
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	app_t *a = (app_t *)appstate;
    // --- LIBERA A MEMÓRIA DO MODELO ---
    tinyobj_attrib_free(&a->model.attrib);
    tinyobj_shapes_free(a->model.shapes, a->model.num_shapes);
    tinyobj_materials_free(a->model.materials, a->model.num_materials);
    // ---------------------------------
	SDL_DestroyWindow(a->window);
	SDL_GL_DestroyContext(a->context);
	SDL_free(a);
	SDL_Quit();
}
/*
::::::::::::::::::::::::::::::::::::::
 GAME LOOP
::::::::::::::::::::::::::::::::::::::
*/
SDL_AppResult SDL_AppIterate(void *appstate)
{
    int w, h;
    float aspect;
    app_t *a = (app_t *)appstate;

    a->rotation_y += 0.5f; // Atualiza a rotação a cada frame
    if(a->rotation_y > 360.0f) a->rotation_y -= 360.0f;

    SDL_GetWindowSizeInPixels(a->window, &w, &h);
    aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);

    // --- CONFIGURA A CÂMERA (PROJEÇÃO DE PERSPECTIVA 3D) ---
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho foi substituído por glFrustum para criar uma visão em perspectiva
    glFrustum(-aspect * 0.1f, aspect * 0.1f, -0.1f, 0.1f, 0.1f, 100.0f);
    // --------------------------------------------------------

    // --- POSICIONA OS OBJETOS E A CÂMERA NO MUNDO ---
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Move a câmera para trás para podermos ver o objeto
    // e um pouco para baixo para centralizar verticalmente o modelo do altar.
    // Gira o modelo no eixo Y.
    glTranslatef(0.0f, -0.2f, -2.5f);
    glRotatef(a->rotation_y, 0.0f, 1.0f, 0.0f);
    // ------------------------------------------------

    /* clear screen */
    glClearColor(0.3f, 0.5f, 0.8f, 1.0f); // Um azul claro de fundo
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // --- DESENHA O MODELO .OBJ ---
    glBegin(GL_TRIANGLES);
    {
        // Define uma cor padrão para o modelo
        glColor3f(0.8f, 0.8f, 0.7f);

        // Percorre cada face (triângulo) do modelo
        size_t index_offset = 0;
        for (size_t f = 0; f < a->model.attrib.num_face_num_verts; f++) {
            // Todos os nossos polígonos são triângulos por causa da flag TINYOBJ_FLAG_TRIANGULATE
            size_t fv = 3;

            // Percorre cada um dos 3 vértices do triângulo
            for (size_t v = 0; v < fv; v++) {
                tinyobj_vertex_index_t idx = a->model.attrib.faces[index_offset + v];

                // Pega as coordenadas x, y, z do vértice
                float vx = a->model.attrib.vertices[3 * idx.v_idx + 0];
                float vy = a->model.attrib.vertices[3 * idx.v_idx + 1];
                float vz = a->model.attrib.vertices[3 * idx.v_idx + 2];

                // Envia o vértice para o OpenGL desenhar
                glVertex3f(vx, vy, vz);
            }
            index_offset += fv;
        }
    }
    glEnd();
    // ---------------------------

    /* swap buffers */
    SDL_GL_SwapWindow(a->window);

    return SDL_APP_CONTINUE;
}

/*
::::::::::::::::::::::::::::::::::::::
LOGICA DE INTERAÇÃO COM CONTROLES
::::::::::::::::::::::::::::::::::::::
*/
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	app_t *a = (app_t *)appstate;

	switch (event->type)
	{
		case SDL_EVENT_QUIT:
			return SDL_APP_SUCCESS;

		case SDL_EVENT_GAMEPAD_ADDED:
			SDL_OpenGamepad(event->cdevice.which);
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			if (event->gbutton.button == SDL_GAMEPAD_BUTTON_START)
				return SDL_APP_SUCCESS;
			break;
	}

	return SDL_APP_CONTINUE;
}
