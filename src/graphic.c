#include "common.h"
#include "graphic.h"
#include "font.c"

char list[0x20000] __attribute__((aligned(64)));

void initGu(){
    sceGuInit();

    //Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,(void*)0x88000,BUFFER_WIDTH);
    sceGuDepthBuffer((void*)0x110000,BUFFER_WIDTH);

    //Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Set some stuff
    sceGuDepthRange(65535, 0); //Use the full buffer for depth testing - buffer is reversed order

    sceGuDepthFunc(GU_GEQUAL); //Depth buffer is reversed, so GEQUAL instead of LEQUAL
    sceGuEnable(GU_DEPTH_TEST); //Enable depth testing

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu()
{
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame(u32 backgroundColor)
{
    sceGuStart(GU_DIRECT, list);
    if (backgroundColor != 0)
        sceGuClearColor(backgroundColor); // White background
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame()
{
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

/*
************************
VERIFIEDS SHAPE FUNCTIONS
************************
*/

void drawRect(float x, float y, float w, float h) 
{
    Vertex* vertices = (Vertex*)sceGuGetMemory(2 * sizeof(Vertex));

    vertices[0].x = x;
    vertices[0].y = y;

    vertices[1].x = x + w;
    vertices[1].y = y + h;

    sceGuColor(0xFF0000FF); // Red, colors are ABGR
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
}

void drawTriangle(float baseX, float baseY, float width, float height, u32 color)
{
    Vertex* v = (Vertex*)sceGuGetMemory(3 * sizeof(Vertex));

    v[0].x = baseX;
    v[0].y = baseY - height;

    v[1].x = baseX - (width /2);
    v[1].y = baseY;

    v[2].x = baseX + (width /2);
    v[2].y = baseY;

    sceGuColor(color);
    /*
    GU_TRIANGLES - Representa triangulos independentes onde cada conjunto de 3 vertices forma um triangulo
    GU_SPRITES: Representa quadriláteros desenhados como dois triângulos conectados. É usado principalmente para desenhar imagens 2D (texturas) ou objetos planos.

    */
    sceGuDrawArray(GU_TRIANGLES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 3, 0, v);
}

/*
************************
FONT
************************
*/

void initGuForFont()
{
    sceGuInit();
	sceGuStart(GU_DIRECT, listFont);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDisable(GU_DEPTH_TEST);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexImage(0, 256, 128, 256, font);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexEnvColor(0x0);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuTexScale(1.0f / 256.0f, 1.0f / 128.0f);
	sceGuTexWrap(GU_REPEAT, GU_REPEAT);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuFinish();
	sceGuSync(0,0);
	sceGuDisplay(GU_TRUE);
}

// fw = fontwidth
void drawStr(const char* text, int x, int y, unsigned int color, int fw)
{
    int len = (int)strlen(text);
    if (!len) // se a string estiver vazia, sai da funcao
        return;

    // definimos uma struct dentro da funcao
    typedef struct
    {
        float s, t;
        unsigned int c; // char
        float x, y, z;
    } VERT;

    VERT* v = sceGuGetMemory(sizeof(VERT) * 2 * len);
    if (v != NULL)
    {
        int i; 
        for(i = 0; i < len; i++)
        {
            // convert o caractere para unsigned char (0, 255) (muda o range)
            unsigned char c = (unsigned char)text[i];
            if (c < 32) // se o caractere esta fora do intervalo imprimivel
                c = 0; // substitui por um caractere vazio ou estpaco na fonte
            else if (c >= 128)
                c = 0;
            int tx = (c & 0x0F) << 4;
            int ty = (c & 0xF0);

            VERT* v0 = &v[i*2+0];
            VERT* v1 = &v[i*2+1];
            
            v0->s = (float)(tx + (fw ? ((16 - fw) >> 1) : ((16 - fontwidthtab[c]) >> 1)));
            v0->t = (float)(ty);
            v0->c = color;
            v0->x = (float)(x);
            v0->y = (float)(y);
            v0->z = 0.0f;

            v1->s = (float)(tx + 16 - (fw ? ((16 - fw) >> 1) : ((16 - fontwidthtab[c]) >> 1)));
            v1->t = (float)(ty + 16);
            v1->c = color;
            v1->x = (float)(x + (fw ? fw : fontwidthtab[c]));
            v1->y = (float)(y + 16);
            v1->z = 0.0f;

            x += (fw ? fw : fontwidthtab[c]);
        }
        sceGumDrawArray(GU_SPRITES, 
        GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
        len * 2, 0, v);
    }
}

void drawStrWithEffectRainbowEffect(const char* text, int x, int y, int fontwidth, float *t)
{
    *t += 0.1f;

    unsigned int c = 0xFF000000 |
        (unsigned int)((sinf(*t * 0.393f + 0.086f) / 2.0f + 0.5f) * 255.0f) << 16 |
        (unsigned int)((sinf(*t * 0.444f + 0.854f) / 2.0f + 0.5f) * 255.0f) <<  8 |
        (unsigned int)((sinf(*t * 0.117f + 1.337f) / 2.0f + 0.5f) * 255.0f) <<  0;

    drawStr(text, x, y, c, fontwidth);
}

const char* drawStr2(const char* text, int x, int y, unsigned int color, int fw)
{
    int len = (int)strlen(text);
    if (!len) // se a string estiver vazia, sai da funcao
        return "";

    // definimos uma struct dentro da funcao
    typedef struct
    {
        float s, t;
        unsigned int c; // char
        float x, y, z;
    } VERT;

    VERT* v = sceGuGetMemory(sizeof(VERT) * 2 * len);
    int i; 
    for(i = 0; i < len; i++)
    {
        // convert o caractere para unsigned char (0, 255) (muda o range)
        unsigned char c = (unsigned char)text[i];
        if (c < 32) // se o caractere esta fora do intervalo imprimivel
            c = 0; // substitui por um caractere vazio ou estpaco na fonte
        else if (c >= 128)
            c = 0;
        int tx = (c & 0x0F) << 4;
        int ty = (c & 0xF0);

        VERT* v0 = &v[i*2+0];
        VERT* v1 = &v[i*2+1];
        
        v0->s = (float)(tx + (fw ? ((16 - fw) >> 1) : ((16 - fontwidthtab[c]) >> 1)));
        v0->t = (float)(ty);
        v0->c = color;
        v0->x = (float)(x);
        v0->y = (float)(y);
        v0->z = 0.0f;

        v1->s = (float)(tx + 16 - (fw ? ((16 - fw) >> 1) : ((16 - fontwidthtab[c]) >> 1)));
        v1->t = (float)(ty + 16);
        v1->c = color;
        v1->x = (float)(x + (fw ? fw : fontwidthtab[c]));
        v1->y = (float)(y + 16);
        v1->z = 0.0f;

        x += (fw ? fw : fontwidthtab[c]);
    }
    sceGumDrawArray(GU_SPRITES, 
    GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, len * 2, 0, v);
    return text;
}