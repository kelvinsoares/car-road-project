#include <GL/glut.h>  
#include <SOIL/SOIL.h> // Para carregar texturas
#include <stdio.h>     
#include "ground.h"  

// ID da textura do chão (grass)
GLuint texGrassDiffuse;

// ------------------------------------------------------
// Função para carregar a textura do chão
// ------------------------------------------------------
void loadGroundTexture() {
    // Carrega a textura "grassy_d.png" usando SOIL
    // SOIL_LOAD_AUTO → detecta automaticamente o formato
    // SOIL_CREATE_NEW_ID → cria um novo ID de textura
    // SOIL_FLAG_MIPMAPS → gera mipmaps (texturas menores para distâncias)
    // SOIL_FLAG_INVERT_Y → inverte verticalmente a imagem (OpenGL e SOIL têm eixo Y diferente)
    texGrassDiffuse = SOIL_load_OGL_texture(
        "assets/grassy_d.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    // Verifica se a textura foi carregada corretamente
    if (!texGrassDiffuse) {
        printf("Erro ao carregar textura do chão: %s\n", SOIL_last_result());
        exit(1); // encerra o programa em caso de falha
    }

    // Configura parâmetros da textura
    glBindTexture(GL_TEXTURE_2D, texGrassDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repetir horizontalmente
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repetir verticalmente
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // suaviza ao reduzir
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // suaviza ao aumentar
    glBindTexture(GL_TEXTURE_2D, 0); // desliga a textura
}

// ------------------------------------------------------
// Desenha o chão como um grid dividido em quadrados
// ------------------------------------------------------
void drawGroundGrid(float size, int divisions) {
    float halfSize = size / 2.0f; // metade do tamanho total do chão
    float step = size / divisions; // tamanho de cada quadrado
    float texRepeat = 40.0f; // quantas vezes a textura se repete no grid

    // Ativa a textura do chão
    glBindTexture(GL_TEXTURE_2D, texGrassDiffuse);

    // Loop para desenhar cada quadrado
    for(int i = 0; i < divisions; i++) {
        for(int j = 0; j < divisions; j++) {
            // Coordenadas X e Z do quadrado
            float x0 = -halfSize + i * step;
            float x1 = x0 + step;
            float z0 = -halfSize + j * step;
            float z1 = z0 + step;

            // Coordenadas de textura correspondentes ao quadrado
            float u0 = (float)i / divisions * texRepeat;
            float u1 = (float)(i+1) / divisions * texRepeat;
            float v0 = (float)j / divisions * texRepeat;
            float v1 = (float)(j+1) / divisions * texRepeat;

            // Normal para iluminação (aponta para cima)
            glNormal3f(0.0f, 1.0f, 0.0f);

            // Desenha o quadrado com textura
            glBegin(GL_QUADS);
                glTexCoord2f(u0, v0); glVertex3f(x0, 0.0f, z0);
                glTexCoord2f(u1, v0); glVertex3f(x1, 0.0f, z0);
                glTexCoord2f(u1, v1); glVertex3f(x1, 0.0f, z1);
                glTexCoord2f(u0, v1); glVertex3f(x0, 0.0f, z1);
            glEnd();
        }
    }

    // Desativa a textura
    glBindTexture(GL_TEXTURE_2D, 0);
}
