#include "graphic.h"
#include "font.c"

char list[0x20000] __attribute__((aligned(64)));
static unsigned int __attribute__((aligned(16))) listFont[262144];
static unsigned int __attribute__((aligned(16))) listCube[262144];

struct Vertex3D __attribute__((aligned(16))) vertices[12*3] =
{
	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 0, 0xff7f0000,-1, 1, 1}, // 4
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5

	{0, 0, 0xff7f0000,-1,-1, 1}, // 0
	{1, 1, 0xff7f0000, 1, 1, 1}, // 5
	{0, 1, 0xff7f0000, 1,-1, 1}, // 1

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 0, 0xff7f0000, 1,-1,-1}, // 2
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6

	{0, 0, 0xff7f0000,-1,-1,-1}, // 3
	{1, 1, 0xff7f0000, 1, 1,-1}, // 6
	{0, 1, 0xff7f0000,-1, 1,-1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 0, 0xff007f00, 1,-1, 1}, // 3
	{1, 1, 0xff007f00, 1, 1, 1}, // 7

	{0, 0, 0xff007f00, 1,-1,-1}, // 0
	{1, 1, 0xff007f00, 1, 1, 1}, // 7
	{0, 1, 0xff007f00, 1, 1,-1}, // 4

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 0, 0xff007f00,-1, 1,-1}, // 3
	{1, 1, 0xff007f00,-1, 1, 1}, // 7

	{0, 0, 0xff007f00,-1,-1,-1}, // 0
	{1, 1, 0xff007f00,-1, 1, 1}, // 7
	{0, 1, 0xff007f00,-1,-1, 1}, // 4

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 0, 0xff00007f, 1, 1,-1}, // 1
	{1, 1, 0xff00007f, 1, 1, 1}, // 2

	{0, 0, 0xff00007f,-1, 1,-1}, // 0
	{1, 1, 0xff00007f, 1, 1, 1}, // 2
	{0, 1, 0xff00007f,-1, 1, 1}, // 3

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 0, 0xff00007f,-1,-1, 1}, // 7
	{1, 1, 0xff00007f, 1,-1, 1}, // 6

	{0, 0, 0xff00007f,-1,-1,-1}, // 4
	{1, 1, 0xff00007f, 1,-1, 1}, // 6
	{0, 1, 0xff00007f, 1,-1,-1}, // 5
};

// void initGu(void* frameBuffer0, void* frameBuffer1, void* depthBuffer, bool activateMore)
// {
//     sceGuInit();

//     //Set up buffers
//     sceGuStart(GU_DIRECT, list);
//     sceGuDrawBuffer(GU_PSM_8888, frameBuffer0, BUFFER_WIDTH);
//     sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, frameBuffer1, BUFFER_WIDTH);
//     sceGuDepthBuffer(depthBuffer, BUFFER_WIDTH);

//     //Set up viewport
//     sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
//     sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
//     sceGuEnable(GU_SCISSOR_TEST);
//     sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

//     //Set some stuff
//     sceGuDepthRange(65535, 0); //Use the full buffer for depth testing - buffer is reversed order

//     sceGuDepthFunc(GU_GEQUAL); //Depth buffer is reversed, so GEQUAL instead of LEQUAL
//     sceGuEnable(GU_DEPTH_TEST); //Enable depth testing
//     if (activateMore == true)
//     {
//         // for cube
//         sceGuFrontFace(GU_CW);
//         sceGuShadeModel(GU_SMOOTH);
//         sceGuEnable(GU_CULL_FACE);
//         sceGuEnable(GU_TEXTURE_2D);
//         sceGuEnable(GU_CLIP_PLANES);

//         // for font
//         // sceGuDisable(GU_DEPTH_TEST);
//         // sceGuShadeModel(GU_SMOOTH);
//         // sceGuEnable(GU_BLEND);
//         // sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
//         // // sceGuEnable(GU_TEXTURE_2D);
//         // sceGuTexMode(GU_PSM_8888, 0, 0, 0);
//         // sceGuTexImage(0, 256, 128, 256, font);
//         // sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
//         // sceGuTexEnvColor(0x0);
//         // sceGuTexOffset(0.0f, 0.0f);
//         // sceGuTexScale(1.0f / 256.0f, 1.0f / 128.0f);
//         // sceGuTexWrap(GU_REPEAT, GU_REPEAT);
//         // sceGuTexFilter(GU_NEAREST, GU_NEAREST);
//     }
// 	sceGuFinish();
// 	sceGuSync(0,0);
// 	sceGuDisplay(GU_TRUE);
// }

