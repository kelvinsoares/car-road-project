// car_pista.c — GLUT + OpenGL "fixed pipeline"
// Pista a partir de curva Bézier cúbica, ribbon com triangle strip,
// carro (cubo) seguindo a curva com orientação pela tangente,
// iluminação básica e textura procedural (checkerboard).

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_TREES 400

// ======= Configurações gerais =======
static int winW = 960, winH = 540;
static float aspect = 960.0f/540.0f;

// controle de execução
static float timePrev = 0.0f;
static int g_run = 0;          // 0 = parado, 1 = rodando (carro só anda se 1)

static float car_t = 0.0f;         // parâmetro [0,1] ao longo da Bézier
static float car_speed = 0.08f;    // "t por segundo" (ajuste fino)

// controle de câmera (3ª pessoa simples)
// câmera (modo seguir carro)
static float camYaw = 0.0f;    // ângulo para orbitar em torno do carro (radianos)
static float camDist = 8.0f;   // distância atrás do carro
static float camHeight = 4.0f; // altura da câmera
static int g_followCar = 1;    // 1 = orbitar o carro; 0 = orbitar o cenário


// câmera (modo orbitar cenário)
static float worldYaw   = 0.0f;    // rad
static float worldPitch = 0.2f;    // rad (inclinação)
static float worldRadius = 22.0f;  // distância do alvo


// textura procedural (checker)
static GLuint texChecker = 0;
static int texSize = 256;

// ======= Árvores (config) =======

// textura checker (pista) e árvores
static GLuint texTrack = 0, texBark=0, texLeaf=0;
// opcional: textura procedural para diferenciar tronco/folha

typedef struct { float x,y,z, s; } Tree;
static Tree g_trees[MAX_TREES];
static int   g_treeCount = 0;
static float g_treeAreaHalf = 40.0f;   // árvores dentro de [-40,+40] em X e Z
static float g_avoidBox    = 14.0f;    // evita região central (pista)

static GLUquadric* g_quad = NULL;

// ======= Vetores util =======
typedef struct { float x,y,z; } vec3;

