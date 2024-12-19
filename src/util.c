#include "util.h"
#include "graphic.h"
#include "common.h"

// Função para calcular a largura do texto com base na largura dos caracteres.
int calculateTextWidth(const char* text, int fontwidth) 
{
    int len = strlen(text);
    int totalWidth = 0;

    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)text[i];
        if (c < 32 || c >= 128) {
            c = 0;  // Caracteres fora do intervalo ASCII imprimível.
        }
        totalWidth += fontwidth ? fontwidth : fontwidthtab[c];  // Usa a largura do caractere ou o valor de fontwidth.
    }

    return totalWidth;
}