#include <GL/glut.h>
#include <stdbool.h>
#include <math.h>
#include "scene.h"

extern float camAngleH;   // Ângulo horizontal da câmera
extern float camAngleV;   // Ângulo vertical da câmera
extern float camDistance; // Distância da câmera
extern float camSpeed;    // Velocidade de movimento da câmera
extern float zoomSpeed;   // Velocidade do zoom

extern float carX;        // Posição X do carro
extern float carY;        // Posição Y do carro
extern float carZ;        // Posição Z do carro
extern float carAngle;    // Ângulo/direção do carro
extern float carSpeed;    // Velocidade do carro
extern float carTurnSpeed;// Velocidade de giro do carro

// Array que guarda se uma tecla está apertada (true) ou não (false)
static bool keys[256] = { false };

// Quando uma tecla é apertada
void keyboardDown(unsigned char key, int x, int y){
    keys[key] = true;        // Marca tecla como pressionada
    if(key == 27) exit(0);   // Se for ESC (27), fecha o programa
}

// Quando uma tecla é solta
void keyboardUp(unsigned char key, int x, int y){
    keys[key] = false;       // Marca tecla como liberada
}

// Atualiza o movimento do carro
void processCarMovement(float deltaTime){
    float moveStep = carSpeed * deltaTime;      // Quanto o carro anda
    float turnStep = carTurnSpeed * deltaTime;  // Quanto o carro gira

    // Frente
    if(keys['w'] || keys['W']){
        carX += sinf(carAngle * 3.14159f/180.0f) * moveStep;
        carZ += cosf(carAngle * 3.14159f/180.0f) * moveStep;
    }
    // Ré
    if(keys['s'] || keys['S']){
        carX -= sinf(carAngle * 3.14159f/180.0f) * moveStep;
        carZ -= cosf(carAngle * 3.14159f/180.0f) * moveStep;
    }
    // Esquerda
    if(keys['a'] || keys['A']){
        carAngle += turnStep;
    }
    // Direita
    if(keys['d'] || keys['D']){
        carAngle -= turnStep;
    }
}

// Controle da câmera com setas e PageUp/PageDown
void specialKeyboardInput(int key, int x, int y){
    switch(key){
        case GLUT_KEY_LEFT:  camAngleH -= camSpeed; break;   // Gira para esquerda
        case GLUT_KEY_RIGHT: camAngleH += camSpeed; break;   // Gira para direita
        case GLUT_KEY_UP:    camAngleV += camSpeed; break;   // Olha para cima
        case GLUT_KEY_DOWN:  camAngleV -= camSpeed; break;   // Olha para baixo
        case GLUT_KEY_PAGE_UP:   camDistance -= zoomSpeed; break;   // Aproxima zoom
        case GLUT_KEY_PAGE_DOWN: camDistance += zoomSpeed; break;   // Afasta zoom
    }

    // Limites da distância da câmera (não chega muito perto ou muito longe)
    if(camDistance < 2.0f)   camDistance = 2.0f;
    if(camDistance > 100.0f) camDistance = 100.0f;
}