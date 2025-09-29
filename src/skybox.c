#include <GL/glut.h>
#include <SOIL/SOIL.h>   // Biblioteca para carregar imagens/texturas
#include <stdio.h>
#include "skybox.h"

// Define GL_CLAMP_TO_EDGE se não estiver definido
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

Skybox skybox; // Estrutura que guarda as texturas e tamanho do skybox

// ------------------------------------------------------
// Carrega as texturas do skybox
// ------------------------------------------------------
void loadSkybox() {
    skybox.size = 50.0f; // Tamanho do cubo do skybox

    const char* folder = "assets/skybox"; // Pasta das imagens
    const char* files[6] = { "front.png", "back.png", "left.png", "right.png", "top.png", "bottom.png" };

    // Loop para carregar cada face do cubo
    for(int i = 0; i < 6; i++) {
        char path[256];
        sprintf(path, "%s/%s", folder, files[i]); // Monta caminho da textura

        // Carrega a textura usando SOIL
        skybox.textures[i] = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                                   SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
        if(!skybox.textures[i]) { // Se falhar, mostra erro e fecha
            printf("Erro ao carregar textura %s: %s\n", path, SOIL_last_result());
            exit(1);
        }

        // Configura parâmetros da textura
        glBindTexture(GL_TEXTURE_2D, skybox.textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
}

// ------------------------------------------------------
// Desenha o skybox na cena
// ------------------------------------------------------
void renderSkybox(float camX, float camY, float camZ) {
    float s = skybox.size; // metade do tamanho do cubo

    // Salva estado atual e desliga iluminação/fog temporariamente
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDepthMask(GL_FALSE); // Não escreve no depth buffer (sempre atrás)
    glPushMatrix();
        glTranslatef(camX, camY, camZ); // Centraliza o cubo na posição da câmera

        glEnable(GL_TEXTURE_2D);

        // ------------------------------------------------------
        // Face Front
        // ------------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, skybox.textures[0]);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(-s,-s, s);
            glTexCoord2f(1,0); glVertex3f( s,-s, s);
            glTexCoord2f(1,1); glVertex3f( s, s, s);
            glTexCoord2f(0,1); glVertex3f(-s, s, s);
        glEnd();

        // ------------------------------------------------------
        // Face Back
        // ------------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, skybox.textures[1]);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f( s,-s,-s);
            glTexCoord2f(1,0); glVertex3f(-s,-s,-s);
            glTexCoord2f(1,1); glVertex3f(-s, s,-s);
            glTexCoord2f(0,1); glVertex3f( s, s,-s);
        glEnd();

        // ------------------------------------------------------
        // Face Left
        // ------------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, skybox.textures[2]);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
            glTexCoord2f(1,0); glVertex3f(-s,-s, s);
            glTexCoord2f(1,1); glVertex3f(-s, s, s);
            glTexCoord2f(0,1); glVertex3f(-s, s,-s);
        glEnd();

        // ------------------------------------------------------
        // Face Right
        // ------------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, skybox.textures[3]);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(s,-s, s);
            glTexCoord2f(1,0); glVertex3f(s,-s,-s);
            glTexCoord2f(1,1); glVertex3f(s, s,-s);
            glTexCoord2f(0,1); glVertex3f(s, s, s);
        glEnd();

        // ------------------------------------------------------
        // Face Top
        // ------------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, skybox.textures[4]);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(-s, s, s);
            glTexCoord2f(1,0); glVertex3f( s, s, s);
            glTexCoord2f(1,1); glVertex3f( s, s,-s);
            glTexCoord2f(0,1); glVertex3f(-s, s,-s);
        glEnd();

        // ------------------------------------------------------
        // Face Bottom
        // ------------------------------------------------------
        glBindTexture(GL_TEXTURE_2D, skybox.textures[5]);
        glBegin(GL_QUADS);
            glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
            glTexCoord2f(1,0); glVertex3f( s,-s,-s);
            glTexCoord2f(1,1); glVertex3f( s,-s, s);
            glTexCoord2f(0,1); glVertex3f(-s,-s, s);
        glEnd();

    glPopMatrix();

    glDepthMask(GL_TRUE); // Volta a escrever no depth buffer
    glPopAttrib();        // Restaura estado anterior
}
