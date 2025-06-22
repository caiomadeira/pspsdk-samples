#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "unfinished/stb_image_write.h"
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<time.h>

/*
Escala cinza = 1 canal de cor
Imagem colorida = 3 canais de cor (R,G,B)
*/

typedef struct {
    double x; // usei double pra maior precisao
    double y;
} Vector2;

static int p[512];

// fisher-yates algorithm
static void shuffle(int* v, int n)
{
    for(int i = n-1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int aux = v[i];
        v[i] = v[j];
        v[j] = aux;
    }
}

void init_noise(unsigned int seed)
{
    srand(seed); 

    for(int i = 0; i < 256; i++) { p[i] = i ;}
    shuffle(p, 256); // o shuffle vai embaralhar os valores da tabela de permutações
    for(int i = 0; i < 256; i++) {  p[256 + i] = p[i]; }
}

// retorna um dos 4 vetores gradientes (constantes) 
Vector2 getvecconst(int v)
{
    // v & 3 eh uma forma bitwise (e rapida) de calcular v % 4 (v resto 4)
    switch(v & 3)
    {
        case 0: return (Vector2){1.0, 1.0};
        case 1: return (Vector2){-1.0, 1.0};
        case 2: return (Vector2){-1.0, -1.0};
        default: return (Vector2){1.0, -1.0};
    }
}

// funcao ease curve (fase) de suavização: 6t^5 - 15t^4 + 10t^3
// versao nao otimizada (muitas multiplicacoes)
static double fade(double t) { return 6*t*t*t*t*t - 15*t*t*t*t + 10*t*t*t; }

// funcao de interpolacao linear (lerp)
static double lerp(double t, double a1, double a2) { return a1 + t * (a2 - a1); }

// funcao p/ calcular o dot product de dois vetores 2d
static double dot(Vector2 v1, Vector2 v2) { return v1.x * v2.x + v1.y + v2.y; }

double noise2d(double x, double y)
{
    // 1 garante que x e y fique no intervalo [0...255] usando a operação bitwise & 255 (eq. a % 256)
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    // 2 aqui encontra as coordenadas fracionarias, ou seja a posição relativa dentro da célula)
    // xf e yf estarao no intervalo [0, 1]
    double xf = x - floor(x);
    double yf = y - floor(y);

    // 3. Calcula os vetores de distância do ponto (x,y) para cada canto da célula
    Vector2 topRight    = {xf - 1.0, yf - 1.0};
    Vector2 topLeft     = {xf,       yf - 1.0};
    Vector2 bottomRight = {xf - 1.0, yf      };
    Vector2 bottomLeft  = {xf,       yf      };
    // 4. Seleciona um valor da tabela de permutação para cada canto
    int valueTopRight    = p[p[X + 1] + Y + 1];
    int valueTopLeft     = p[p[X] + Y + 1];
    int valueBottomRight = p[p[X + 1] + Y];
    int valueBottomLeft  = p[p[X] + Y];

    // 5. Calcula o produto escalar entre o vetor de distância e o vetor gradiente constante
    double dotTopRight    = dot(topRight, getvecconst(valueTopRight));
    double dotTopLeft     = dot(topLeft, getvecconst(valueTopLeft));
    double dotBottomRight = dot(bottomRight,getvecconst(valueBottomRight));
    double dotBottomLeft  = dot(bottomLeft,getvecconst(valueBottomLeft));
    
    // 6. Suaviza as coordenadas fracionárias usando a função "Fade"
    double u = fade(xf);
    double v = fade(yf);

    // 7. Interpola os 4 valores para obter o resultado final
    return lerp(u,
        lerp(v, dotBottomLeft, dotTopLeft),
        lerp(v, dotBottomRight, dotTopRight)
    );
}

// fractal brownian motion
double fbm(double x, double y, int numoctaves)
{
    double result = 0.0;
    double amplitude = 1.0;
    double frequency = 0.005;

    for(int i = 0; i < numoctaves; i++)
    {
        double n = amplitude * noise2d(x * frequency, y * frequency);
        result += n;

        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return result;
}

int main(void) {

    int width = 500;
    int height = 500;

    init_noise(time(NULL));
    /*
    Com 3 canais de cores, o buffer da imagem precisa de 3 bytes pra cada pixel (1 byte pra R, 1 byte pra B, 1 byte pra G)
    */
   const int channels = 3;
    unsigned char* img = (unsigned char*)malloc(width * height * channels); // vetor de bytes (onde cada byte eh o brilho de cada pixel na escala cinza)
    if (img)
    {
        double freq = 0.01; // frequencia do ruido. valores menores = zoom out; valores maiores = zoom in
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                // gera 3 valores de rudio, um pra cada canal c/ grandes offsets
                // usamos grandes offsets pra garantir q estamos pegando valores de partes completamente diferentes e do universo gerado pelo perlin noise
                // com isso, fazemos c q os 3 mapas de ruido napo parecam relacionados
                double r_noise = fbm(x + 1000, y + 1000, 10);
                double g_noise = fbm(x + 2000, y - 2000, 10);
                double b_noise = fbm(x - 3000, y + 3000, 10);

                // normaliza cada alor pra [0, 255]
                unsigned char r = (unsigned char)round(((r_noise + 1.0) / 2.0) * 255.0);
                unsigned char g = (unsigned char)round(((g_noise + 1.0) / 2.0) * 255.0);
                unsigned char b = (unsigned char)round(((b_noise + 1.0) / 2.0) * 255.0);

                // define o pixel da img
                int index = (y * width + x) * channels;
                img[index] = r;
                img[index + 1] = g;
                img[index + 2] = b;
            }
        }

        if (!stbi_write_png("test1.png", width, height, channels, img, width * channels)) {
            fprintf(stderr, "erro ao salvar imagem PNG.\n");
            free(img);
            return 1;
        }

        printf("Img salva.\n");
        free(img);
    }
    return 0;
}