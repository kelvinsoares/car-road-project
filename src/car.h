#ifndef CAR_H
#define CAR_H

#include <GL/glut.h>

// estrutura para carro
typedef struct {
    float x, y, z;      // posição
    float w, h, l;      // largura, altura, comprimento
    float rotY;         // rotação no eixo Y
} Car;

// funções
void initCar(Car* car, float x, float y, float z);
void drawCar(const Car* car);

#endif