void initGu(void* frameBuffer0, void* frameBuffer1, void* depthBuffer, bool isForFont) {
    sceGuInit();
    sceGuStart(GU_DIRECT, list);
    
    // Setup básico
    sceGuDrawBuffer(GU_PSM_8888, frameBuffer0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, frameBuffer1, BUFFER_WIDTH);
    sceGuDepthBuffer(depthBuffer, BUFFER_WIDTH);
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    if (isForFont) {
        // Configurações específicas para fontes
	    sceGuDepthRange(0xc350,0x2710);
        sceGuDisable(GU_DEPTH_TEST);
        sceGuEnable(GU_BLEND);
        sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuEnable(GU_TEXTURE_2D);
        sceGuTexMode(GU_PSM_8888, 0, 0, 0);
        sceGuTexImage(0, 256, 128, 256, font);
        sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
        sceGuTexEnvColor(0x0);
        sceGuTexScale(1.0f / 256.0f, 1.0f / 128.0f);
        sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    } else {
        // Configurações específicas para objetos 3D (cubo)
        sceGuDepthRange(65535, 0);
        sceGuEnable(GU_DEPTH_TEST);
        sceGuFrontFace(GU_CW);
        sceGuShadeModel(GU_SMOOTH);
        sceGuEnable(GU_CULL_FACE);
        sceGuEnable(GU_TEXTURE_2D);
        sceGuEnable(GU_CLIP_PLANES);
    }
    
    sceGuFinish();
    sceGuSync(0, 0);
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

void startFrameCube(u32 backgroundColor)
{
    sceGuStart(GU_DIRECT, list);
    if (backgroundColor != 0)
        sceGuClearColor(backgroundColor); // White background
    sceGuClearDepth(0); 
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
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
FONT
************************
*/

void initGuForFont()
{
    sceGuInit();

    //Set up buffers
	sceGuStart(GU_DIRECT, listFont); // verificar
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);

    //Set up viewport
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
	sceGuScissor(0, 0, SCR_WIDTH,SCR_HEIGHT);

    //Set some stuff
	sceGuDepthRange(0xc350,0x2710);

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

// CUBE
// Variaveis de renderização do cubo
int textureImageWidth = 64;
int textureImageHeight = 64;
int textureImageStride = 64;
float rotZConst = 1.32f;
float rotYConst = 0.5f;
float rotXConst = 0.9f;
float ZDepth = 75.0f;
float flatCube =  16.0f/9.0f;
float verticeProximityScreen = 0.5f;
    

void renderCube(float posX, float posY, float posZ)
{
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

		sceGumMatrixMode(GU_PROJECTION);
		sceGumLoadIdentity();
		// cube perspective and pos and rot
		sceGumPerspective(ZDepth, flatCube, verticeProximityScreen, 1000.0f);
		sceGumMatrixMode(GU_VIEW);
		sceGumLoadIdentity();
		sceGumMatrixMode(GU_MODEL);
		sceGumLoadIdentity();
		{
			// Initial pos
            ScePspFVector3 pos = { posX, posY, posZ };
			sceGumTranslate(&pos);
		}

		// // setup texture and draw cube
		sceGuTexMode(GU_PSM_5650, 0,0,0);
		sceGuTexImage(0,textureImageWidth,textureImageHeight,textureImageStride,logo_start);
		sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB); // Define como a textura será combinada com a cor base dos vértices.
		sceGuTexEnvColor(0xffff00); // sinceramente, nao vi mt diferenca
		sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		sceGuTexScale(1.0f,1.0f);
		sceGuTexOffset(0.0f,0.0f);
		sceGuAmbientColor(0xffffffff);
		sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3,0,vertices);
		sceGuFinish();
		sceGuSync(0,0);
}

int calculateTextWidth(const char* text, int fontwidth) {
    int width = 0;
    while (*text) {
        width += fontwidthtab[(unsigned char)*text++];
    }
    return width;
}

void configureGuForMenu() {
    sceGuStart(GU_DIRECT, listCube);

    sceGuDisable(GU_DEPTH_TEST);
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, 0);
    sceGuTexImage(0, 256, 128, 256, font);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexOffset(0.0f, 0.0f);
    sceGuTexScale(1.0f / 256.0f, 1.0f / 128.0f);

    sceGuFinish();
    sceGuSync(0, 0);
}

void configureGuForCube() {
    sceGuStart(GU_DIRECT, list);

    sceGuEnable(GU_DEPTH_TEST);
    sceGuFrontFace(GU_CW);
    sceGuShadeModel(GU_SMOOTH);
    sceGuEnable(GU_CULL_FACE);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_CLIP_PLANES);

    sceGuFinish();
    sceGuSync(0, 0);
}
