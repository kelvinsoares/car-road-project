# Projeto OpenGL — Carro em ambiente 3D


Este projeto em C é uma simulação 3D desenvolvida em OpenGL/GLUT, representando um carro em movimento em uma pista oval.
O ambiente inclui céu com skybox, árvores distribuídas aleatoriamente, pista texturizada, grama, e iluminação básica com materiais para dar maior realismo; com câmera em terceira pessoa.

## Funcionalidades

  - Carro 3D com rodas modeladas

  - Movimentação do carro (aceleração, rotação)

  - Pista oval texturizada com gramado ao redor

  - Skybox com texturas (céu, nuvens)

  - Árvores geradas proceduralmente

  - Iluminação e materiais (efeitos de brilho especular e sombras)

  - Câmera interativa (seguindo o carro, visão em terceira pessoa)

## Controles:
  
  - Setas do teclado: mover e girar o carro
  - Esc: sair

## Pré-requisitos

### Ubuntu / WSL (Windows 11)
  Instale compilador C++ compatível e bibliotecas:

  - OpenGL
    
  - GLUT
    
  - GLU

  - SOIL

  ```bash
  sudo apt update
  sudo apt install -y build-essential freeglut3-dev mesa-common-dev libglu1-mesa-dev
  ```

## Estrutura do projeto
 ```bash
 src/
 ├── car.c / car.h         # Carro e rodas
 ├── track.c / track.h     # Pista oval
 ├── ground.c              # Grama / terreno
 ├── trees.c / trees.h     # Árvores
 ├── skybox.c / skybox.h   # Céu e nuvens
 ├── scene.c / scene.h     # Cena principal (câmera, iluminaçao, display)
 └── main.c                # Ponto de entrada
assets/
 ├── track.png             # Textura da pista
 ├── grassy.png            # Textura da grama
 ├── car_diffuse.png       # Textura do carro
 ├── tire.png              # Textura da lateral do pneu
 ├── rim.png               # Textura da roda (aro)
 └── skybox/               # 6 imagens do céu (front, back, left, right, top, bottom)
 ```

## Como compilar (Ubuntu/WSL)

Após instalar dependências:
```bash
  sudo apt-get install freeglut3-dev libsoil-dev
  ```
  ## Na raiz do projeto (onde está src/), compilar:
  ```bash
  g++ *.c -o build/opengl-app -lGL -lGLU -lglut -lm -lSOIL
  ```
  ## Para executar:
  ```bash
  build/opengl-app
  ```
