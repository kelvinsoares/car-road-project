#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glut.h>

typedef struct {
    GLuint textures[6];
    float size;
} Skybox;

extern Skybox skybox;

void loadSkybox();
void renderSkybox(float camX, float camY, float camZ);

#endif
