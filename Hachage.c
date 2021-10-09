#include "Hachage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define A 0.61803398874989 //A = (sqrt(5)-1)/2

/* Fonction retournant la cl'e d'un point */
int f(double x, double y){
    return y+(x+y)*(x+y+1)/2 ;
}

/* Fonction de hachage */
int hachage(int cle, int m ){
    return (int)( m * (cle*A - (int)(cle*A)) );
}

Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y){
    if (R == NULL) return NULL;
    
    /* Si le noeud est pr'esent dans la table de hachage, on le retourne ( en utilisant sa cle ) */
    int position = hachage(f(x,y), H->m);
    if (H->T[position] != NULL){
        CellNoeud *cn = H->T[position];
        while (cn) {
            if (cn->nd->x == x && cn->nd->y == y) {
                return cn->nd;
            }
            cn = cn->suiv;
        }
    }

    /* Sinon, on le cr'ee */
    Noeud *nd = NULL;
    CellNoeud *cn = NULL;
    /* Creation de Neoud */
    nd = (Noeud*)malloc(sizeof(Noeud));
    if (nd == NULL) {
        fprintf(stderr, "Probleme d'allocation de Noeud\n");
        return NULL;
    }
    nd->x = x;
    nd->y = y;
    nd->num = ++(R->nbNoeuds);
    nd->voisins = NULL;

    /* Creation de CellNoeud pour Reseau */
    cn = (CellNoeud*)malloc(sizeof(CellNoeud));
    if (cn == NULL) {
        fprintf(stderr, "Probleme d'allocation de CellNoeud\n");
        return NULL;
    }
    cn->nd = nd;
    cn->suiv = R->noeuds;
    R->noeuds = cn;

    /* Creation de CellNoeud dans pour TableHachage */
    cn = (CellNoeud*)malloc(sizeof(CellNoeud));
    if (cn == NULL) {
        fprintf(stderr, "Probleme d'allocation de CellNoeud\n");
        return NULL;
    }
    cn->nd = nd;
    cn->suiv = H->T[position];
    H->T[position]= cn;    
    ++(H->n);

    return nd;
}

Reseau* reconstitueReseauHachage(Chaines *C, int M){
    if (C == NULL) return NULL;

    /* Creatio de Table de Hachage */ 
    TableHachage *H = (TableHachage*)malloc(sizeof(TableHachage));
    if (H == NULL) {
        fprintf(stderr, "Probleme d'allocation de TableHachage\n");
        return NULL;
    }
    H->n = 0;
    H->m = M;
    H->T = (CellNoeud**)malloc(sizeof(CellNoeud*)*M);
    if (H->T == NULL) {
        fprintf(stderr, "Probleme d'allocation de H->T\n");
        free(H);
        return NULL;
    }
    memset(H->T, 0, sizeof(CellNoeud*)*M); //initialiser toutes les cases a' NULL

    /* Creation de Reseau vide */
    Reseau *R = creerReseauVide(C->gamma);

    /* Insertion de CellNoeud et de CellCommodite */
    CellCommodite *commod = NULL;
    Noeud *n = NULL;
    Noeud *prev_n = NULL;
    CellNoeud *voisin = NULL;
    CellPoint *pt = NULL;
    CellChaine *c = C->chaines;

    while (c) {
        commod = (CellCommodite*)malloc(sizeof(CellCommodite));
        if (commod == NULL) {
            fprintf(stderr, "Probleme d'allocation de CellCommodite.\n");
            free(R);
            return NULL;
        }
        commod->suiv = R->commodites;
        R->commodites = commod;

        pt = c->points;
        n = rechercheCreeNoeudHachage(R, H, pt->x, pt->y);
        commod->extrA = n;
        pt = pt->suiv;
        while (pt) {
            // prev_n : le noeud precedent
            prev_n = n;
            // n : le noeud courant
            n = rechercheCreeNoeudHachage(R, H, pt->x, pt->y);

            /* Verification d'existence de liaison */
            int voisin_existe = 0;
            voisin = prev_n->voisins;
            while (voisin) {
                if (voisin->nd->num == n->num) {
                    voisin_existe = 1;
                    break;
                }
                voisin = voisin->suiv;
            }
            if (voisin_existe) { //n est deja present dans la liste des voisins de prev_n
                pt = pt->suiv;
                continue;
            }

            /* Mise a jour des voisins des noeuds adjacents (courant et precedent) */
            /* Creation de voisin pour prev_n */
            voisin = (CellNoeud*)malloc(sizeof(CellNoeud));
            if (voisin == NULL) {
                fprintf(stderr, "Probleme d'allocation de voisin (CellNoeud)\n");
            }
            else {
                voisin->nd = n;    //le nouveau voisin
                voisin->suiv = prev_n->voisins;
                prev_n->voisins = voisin;   //inserer en tete
            }

            /* Creation de voisin pour n */
            voisin = (CellNoeud*)malloc(sizeof(CellNoeud));
            if (voisin == NULL) {
                fprintf(stderr, "Probleme d'allocation de voisin (CellNoeud)\n");
            }
            else {
                voisin->nd = prev_n;   //le nouveau voisin
                voisin->suiv = n->voisins;
                n->voisins = voisin;   //inserer en tete
            }

            pt = pt->suiv;
        }
        commod->extrB = n;

        c = c->suiv;
    }

    libererTableHachage(H);
    return R;
}


void libererTableHachage(TableHachage *H) {
    /* 
     * Liberer la memoire occupee par la table de hachage H
     */
    if (H == NULL) {
        return;
    }

    CellNoeud *cn = NULL;
    CellNoeud *tmp = NULL;

    /* Parcours de toutes les cases */
    for (int i=0; i<H->m; ++i) {
        cn = H->T[i];
        while (cn) {
            tmp = cn->suiv;
            /* Liberer uniquement les CellNoeud mais PAS les Noeud */
            free(cn);
            cn = tmp;
        }
    }

    free(H->T);
    free(H);
}
