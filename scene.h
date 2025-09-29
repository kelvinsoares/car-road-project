#ifndef SCENE_H
#define SCENE_H

#include <GL/glut.h>

// Inicialização
void initGL();
void initScene();
// void loadTextures();

// Renderização
void displayScene();
// void drawGroundGrid(float size, int divisions);

// Timer
void initTimer();
void idleTimer();

// // --- Skybox ---
// typedef struct {
//     GLuint textures[6]; // IDs das 6 texturas
//     float size;
// } Skybox;

// extern Skybox skybox;

// void loadSkybox();
// void renderSkybox(float camX, float camY, float camZ);

// // Variáveis da câmera (globais)
// extern float camDistance;
// extern float camAngleH;
// extern float camAngleV;
// extern float camSpeed;
// extern float zoomSpeed;

// // Texturas (expostas se precisar em outros arquivos)
// extern unsigned int texGrassDiffuse;

#endif
