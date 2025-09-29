#ifndef INPUT_H
#define INPUT_H

// Teclas
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);

// Teclas especiais
void specialKeyboardInput(int key, int x, int y);

// Processa movimento do carro a cada frame
void processCarMovement(float deltaTime);

#endif
