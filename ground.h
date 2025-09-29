#ifndef GROUND_H
#define GROUND_H

#include <GL/glut.h>

extern GLuint texGrassDiffuse;

void loadGroundTexture();
void drawGroundGrid(float size, int divisions);

#endif
