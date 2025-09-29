#include <GL/glut.h>   
#include "scene.h"     
#include "input.h"     
#include "window.h"    

// Configuração inicial do OpenGL
void initGL(){
    glEnable(GL_DEPTH_TEST);   // Ativa profundidade (3D)
    glShadeModel(GL_SMOOTH);   // Ativa sombreamento suave
    glEnable(GL_NORMALIZE);    // Corrige iluminação
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Fundo azul (céu)
    glEnable(GL_TEXTURE_2D);   // Ativa texturas
}

int main(int argc, char** argv){
    glutInit(&argc, argv); // Inicia GLUT

    // Define como a janela será exibida
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Define tamanho inicial da janela
    glutInitWindowSize(getWindowWidth(), getWindowHeight());

    // Cria a janela
    glutCreateWindow("Carro em Pista Oval - OpenGL");

    // Inicializações
    initGL();      // Configurações do OpenGL
    initScene();   // Monta a cena (pista, carro, etc.)
    initTimer();   // Inicia o tempo

    // Callbacks (funções chamadas automaticamente pelo GLUT)
    glutDisplayFunc(displayScene);      // Desenho da cena
    glutReshapeFunc(reshapeWindow);     // Redimensionar janela
    glutKeyboardFunc(keyboardDown);     // Tecla pressionada
    glutKeyboardUpFunc(keyboardUp);     // Tecla solta
    glutSpecialFunc(specialKeyboardInput); // Teclas especiais (setas, PageUp/Down)
    glutIdleFunc(idleTimer);            // Atualização contínua

    glutMainLoop(); // Loop principal (fica rodando o programa)

    return 0;
}
