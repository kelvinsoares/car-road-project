#include "track.h"       // Header com declarações da pista oval
#include <SOIL/SOIL.h>   // Para carregar texturas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265f   // Definindo PI para cálculos trigonométricos

// ------------------------------------------------------
// Carrega textura da pista a partir de um arquivo
// ------------------------------------------------------
GLuint loadTrackTexture(const char* filename) {
    GLuint texID = SOIL_load_OGL_texture(
        filename,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    // Verifica se a textura carregou corretamente
    if (!texID) {
        printf("Erro ao carregar textura da pista %s: %s\n", filename, SOIL_last_result());
        exit(1);
    }

    // Configura parâmetros da textura
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repetir horizontalmente
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repetir verticalmente
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // suaviza ao reduzir
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               // suaviza ao aumentar
    glBindTexture(GL_TEXTURE_2D, 0); // desativa textura

    return texID;
}

// Inicializa dados da pista oval
// inner: raio interno
// outer: raio externo
// height: altura do plano
// segments: subdivisões para curvas
// length: comprimento das retas

void initTrack(OvalTrack* track, float inner, float outer, float height, int segments, float length) {
    track->radiusInner = inner * 2.5f;  // escala o raio interno
    track->radiusOuter = outer * 2.5f;  // escala o raio externo
    track->height = height;              // altura do plano
    track->segments = segments;          // subdivisões para curvas (quanto maior, mais suave)
    track->length = length;              // comprimento das retas
    track->textureID = 0;                // sem textura inicial
}

// Desenha a pista oval (retas + curvas semicirculares)
void drawOvalTrack(const OvalTrack* track) {
    float rIn = track->radiusInner;   // raio interno
    float rOut = track->radiusOuter;  // raio externo
    float y = track->height;          // altura da pista
    int segs = track->segments;       // subdivisões para curvas
    float cx = track->length;         // comprimento das retas
    float angleStep = PI / segs;      // incremento de ângulo para semicirculo

    // ativa textura da pista, se houver
    if (track->textureID) {
        glBindTexture(GL_TEXTURE_2D, track->textureID);
    }

    //Semicírculo esquerdo
    glNormal3f(0.0f, 1.0f, 0.0f); // normal apontando para cima
    glBegin(GL_QUAD_STRIP);       // faixa de quadrados para curva
    for (int i = 0; i <= segs; i++) {
        float angle = PI/2 + i * angleStep;     // ângulo no semicirculo
        float xOut = -cx + cos(angle) * rOut;  // coordenada X externa
        float zOut = sin(angle) * rOut;        // coordenada Z externa
        float xIn  = -cx + cos(angle) * rIn;   // coordenada X interna
        float zIn  = sin(angle) * rIn;         // coordenada Z interna

        // aplica coordenadas de textura e vertices
        glTexCoord2f((float)i/segs, 1.0f); glVertex3f(xOut, y, zOut);
        glTexCoord2f((float)i/segs, 0.0f); glVertex3f(xIn,  y, zIn);
    }
    glEnd();

    // --- Semicírculo direito ---
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++) {
        float angle = -PI/2 + i * angleStep;    // ângulo semicirculo direito
        float xOut = cx + cos(angle) * rOut;
        float zOut = sin(angle) * rOut;
        float xIn  = cx + cos(angle) * rIn;
        float zIn  = sin(angle) * rIn;

        glTexCoord2f((float)i/segs, 1.0f); glVertex3f(xOut, y, zOut);
        glTexCoord2f((float)i/segs, 0.0f); glVertex3f(xIn,  y, zIn);
    }
    glEnd();

    // --- Reta superior ---
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cx, y,  rOut);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( cx, y,  rOut);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( cx, y,  rIn);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cx, y,  rIn);
    glEnd();

    // --- Reta inferior ---
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-cx, y, -rOut);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( cx, y, -rOut);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( cx, y, -rIn);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-cx, y, -rIn);
    glEnd();

    // desativa textura
    if (track->textureID) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
