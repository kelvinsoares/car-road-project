# Projeto OpenGL — Carro em ambiente 3D

Este projeto em C++ usa OpenGL (GLUT) e stb_image para criar uma cena 3D interativa que simula um carro percorrendo uma pista com curvas, com câmera em terceira pessoa e cenário (árvores, pista, grama, chão, iluminação).

---

## Sumário
- [Pré-requisitos](#pré-requisitos)
- [Estrutura do projeto](#estrutura-do-projeto)
- [Como compilar (Ubuntu/WSL)](#como-compilar-ubuntuwsl)
- [Como executar](#como-executar)
- [Funcionalidades e Controles](#funcionalidades-e-controles)
- [Dicas e Solução de Problemas](#dicas-e-solução-de-problemas)
  
---

## Pré-requisitos

### Ubuntu / WSL (Windows 11)
  Instale compilador e bibliotecas OpenGL:

  - OpenGL
    
  - GLUT
    
  - stb_image
    
  - Compilador C++ compatível

  ```bash
  sudo apt update
  sudo apt install -y build-essential freeglut3-dev mesa-common-dev libglu1-mesa-dev
  ```

## Estrutura do projeto
 ```bash
  car_project/
  ├── include/        # headers (.h)
  └── src/            # código-fonte (.c)
      └── main.c
 ```

## Como compilar (Ubuntu/WSL)
  
  Na raiz do projeto (onde estão src/ e include/):
  ```bash
  gcc src/*.c -Iinclude -o ambient -lGL -lGLU -lglut -lm
  ```
  ## Como executar
  ```bash
  ./ambient
  ```

## Funcionalidades e Controles
  
  - Espaço: play/pause do carro
  
  - F: alterna modo de câmera (seguir carro ↔ orbitar cenário)
  
  - A / D: orbitar câmera em torno do carro
  
  - W / S: aproximar/afastar a câmera
  
  - Q / E: aumentar/diminuir altura da câmera
  
  - Setas: orbitar a cena (modo cenário)
  
  - PgUp / PgDn: aproximação/afastamento (modo cenário)
  
  -  -/+: aumenta/diminui velocidade do carro
    
  - Esc: sair

## Dicas e Solução de Problemas

  **Caso veja o erro “undefined reference to …” ao linkar:**
  
  → Provavelmente foi compilado apenas main.c. Use src/*.c para linkar todos os .c:
  ```bash
  gcc src/*.c -Iinclude -o ambient -lGL -lGLU -lglut -lm
  ```

  **Se acontecer fatal error: GL/glut.h: No such file or directory**
  
  → Instale os dev packages:
  ```bash
  sudo apt update
  sudo apt install -y freeglut3-dev mesa-common-dev libglu1-mesa-dev
  ```
