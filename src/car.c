#include "car.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>  // Para carregar texturas
#include <stdio.h>

// Posição e ângulo do carro na pista
float carX = 0.0f;
float carY = 0.0f;
float carZ = 0.0f;
float carAngle = 0.0f;

// Velocidade do carro e giro (graus por segundo)
float carSpeed = 7.0f;
float carTurnSpeed = 90.0f;

// Texturas do carro, rodas e tampas
static GLuint TEX_CAR = 0, TEX_TIRE = 0, TEX_RIM = 0;

// Objeto GLU para cilindros e discos (rodas)
static GLUquadric* g_quad = NULL;

// ------------------------------------------------------
// Função para carregar textura do SOIL
// ------------------------------------------------------
static GLuint load_tex(const char* path){
    GLuint id = SOIL_load_OGL_texture(
        path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if(!id){
        printf("Falha ao carregar textura %s: %s\n", path, SOIL_last_result());
        // Continua mesmo sem textura
    } else {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    return id;
}

// ------------------------------------------------------
// Inicializa o carro
// ------------------------------------------------------
void initCar(Car* car, float x, float y, float z) {
    car->x = x;
    car->y = y;
    car->z = z;

    // Tamanho do carro (largura, altura, comprimento)
    car->w = 2.0f;
    car->h = 1.0f;
    car->l = 4.0f;
    car->rotY = 0.0f;

    // Carrega texturas se ainda não carregadas
    if (!TEX_CAR)  TEX_CAR  = load_tex("assets/car_diffuse.png");
    if (!TEX_TIRE) TEX_TIRE = load_tex("assets/tire.png");
    if (!TEX_RIM)  TEX_RIM  = load_tex("assets/rim.png");

    // Cria quadric GLU para rodas
    if (!g_quad){
        g_quad = gluNewQuadric();
        gluQuadricNormals(g_quad, GLU_SMOOTH);
        gluQuadricTexture(g_quad, GL_TRUE); // permite aplicar textura
    }
}

// ------------------------------------------------------
// Desenha uma caixa texturizada centrada em (0,0,0)
// ------------------------------------------------------
static void drawTexturedBox(float w, float h, float l){
    float x = w*0.5f, y = h*0.5f, z = l*0.5f;

    glBegin(GL_QUADS);

    // +Z (frente)
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-x,-y, z);
    glTexCoord2f(1,0); glVertex3f( x,-y, z);
    glTexCoord2f(1,1); glVertex3f( x, y, z);
    glTexCoord2f(0,1); glVertex3f(-x, y, z);

    // -Z (traseira)
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f( x,-y,-z);
    glTexCoord2f(1,0); glVertex3f(-x,-y,-z);
    glTexCoord2f(1,1); glVertex3f(-x, y,-z);
    glTexCoord2f(0,1); glVertex3f( x, y,-z);

    // +X (direita)
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f( x,-y, z);
    glTexCoord2f(1,0); glVertex3f( x,-y,-z);
    glTexCoord2f(1,1); glVertex3f( x, y,-z);
    glTexCoord2f(0,1); glVertex3f( x, y, z);

    // -X (esquerda)
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
    glTexCoord2f(1,0); glVertex3f(-x,-y, z);
    glTexCoord2f(1,1); glVertex3f(-x, y, z);
    glTexCoord2f(0,1); glVertex3f(-x, y,-z);

    // +Y (teto)
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-x, y, z);
    glTexCoord2f(1,0); glVertex3f( x, y, z);
    glTexCoord2f(1,1); glVertex3f( x, y,-z);
    glTexCoord2f(0,1); glVertex3f(-x, y,-z);

    // -Y (assoalho)
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-x,-y,-z);
    glTexCoord2f(1,0); glVertex3f( x,-y,-z);
    glTexCoord2f(1,1); glVertex3f( x,-y, z);
    glTexCoord2f(0,1); glVertex3f(-x,-y, z);

    glEnd();
}

// ------------------------------------------------------
// Desenha o carro completo
// ------------------------------------------------------
void drawCar(const Car* car) {
    glPushMatrix();
        // Move e gira o carro na cena
        glTranslatef(car->x, car->y + car->h, car->z);
        glRotatef(car->rotY, 0,1,0);

        // Corpo do carro: material brilhante + textura
        GLfloat spec[4] = {0.6f, 0.6f, 0.6f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);

        glBindTexture(GL_TEXTURE_2D, TEX_CAR);  // Aplica textura
        glColor3f(1,1,1); // Branco = não altera a cor da textura
        drawTexturedBox(car->w, car->h, car->l);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Parabrisa/janelas simples (quads azulados)
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.3f, 0.45f, 0.6f);
        glBegin(GL_QUADS);
            // Frontal
            float x = car->w*0.45f, y = car->h*0.45f, z = car->l*0.5f + 0.001f;
            glNormal3f(0,0,1);
            glVertex3f(-x, 0.0f, z);
            glVertex3f( x, 0.0f, z);
            glVertex3f( x, y,   z);
            glVertex3f(-x, y,   z);

            // Traseiro
            z = -car->l*0.5f - 0.001f;
            glNormal3f(0,0,-1);
            glVertex3f( x, 0.0f, z);
            glVertex3f(-x, 0.0f, z);
            glVertex3f(-x, y,   z);
            glVertex3f( x, y,   z);
        glEnd();
        glEnable(GL_TEXTURE_2D);

        // Rodas (cilindros com textura + tampas)
        float wheelRadius = 0.4f;
        float wheelWidth  = 0.22f;

        GLfloat noSpec[4] = {0.0f,0.0f,0.0f,1.0f}; // Rodas foscas
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpec);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 4.0f);

        // Posições das rodas
        float wX[] = {-car->w/2 - wheelWidth/2,  car->w/2 + wheelWidth/2,
                      -car->w/2 - wheelWidth/2,  car->w/2 + wheelWidth/2};
        float wZ[] = { car->l/2 - wheelRadius,   car->l/2 - wheelRadius,
                      -car->l/2 + wheelRadius,  -car->l/2 + wheelRadius};

        // Loop para desenhar 4 rodas
        for(int i=0;i<4;i++){
            glPushMatrix();
                glTranslatef(wX[i]-0.1f, -car->h/2, wZ[i]);
                glRotatef(90,0,1,0); // gira eixo para cilindro

                // Pneu
                glBindTexture(GL_TEXTURE_2D, TEX_TIRE);
                glColor3f(0,0,0);
                gluCylinder(g_quad, wheelRadius, wheelRadius, wheelWidth, 18, 2);

                // Tampas (rims)
                glBindTexture(GL_TEXTURE_2D, TEX_RIM);
                // Externa
                glPushMatrix();
                    glTranslatef(0, 0, 0);
                    glRotatef(180, 0,1,0);
                    gluDisk(g_quad, 0.0, wheelRadius, 18, 1);
                glPopMatrix();

                // Interna
                glPushMatrix();
                    glTranslatef(0, 0, wheelWidth);
                    gluDisk(g_quad, 0.0, wheelRadius, 18, 1);
                glPopMatrix();

                glBindTexture(GL_TEXTURE_2D, 0);
            glPopMatrix();
        }

    glPopMatrix();
}
