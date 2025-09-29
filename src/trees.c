#include <GL/glut.h> 
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include "track.h" // necessário para acessar OvalTrack

#define MAX_TREES 400  // limite máximo de árvores

// Estrutura para armazenar cada árvore
typedef struct { 
    float x,y,z; // posição no mundo
    float s;     // escala da árvore
} Tree;

// Variáveis globais
static Tree g_trees[MAX_TREES]; // array de árvores
static int g_treeCount = 0;     // contador de árvores atuais
static GLUquadric* g_quad = NULL; // usado para tronco cilíndrico

// ------------------------------------------------------
// Gera número aleatório entre [a,b]
// ------------------------------------------------------
static float frand(float a, float b){
    return a + (b - a) * (rand() / (float)RAND_MAX);
}

// ------------------------------------------------------
// Verifica se um ponto (x,z) está sobre a pista oval
// Retorna 1 se dentro da pista, 0 se fora
// ------------------------------------------------------
static int point_in_track(float x, float z, const OvalTrack* track) {
    float cx = track->length;
    float rIn = track->radiusInner;
    float rOut = track->radiusOuter;

    // retas
    if (fabs(x) <= cx && fabs(z) >= rIn && fabs(z) <= rOut)
        return 1;

    // semicírculo esquerdo
    float dx = x + cx;
    float dz = z;
    float dist2 = dx*dx + dz*dz;
    if (dist2 >= rIn*rIn && dist2 <= rOut*rOut) return 1;

    // semicírculo direito
    dx = x - cx;
    dist2 = dx*dx + dz*dz;
    if (dist2 >= rIn*rIn && dist2 <= rOut*rOut) return 1;

    return 0; // fora da pista
}

// ------------------------------------------------------
// Inicializa árvores com posições aleatórias fora da pista
// ------------------------------------------------------
void trees_init(int count, float areaHalf, float avoidBox, unsigned int seed, const OvalTrack* track){
    if(count > MAX_TREES) count = MAX_TREES;
    g_treeCount = 0;
    srand(seed); // inicializa semente aleatória

    int tries = 0;
    while(g_treeCount < count && tries < count*50){
        tries++;
        float x = frand(-areaHalf, areaHalf);
        float z = frand(-areaHalf, areaHalf);

        // evita gerar árvore sobre a pista
        if(point_in_track(x, z, track)) 
        continue;

        float s = frand(0.8f, 1.5f); // escala aleatória
        g_trees[g_treeCount++] = (Tree){x, 0.0f, z, s};
    }
}

// ------------------------------------------------------
// Função Bézier cúbica 1D: retorna valor interpolado
// ------------------------------------------------------
static float bez3f(float p0, float p1, float p2, float p3, float t){
    float u = 1.0f - t, uu = u*u, tt = t*t;
    return u*uu*p0 + 3*uu*t*p1 + 3*u*tt*p2 + tt*t*p3;
}

// ------------------------------------------------------
// Estrutura para vetor 2D (X,Z)
// ------------------------------------------------------
typedef struct { 
    float x, z; 
} V2;

static V2 v2(float x,float z){ 
    V2 a={x,z}; 
    return a; 
}

// Bézier cúbica 2D (plano XZ)
static V2 bez3v2(V2 P0, V2 P1, V2 P2, V2 P3, float t){
    float u = 1.0f - t, uu = u*u, tt = t*t;
    float b0 = u*uu, b1 = 3*uu*t, b2 = 3*u*tt, b3 = tt*t;
    return v2(b0*P0.x + b1*P1.x + b2*P2.x + b3*P3.x,
              b0*P0.z + b1*P1.z + b2*P2.z + b3*P3.z);
}

