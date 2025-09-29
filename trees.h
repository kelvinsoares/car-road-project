#ifndef TREES_H
#define TREES_H

// inicializa árvores: quantidade, tamanho da área, área a evitar (pista), semente aleatória
void trees_init(int count, float areaHalf, float avoidBox, unsigned int seed, const OvalTrack* track);

// desenha todas as árvores
void draw_trees(void);

#endif // TREES_H