static vec3 v3(float x,float y,float z){ 
    vec3 v={x,y,z}; return v; 
}
static vec3 vadd(vec3 a, vec3 b){ 
    return v3(a.x+b.x, a.y+b.y, a.z+b.z); 
}
static vec3 vsub(vec3 a, vec3 b){ 
    return v3(a.x-b.x, a.y-b.y, a.z-b.z); 
}
static vec3 vscale(vec3 a, float s){ 
    return v3(a.x*s, a.y*s, a.z*s); 
}
static float vdot(vec3 a, vec3 b){ 
    return a.x*b.x + a.y*b.y + a.z*b.z; 
}
static vec3 vcross(vec3 a, vec3 b){
    return v3(a.y*b.z - a.z*b.y,a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
static float vlen(vec3 a){ 
    return sqrtf(vdot(a,a));
}
static vec3 vnorm(vec3 a){
     float L=vlen(a); 
     return (L>1e-8f) ? vscale(a,1.0f/L) : v3(0,0,0); 
}

static vec3 worldTarget = {0,0,0};

// ======= Bézier cúbica (centro da pista) =======
// 4 pontos de controle (ajuste como quiser; quase plano XZ)
static vec3 P0 = {-12, 0, -10};
static vec3 P1 = { -6, 0,   8};
static vec3 P2 = {  6, 0,  -8};
static vec3 P3 = { 12, 0,  10};

static vec3 bezierC(float t){
    float u = 1.0f - t;
    float b0 = u*u*u;
    float b1 = 3*u*u*t;
    float b2 = 3*u*t*t;
    float b3 = t*t*t;
    return v3(b0*P0.x + b1*P1.x + b2*P2.x + b3*P3.x,
              b0*P0.y + b1*P1.y + b2*P2.y + b3*P3.y,
              b0*P0.z + b1*P1.z + b2*P2.z + b3*P3.z);
}

static vec3 bezierT(float t){ // derivada (tangente)
    float u = 1.0f - t;
    vec3 d =
        vadd(
          vadd( vscale(P0, -3*u*u),
                vscale(P1, (3*u*u - 6*u*t)) ),
          vadd( vscale(P2, (6*u*t - 3*t*t)),
                vscale(P3, 3*t*t) )
        );
    return vnorm(d);
}

// ======= Textura checker procedural =======
static void makeChecker(){
    unsigned char* data = (unsigned char*) malloc(texSize*texSize*3);
    if(!data) 
        return;
    for(int y = 0; y < texSize; y++){
        for(int x = 0; x<texSize; x++){
            int c = (((x>>4)&1) ^ ((y>>4)&1)) ? 220 : 80; // quadriculado 16px
            int idx = (y*texSize + x)*3;
            data[idx+0] = (unsigned char)c;
            data[idx+1] = (unsigned char)c;
            data[idx+2] = (unsigned char)c;
        }
    }
    glGenTextures(1, &texChecker);
    glBindTexture(GL_TEXTURE_2D, texChecker);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texSize, texSize, GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);
}

static GLuint make_checker_tex(int size, unsigned char c1, unsigned char c2){
    int sz = size;
    unsigned char* data = (unsigned char*) malloc(sz*sz*3);
    for (int y = 0; y < sz; ++y){
        for (int x = 0; x < sz; ++x){
            int check = (((x>>4)&1) ^ ((y>>4)&1));
            int c = check ? c1 : c2;
            int i = (y*sz + x)*3;
            data[i+0] = (unsigned char)c;
            data[i+1] = (unsigned char)c;
            data[i+2] = (unsigned char)c;
        }
    }
    GLuint tex; glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sz, sz, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(data);
    return tex;
}

// ======= Desenhar pista como ribbon =======
static void drawTrackStrip(){
    const int   SAMPLES = 200;
    const float halfW   = 1.2f; // meia-largura da pista ~ 2.4 total
    vec3 up = v3(0,1,0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texChecker);

    // cor base multiplicativa
    GLfloat mat_diffuse[] = {0.85f, 0.85f, 0.85f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);

    glBegin(GL_TRIANGLE_STRIP);
    for(int i=0;i<SAMPLES;i++){
        float t = (float)i/(float)(SAMPLES-1);
        vec3 p  = bezierC(t);
        vec3 tg = bezierT(t);
        vec3 left = vnorm(vcross(up, tg));
        vec3 n    = vnorm(vcross(tg, left)); // normal aproximada

        vec3 L = vadd(p, vscale(left, +halfW));
        vec3 R = vadd(p, vscale(left, -halfW));

        float v = t; // coord V ao longo
        // left
        glNormal3f(n.x, n.y, n.z);
        glTexCoord2f(0.0f, v*10.0f); // repete textura ao longo
        glVertex3f(L.x, L.y, L.z);
        // right
        glNormal3f(n.x, n.y, n.z);
        glTexCoord2f(1.0f, v*10.0f);
        glVertex3f(R.x, R.y, R.z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

// ======= Carro: cubo escalado =======
static void drawCarBody(){
    // glutSolidCube desenha centrado na origem (1x1x1)
    glutSolidCube(1.0);
}

static void drawCar(vec3 pos, vec3 forward){
    vec3 up = v3(0,1,0);
    vec3 right = vscale(vnorm(vcross(up, forward)), -1.0f); // right = -left
    up = vnorm(vcross(forward, right));

    // Monta rotação com base (colunas) [right up forward]
    // OpenGL espera coluna-maior. Usaremos glMultMatrixf com matriz 4x4.
    GLfloat M[16] = {
        right.x,  right.y,  right.z,  0.0f,
        up.x,     up.y,     up.z,     0.0f,
        forward.x,forward.y,forward.z,0.0f,
        0.0f,     0.0f,     0.0f,     1.0f
    };
    glPushMatrix();
        glTranslatef(pos.x, pos.y + 0.35f, pos.z);
        glMultMatrixf(M);
        glScalef(1.1f, 0.5f, 2.0f); // corpo alongado no eixo "forward"
        GLfloat col[] = {0.8f, 0.1f, 0.12f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, col);
        drawCarBody();
    glPopMatrix();
}

static float frand(float a, float b){
    return a + (b - a) * (rand() / (float)RAND_MAX);
}

static void trees_init(int count, float areaHalf, float avoidBox, unsigned int seed){
    if (count > MAX_TREES) count = MAX_TREES;
    g_treeCount = 0; srand(seed);

    int tries = 0;
    while (g_treeCount < count && tries < count*50){
        tries++;
        float x = frand(-areaHalf, areaHalf);
        float z = frand(-areaHalf, areaHalf);

        // evita retângulo central (onde está a curva/pista)
        if (fabsf(x) < avoidBox && fabsf(z) < avoidBox) continue;

        float s = frand(0.8f, 1.5f); // escala da árvore
        g_trees[g_treeCount++] = (Tree){ x, 0.0f, z, s };
    }
}

// ======= IDLE (animação) =======
static void idle(void){
    glutPostRedisplay();
}

static void draw_one_tree(float x, float y, float z, float s){
    if (!g_quad){
        g_quad = gluNewQuadric();
        gluQuadricNormals(g_quad, GLU_SMOOTH);
    }

    // Habilita texturas
    glEnable(GL_TEXTURE_2D);

    // --- tronco ---
    float trunkH  = 1.2f * s;
    float trunkR1 = 0.12f * s;
    float trunkR2 = 0.10f * s;

    glBindTexture(GL_TEXTURE_2D, texBark); // textura de casca
    glPushMatrix();
      glTranslatef(x, y, z);
      glPushMatrix();
        glTranslatef(0, trunkH*0.5f, 0);
        glRotatef(-90, 1,0,0); // cylinder along +Y
        gluQuadricTexture(g_quad, GL_TRUE);
        gluCylinder(g_quad, trunkR1, trunkR2, trunkH, 12, 1);
      glPopMatrix();

      // --- copas (cones) ---
      glBindTexture(GL_TEXTURE_2D, texLeaf);

      // cone 1
      glPushMatrix();
        glTranslatef(0, trunkH + 0.45f*s, 0);
        glutSolidCone(0.65f*s, 1.0f*s, 12, 2);
      glPopMatrix();
      // cone 2
      glPushMatrix();
        glTranslatef(0, trunkH + 1.05f*s, 0);
        glutSolidCone(0.50f*s, 0.8f*s, 12, 2);
      glPopMatrix();
      // cone 3
      glPushMatrix();
        glTranslatef(0, trunkH + 1.5f*s, 0);
        glutSolidCone(0.35f*s, 0.6f*s, 12, 2);
      glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

static void draw_trees(void){
    for (int i = 0; i < g_treeCount; ++i){
        draw_one_tree(g_trees[i].x, g_trees[i].y, g_trees[i].z, g_trees[i].s);
    }
}

static void drawGround(void){
    glDisable(GL_LIGHTING);
    glColor3f(0.35f,0.55f,0.35f);
    float s=60.0f;
    glBegin(GL_QUADS);
      glVertex3f(-s,-0.001f,-s); glVertex3f(s,-0.001f,-s);
      glVertex3f(s,-0.001f,s);   glVertex3f(-s,-0.001f,s);
    glEnd();
    glEnable(GL_LIGHTING);
}

// ======= Iluminação básica =======
static void initLights(void){
    GLfloat ambient[] = {0.2f, 0.2f, 0.22f, 1.0f};
    GLfloat diffuse[] = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat position[]= {15.0f, 12.0f, 10.0f, 1.0f};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    // Material default
    GLfloat mat_diffuse[]   = {0.9f, 0.9f, 0.9f, 1.0f};
    GLfloat mat_specular[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_shininess[] = {32.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

// ======= Atualização do tempo =======
static float getDeltaTime(){
    float ms = (float)glutGet(GLUT_ELAPSED_TIME);
    float dt = (ms - timePrev) * 0.001f;
    timePrev = ms;
    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.1f) dt = 0.1f; // clamp
    return dt;
}

// ======= DISPLAY =======
static void display(void){

    float dt = getDeltaTime();
    if (g_run) {                         // só avança se estiver "play"
        car_t += car_speed * dt;
        if (car_t > 1.0f) car_t -= 1.0f;
    }

    vec3 carPos = bezierC(car_t);
    vec3 fwd    = bezierT(car_t);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- Projeção & View ---

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (g_followCar) {
        // --- MODO: seguir o carro com órbita horizontal pelo camYaw ---
        // órbita simples ao redor do carro (horizontal), independente da direção do carro
        // (fácil e funciona bem para apresentar)
        vec3 camPos = vadd(carPos, v3(camDist * sinf(camYaw), camHeight, camDist * cosf(camYaw)));

        gluLookAt(camPos.x, camPos.y, camPos.z,
                carPos.x, carPos.y, carPos.z,
                0,1,0);
    } else {
        // --- MODO: orbitar o cenário (mundo) ---
        float cp = cosf(worldPitch), sp = sinf(worldPitch);
        vec3 camPos = v3(worldRadius * cp * sinf(worldYaw),
                        worldRadius * sp + 3.0f,
                        worldRadius * cp * cosf(worldYaw));

        gluLookAt(camPos.x, camPos.y, camPos.z,
                worldTarget.x, worldTarget.y, worldTarget.z,
                0,1,0);
    }


    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera 3ª pessoa simples
    vec3 camPos = vadd( vadd(carPos, vscale(fwd, -camDist)), v3(0, camHeight, 0) );
    gluLookAt(camPos.x, camPos.y, camPos.z,
              carPos.x, carPos.y, carPos.z,
              0,1,0);*/

    // luz (posicionamento relativo à modelview atual)
    GLfloat lightPos[] = {15.0f, 12.0f, 10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // chão + pista + carro
    drawGround();
    draw_trees(); 
    drawTrackStrip();
    drawCar(carPos, fwd);

    glutSwapBuffers();
}

//static GLUquadric* g_quad = NULL;

// ======= RESHAPE =======
static void reshape(int w, int h){
    winW = (w>1? w:1); winH = (h>1? h:1);
    aspect = (float)winW / (float)winH;
    glViewport(0,0, winW, winH);
}

// ======= INPUT =======
static void keyboard(unsigned char key, int x, int y){
    (void)x; (void)y;
    switch (key) {
        case 27:        // ESC
            exit(0); 
            break;              
        case ' ': 
            g_run = !g_run; 
            break;      // ESPAÇO = play/pause

        // alterna modo câmera: seguir carro vs orbitar cenário
        case 'f': case 'F':
            g_followCar = !g_followCar;
            break;

        // CONTROLES NO MODO "SEGUIR O CARRO":
        // orbitar ao redor do carro (girar câmera na horizontal)
        case 'a': 
            camYaw -= 0.10f; 
            break;     // gira esquerda
        case 'd': 
            camYaw += 0.10f;
            break;     // gira direita

        // distância e altura da câmera em relação ao carro
        case 'w': 
            camDist -= 0.5f; 
            if (camDist < 2.0f) 
                camDist = 2.0f; 
            break;
        case 's': 
            camDist += 0.5f; 
            break;
        case 'q': 
            camHeight += 0.2f; 
            break;
        case 'e': 
            camHeight -= 0.2f; 
            if (camHeight < 0.5f)
                camHeight = 0.5f; 
            break;

        // velocidade do carro (se estiver rodando)
        case '+': case '=': car_speed += 0.02f; break;
        case '-': case '_': car_speed -= 0.02f; if (car_speed < 0.0f) car_speed = 0.0f; break;
    }
    glutPostRedisplay();
}

static void specialKeyboard(int key, int x, int y){
    (void)x; (void)y;
    // Estes controles fazem mais sentido no modo de orbitar o cenário.
    // Mas deixamos sempre disponíveis.
    switch (key) {
        case GLUT_KEY_LEFT:  
            worldYaw   -= 0.08f; 
            break;
        case GLUT_KEY_RIGHT: 
            worldYaw   += 0.08f; 
            break;
        case GLUT_KEY_UP:    
            worldPitch += 0.05f; 
            if (worldPitch >  1.2f) 
                worldPitch = 1.2f; 
            break;
        case GLUT_KEY_DOWN:  
            worldPitch -= 0.05f; 
            if (worldPitch < -1.2f) 
                worldPitch = -1.2f; 
            break;
        case GLUT_KEY_PAGE_UP:   
            worldRadius -= 1.0f; 
            if (worldRadius < 5.0f) 
                worldRadius = 5.0f; 
            break;
        case GLUT_KEY_PAGE_DOWN: 
            worldRadius += 1.0f; 
            if (worldRadius > 80.0f) 
                worldRadius = 80.0f; 
            break;
    }
    glutPostRedisplay();
}

// ======= INIT =======
static void initGL(void){
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    // background
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // azul "céu"

    initLights();
    texTrack = make_checker_tex(256, 200, 120); // pista cinza
    texBark  = make_checker_tex(256, 110,  70); // tronco marrom
    texLeaf  = make_checker_tex(256, 100, 160); // folhas verdes

    trees_init(150, 28.0f, 10.0f, 1234);

    makeChecker();
}


// ======= MAIN =======
int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Carro na Pista Curva (GLUT)");

    initGL();

    timePrev = (float)glutGet(GLUT_ELAPSED_TIME);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
