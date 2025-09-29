#include <GL/glut.h>
#include <SOIL/SOIL.h>   // Para carregar texturas
#include <math.h>
#include "scene.h"
#include "skybox.h"
#include "ground.h"
#include "track.h"
#include "car.h"
#include "trees.h"
#include "input.h"

// Objetos da cena
OvalTrack nascarTrack;  // Pista oval
Car myCar;              // Carro

static float timePrev;  // Guarda o tempo anterior (para calcular deltaTime)

// ------------------------------------------------------
// Câmera
// ------------------------------------------------------
float camDistance = 20.0f;  // Distância da câmera para o carro
float camAngleH = 0.0f;     // Ângulo horizontal da câmera
float camAngleV = 20.0f;    // Ângulo vertical da câmera
float camSpeed = 2.0f;      // Velocidade de rotação da câmera
float zoomSpeed = 1.0f;     // Velocidade do zoom

// Variáveis externas do carro (definidas em input.c)
extern float carX;
extern float carY;
extern float carZ;
extern float carAngle;
extern float carSpeed;
extern float carTurnSpeed;

// ------------------------------------------------------
// Configura iluminação e efeitos da cena
// ------------------------------------------------------
static void setupLighting(void){
    // Luz direcional (sol)
    GLfloat lightDirPos[4] = { -0.4f, 1.0f, 0.3f, 0.0f }; // w=0 → direcional
    GLfloat ambient[4]    = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat diffuse[4]    = { 0.95f, 0.95f, 0.95f, 1.0f };
    GLfloat specular[4]   = { 0.4f, 0.4f, 0.4f, 1.0f };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightDirPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Combina luz com textura (importante para chão e pista)
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Usa cores atuais como material difuso/especular
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Especular (brilho)
    GLfloat matSpec[4] = {0.3f, 0.3f, 0.3f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

    // Fog (atmosfera/profundidade)
    glEnable(GL_FOG);
    GLfloat fogColor[4] = {0.53f, 0.81f, 0.98f, 1.0f}; // mesma cor do céu
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.012f); 
}

// ------------------------------------------------------
// Inicializa a cena
// ------------------------------------------------------
void initScene() {
    loadGroundTexture();  // Carrega textura do chão
    loadSkybox();         // Carrega céu

    // Inicializa pista oval
    initTrack(&nascarTrack, 8.0f, 10.0f, 0.02f, 40, 15.0f);
    nascarTrack.textureID = loadTrackTexture("assets/track.png");

    // Inicializa carro
    initCar(&myCar, 0.0f, 0.0f, 0.0f);
    carX = 0.0f;
    carY = 0.0f;
    carZ = -( (nascarTrack.radiusInner + nascarTrack.radiusOuter) / 2.0f ); // centraliza na pista
    carAngle = 90.0f;

    // Inicializa árvores
    trees_init(150, 28.0f, 10.0f, 1234, &nascarTrack);

    // Configura luz e fog
    setupLighting();
}


// Desenha a cena
void displayScene(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // limpa tela e profundidade
    glLoadIdentity();

    // Calcula posição da câmera em terceira pessoa
    float camX = carX + camDistance * sinf(camAngleH * 3.14159f/180.0f) * cosf(camAngleV * 3.14159f/180.0f);
    float camY = carY + camDistance * sinf(camAngleV * 3.14159f/180.0f);
    float camZ = carZ + camDistance * cosf(camAngleH * 3.14159f/180.0f) * cosf(camAngleV * 3.14159f/180.0f);

    // Define para onde a câmera olha
    gluLookAt(camX, camY, camZ,  // posição da câmera
              carX, carY + 0.5f, carZ,  // olhar para o carro
              0.0f, 1.0f, 0.0);         // eixo "para cima"

    renderSkybox(camX, camY, camZ); // Desenha céu
    drawGroundGrid(200.0f, 50);     // Desenha chão
    drawOvalTrack(&nascarTrack);    // Desenha pista

    // Atualiza posição e rotação do carro
    myCar.x = carX;
    myCar.z = carZ;
    myCar.rotY = carAngle;
    drawCar(&myCar);                // Desenha carro

    draw_trees();                   // Desenha árvores
    glutSwapBuffers();              // Troca buffers (double buffering)
}

// ------------------------------------------------------
// Inicializa timer para movimento
// ------------------------------------------------------
void initTimer(){
    timePrev = (float)glutGet(GLUT_ELAPSED_TIME); // pega tempo atual
}

// ------------------------------------------------------
// Função chamada continuamente (idle)
// ------------------------------------------------------
void idleTimer(){
    float timeNow = (float)glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (timeNow - timePrev) / 1000.0f; // calcula tempo desde último frame
    timePrev = timeNow;

    processCarMovement(deltaTime); // atualiza posição do carro

    glutPostRedisplay();           // pede redesenho da cena
}
