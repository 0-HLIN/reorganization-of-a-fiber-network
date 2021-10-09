#ifndef HACHAGE_H
#define HACHAGE_H
#include "Reseau.h"

typedef struct tableH{
    int n;     /* Nombre d'éléments contenus dans la table de hachage */
    int m;     /* Taille de la table de hachage */
    CellNoeud **T; /* Table de hachage avec resolution des collisions par chainage */
} TableHachage;

int f(double x, double y);
int hachage(int cle, int m );

Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y);
Reseau* reconstitueReseauHachage(Chaines *C, int M);

void libererTableHachage(TableHachage *H);

#endif
