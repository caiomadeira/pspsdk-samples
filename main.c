//#include "sources/internal/pspvram.h"
//#include "sources/internal/pspgraphics.h"
#include "sources/internal/callbacks.h"

// STB_IMAGE
// Is a library to allows the use of textures images
// We can load the files with this lib
#define STB_IMAGE_IMPLEMENTATION
#include "sources/external/stb_image.h"

// Include Graphics Libraries // Once included in pspvram.c
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

// Wrapper called gu2gl.h
// This essentially convert the GU functions to OPENGL functions implementations
// with this the functions get more simpler to implement
#define GUGL_IMPLEMENTATION
#include <gu2gl.h>

// PSP Module Info
PSP_MODULE_INFO("Context Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

// Define PSP Width / Height
#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

// Colors
#define WHITE 0xFFFFFFFF
#define BLUE 0xFFFF0000
#define PURPLE 0x00BE3131
#define BLUE_LIGHT 0x00FFFF00
#define GREEN_LIGHT 0x0041FF00
#define PINK_LIGHT 0xC7D595FF

// Global variables
int running = 1;
static unsigned int __attribute__((aligned(16))) list[262144];

// TRIANGULE VERTEX
//  A vertice is a single point in a geometric shape
struct Vertex
{
    float u, v // texture coords
    unsigned int color;
    float x, y, z;
};

// Creating data rainbow data TRIANGLE
struct Vertex __attribute__((aligned(16))) triangle[3] = {
    {0x00EEFF00, 0.35f, 0.0f, -1.0f}, // color, x, y, z first point
    {0x00EEFF00, -0.35f, 0.0f, -1.0f},
    {0x00EEFF00, 0.0f, 0.5f, -1.0f},
};

// Creating a square with indices
struct Vertex __attribute__((aligned(16))) square_indexed[4] = { // A square will need two right triangles
   {0.0f, 0.0f, PINK_LIGHT, -0.25f, -0.25f, -1.0f}, // u, v, color, x, y, z
   {0.0f, 0.0f, PINK_LIGHT, -0.25f, 0.25f, -1.0f},
   {0.0f, 0.0f, PINK_LIGHT, 0.25f, 0.25f, -1.0f},
   {0.0f, 0.0f, PINK_LIGHT, 0.25f, -0.25f, -1.0f},
};
unsigned short __attribute__((aligned(16))) indices[6] = {
     0, 1, 2, 2, 3, 0 // square indices order
};
// Help to reset the Matrix and set translation
void reset_transform(float x, float y, float z)
{
    glMatrixMode(GL_MODEL);
    glLoadIdentity();

    ScePspFVector3 v = { x, y, z};
    gluTranslate(&v);
}

// TEXTURES HELP FUNCTIONS
unsigned int pow2(const unsigned int value)
{
    unsigned int power_of_two = 1/
    while(power_of_two < value)
    {
        power_of_two <<= 1;
    }
    return power_of_two/
}

void copy_texture_data(void* dest, const void* src, const in pW, const int width, const int height)
{
    for(unsigned int y = 0; y < height; y++)
    {
        for(unsigned int y = 0; x < width; y++)
        {
            ((unsigned int*)dest)[x + y * pW] = ((unsigned int*)src)[x + y * width];
        }
    }
}

void swizzle_fast(unsigned char* out, const unsigned char*in, const unsigned char *in, const unsigned int width, const unsigned int height)
{
    
}

int main() {
    // Boilerplate
    SetupCallbacks();

    // Initialize Graphics
    //initGraphics();
    guglInit(list);

    // initialize Matrices for transforms
    // Projection Matrix just tell if your are in 2D or 3D
    // When you're in 2D Graphics, you use ORTHOGRAPHIC projection matrix (one to one)
    // When you're in 3D Graphics, you (probably) use PERSPECTIVE projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Load Matrix and default values
    glOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f); // Require a orthographic projection. Basically defines a Box;

    // Matrix for camera transformations
    glMatrixMode(GL_VIEW);
    glLoadIdentity(); // Load Matrix and default values

    // Matrix for position of model
    glMatrixMode(GL_MODEL);
    glLoadIdentity(); // Load Matrix and default values

    //Main program loop
    while(running){
        //startFrame();
        guglStartFrame(list, GL_FALSE);

        // For 2D Mode, disable depth test
        glDisable(GL_DEPTH_TEST);
        //glDisable(GL_TEXTURE_2D);

        //Clear background to Green
        glClearColor(0xFF000000);
        glClearColor(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Triangle
        reset_transform(-0.5f, 0.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, 3, NULL, triangle);

        // Square
        //Move back to origin, and down 0.5f
        reset_transform(0.0f, -0.5f, 0.0f);
        // Draw Indexed Square
        glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_VERTEX_32BITF | GL_COLOR_8888 | GL_TRANSFORM_3D, 6, indices, square_indexed);

        //endFrame();
        guglSwapBuffers(GL_TRUE, GL_FALSE);
    }

    // Terminate Graphics
    //termGraphics();
    guglTerm();

    // Exit Game
    sceKernelExitGame();
    return 0;
}