// ------------------------------------------------------
// Desenha uma árvore individual com tronco curvo via Bézier
// ------------------------------------------------------
static void draw_one_tree(float x, float y, float z, float s){
    // cria quadric para tronco se ainda não criado
    if (!g_quad){
        g_quad = gluNewQuadric();
        gluQuadricNormals(g_quad, GLU_SMOOTH);
    }

    glPushMatrix();
    glTranslatef(x, y, z); // posiciona árvore no mundo

    // Parâmetros do tronco
    float trunkH  = 1.25f * s; // altura tronco
    float rBase   = 0.13f * s; // raio base
    float rTop    = 0.07f * s; // raio topo

    // Parâmetros da copa
    float crownH     = 1.10f * s;   // altura copa
    float crownR0    = 0.60f * s;   // raio topo copa
    float crownRmid  = 0.75f * s;   // raio meio copa

    // variações aleatórias suaves para tronco
    float r1 = fabsf(sinf(x*12.9898f + z*78.233f));
    float r2 = fabsf(sinf(x*93.9898f + z*18.233f));
    float r3 = fabsf(sinf(x*41.3123f + z*11.918f));
    float bend = 0.25f * s; // intensidade curvatura

    // Pontos de controle da Bézier do tronco (XZ)
    float P0x = 0.0f,                 P0z = 0.0f;
    float P1x = (r1-0.5f)*2.0f*bend,  P1z = (r2-0.5f)*1.2f*bend;
    float P2x = (r3-0.5f)*2.0f*bend,  P2z = (r1-0.5f)*1.2f*bend;
    float P3x = (r2-0.5f)*1.0f*bend,  P3z = (r3-0.5f)*1.0f*bend;

    // Macro para Bézier 1D
    #define BEZ3(p0,p1,p2,p3,t) ( \
        (1-(t))*(1-(t))*(1-(t))*(p0) + \
        3*(1-(t))*(1-(t))*(t)*(p1) +  \
        3*(1-(t))*(t)*(t)*(p2) +      \
        (t)*(t)*(t)*(p3) )

    // Tronco segmentado ao longo da curva
    glColor3f(0.45f, 0.28f, 0.10f); // marrom
    int segments = 50; // número de segmentos do tronco

    for(int i=0; i<segments; ++i){
        float t0 = (float)i       / (float)segments;
        float t1 = (float)(i + 1) / (float)segments;

        // pontos do tronco no plano XZ
        float C0x = BEZ3(P0x,P1x,P2x,P3x, t0);
        float C0z = BEZ3(P0z,P1z,P2z,P3z, t0);
        float C1x = BEZ3(P0x,P1x,P2x,P3x, t1);
        float C1z = BEZ3(P0z,P1z,P2z,P3z, t1);

        // alturas correspondentes
        float y0 = t0 * trunkH;
        float y1 = t1 * trunkH;

        // vetor direção 3D do segmento
        float dx = C1x - C0x;
        float dy = y1  - y0;
        float dz = C1z - C0z;
        float segLen = sqrtf(dx*dx + dy*dy + dz*dz);
        if (segLen < 1e-6f) continue;

        float invLen = 1.0f / segLen;
        float ux = dx * invLen, uy = dy * invLen, uz = dz * invLen;

        // calcula rotação para alinhar cilindro ao segmento
        float ax = -uy, ay = ux, az = 0.0f;
        float axisLen = sqrtf(ax*ax + ay*ay + az*az);
        float dot = uz; if (dot>1.0f) dot=1.0f; if (dot<-1.0f) dot=-1.0f;
        float angDeg = acosf(dot) * 57.2957795f;

        // raio interpolado para afunilamento
        float r0 = rBase + (rTop - rBase) * t0;
        float r1r= rBase + (rTop - rBase) * t1;
        float r  = 0.5f * (r0 + r1r);

        glPushMatrix();
            glTranslatef(C0x, y0, C0z);
            if (axisLen > 1e-6f) glRotatef(angDeg, ax/axisLen, ay/axisLen, az/axisLen);
            gluCylinder(g_quad, r, r, segLen, 20, 1);
        glPopMatrix();
    }

    // topo do tronco (âncora da copa)
    float TopX = BEZ3(P0x,P1x,P2x,P3x, 1.0f);
    float TopZ = BEZ3(P0z,P1z,P2z,P3z, 1.0f);

    // ===================== COPA =====================
    glPushMatrix();
        glTranslatef(TopX, trunkH, TopZ);
        glColor3f(0.05f, 0.50f, 0.10f); // verde folhagem

        int rings = 10;   // número de “fatias” verticais
        int sides = 22;   // segmentos do círculo

        for(int j=0; j<rings-1; ++j){
            float tA = (float)j       / (float)(rings-1);
            float tB = (float)(j + 1) / (float)(rings-1);

            float yA = tA * crownH;
            float yB = tB * crownH;

            float rA = BEZ3(0.0f, crownRmid, crownR0, 0.0f, tA);
            float rB = BEZ3(0.0f, crownRmid, crownR0, 0.0f, tB);

            glBegin(GL_TRIANGLE_STRIP);
            for(int k=0; k<=sides; ++k){
                float ang = (float)k / (float)sides * 6.2831853f;
                float cx = cosf(ang), cz = sinf(ang);

                glNormal3f(cx, 0.3f, cz); // normal aproximada
                glVertex3f(cx*rA, yA, cz*rA);
                glVertex3f(cx*rB, yB, cz*rB);
            }
            glEnd();
        }
    glPopMatrix();

    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);

    #undef BEZ3
}

// ------------------------------------------------------
// Desenha todas as árvores
// ------------------------------------------------------
void draw_trees(void){
    for(int i = 0; i < g_treeCount; ++i){
        draw_one_tree(g_trees[i].x, g_trees[i].y, g_trees[i].z, g_trees[i].s);
    }
}
