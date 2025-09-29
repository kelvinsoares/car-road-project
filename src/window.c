#include <GL/glut.h>
#include "window.h"

// Tamanho inicial da janela
static int winW = 800;
static int winH = 600;

// Retorna largura da janela
int getWindowWidth() { 
    return winW; 
}

// Retorna altura da janela
int getWindowHeight() { 
    return winH; 
}

// Ajusta a janela quando o tamanho muda
void reshapeWindow(int w, int h) {
    winW = w;   // Atualiza largura
    winH = h;   // Atualiza altura
    if(h == 0) h = 1;  // Evita divisão por zero
    float aspect = (float)w / (float)h; // Proporção da tela

    glViewport(0, 0, w, h); // Área de desenho

    // Define projeção (perspectiva 3D)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 100.0);

    // Volta para o modo de desenho
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
