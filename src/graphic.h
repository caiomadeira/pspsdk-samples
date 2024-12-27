#ifndef graphic_h
#define graphic_h
#include "common.h"

// Dimensões do framebuffer e tela
#define BUFFER_WIDTH 512         // Largura do buffer de imagem em pixels
#define BUFFER_HEIGHT 272        // Altura do buffer de imagem em pixels
#define SCREEN_WIDTH 480         // Largura visível da tela
#define SCREEN_HEIGHT BUFFER_HEIGHT // Altura visível da tela

#define SCR_HEIGHT SCREEN_HEIGHT
#define SCR_WIDTH SCREEN_WIDTH
#define BUF_WIDTH BUFFER_WIDTH 
#define BUF_HEIGHT BUFFER_HEIGHT

// Estrutura para representar um vértice
typedef struct {
    unsigned short u, v;
    short x, y, z;
	//u32 color;
} Vertex;

// TEXTURE TEST - LOGO
extern unsigned char logo_start[];
extern unsigned char face_start[];


struct Vertex3D
{
	float u, v;
	unsigned int color;
	float x,y,z;
};

//extern struct Vertex3D __attribute__((aligned(16))) cubeVertices[12*3];
extern struct Vertex3D __attribute__((aligned(16))) buildingVertices[12*3];

// FUNCTIONS
void renderPlayer(float posX, float posY, float posZ);
void renderBuilding(float posX, float posY, float posZ, float scaleX, float scaleY, float scaleZ, struct Vertex3D *vertices);
void renderScene(float posX, float posY, float posZ);

// Inicializa o pipeline gráfico
void initGu(void* frameBuffer0, void* frameBuffer1, void* depthBuffer);

// Finaliza o pipeline gráfico
void endGu();

// Inicia o desenho de um frame
void startFrame();
void startFrameCube(u32 backgroundColor);
void startFrameCubeNoBackground(void);
void startFramePlane(u32 backgroundColor);
void startFrameCubeAndPlane();
// Finaliza e apresenta o frame
void endFrame();

void drawTriangle(float baseX, float baseY, float width, float height, u32 color);
void drawRect(float x, float y, float w, float h);

// for font
void initGuForFont();
void drawStr(const char* text, int x, int y, unsigned int color, int fw);
const char* drawStr2(const char* text, int x, int y, unsigned int color, int fw);
void drawStrWithEffectRainbowEffect(const char* text, int x, int y, int fontwidth, float* t);  // Alteração para aceitar float* t
int calculateTextWidth(const char* text, int fontwidth);

static unsigned int __attribute__((aligned(16))) listFont[262144];

static int fontwidthtab[128] = {
	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10, 
	10, 10, 10, 10,

	10, 10, 10, 10, 
	10, 10, 10, 10,
	10, 10, 10, 10,
	10, 10, 10, 10,

	10,  6,  8, 10, //   ! " #
	10, 10, 10,  6, // $ % & '
	10, 10, 10, 10, // ( ) * +
	 6, 10,  6, 10, // , - . /

	10, 10, 10, 10, // 0 1 2 3
	10, 10, 10, 10, // 6 5 8 7
	10, 10,  6,  6, // 10 9 : ;
	10, 10, 10, 10, // < = > ?

	16, 10, 10, 10, // @ A B C
	10, 10, 10, 10, // D E F G
	10,  6,  8, 10, // H I J K
	 8, 10, 10, 10, // L M N O

	10, 10, 10, 10, // P Q R S
	10, 10, 10, 12, // T U V W
	10, 10, 10, 10, // X Y Z [
	10, 10,  8, 10, // \ ] ^ _

	 6,  8,  8,  8, // ` a b c
	 8,  8,  6,  8, // d e f g
	 8,  6,  6,  8, // h i j k
	 6, 10,  8,  8, // l m n o

	 8,  8,  8,  8, // p q r s
	 8,  8,  8, 12, // t u v w
	 8,  8,  8, 10, // x y z {
	 8, 10,  8, 12  // | } ~  
};

#define PIXEL_SIZE (4)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2)


#endif
