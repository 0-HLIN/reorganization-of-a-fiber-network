#include <stdio.h>
#include <stdlib.h>
#include "Reseau.h"
#include "ArbreQuat.h"

void chaineCoordMinMax(Chaines *C, double *xmin, double *ymin, double *xmax, double *ymax) {
    /**
     * Recherche iterativement les valeurs maximales et minimales des coordonnees (x et y) 
     *  des noeuds de la Chaines C, et les placer dans les varaibles passees en arguments.
     */
    if (C == NULL) {
        return;
    }

    CellChaine *ch = C->chaines;
    CellPoint *pt = ch->points;
    *xmin = pt->x;
    *xmax = pt->x;
    *ymin = pt->y;
    *ymax = pt->y;
    while (ch) {
        pt = ch->points;
        while (pt) {
            if (pt->x < *xmin) {
                *xmin = pt->x;
            }
            else if (pt->x > *xmax) {
                *xmax = pt->x;
            }
            if (pt->y < *ymin) {
                *ymin = pt->y;
            }
            else if (pt->y > *ymax) {
                *ymax = pt->y;
            }
            pt = pt->suiv;
        }
        ch = ch->suiv;
    }
}


ArbreQuat *creerArbreQuat(double xc, double yc, double coteX, double coteY) {
    ArbreQuat *aq = (ArbreQuat*)malloc(sizeof(ArbreQuat));
    if (aq == NULL) {
        fprintf(stderr, "Probleme d'allocation de ArbreQuat.\n");
        return NULL;
    }

    aq->xc = xc;
    aq->yc = yc;
    aq->coteX = coteX;
    aq->coteY = coteY;

    aq->noeud = NULL;
    aq->so = NULL;
    aq->se = NULL;
    aq->no = NULL;
    aq->ne = NULL;

    return aq;
}


void insererNoeudArbre(Noeud *n, ArbreQuat **a, ArbreQuat *parent) {
    /**
     * Inserer le noeud n dans l'arbre (*a)
     * Param:
     *  - n      : le noeud a inserer
     *  - a      : l'adresse du pointeur sur l'abre
     *  - parent : le parent de l'abre represente par 'a'
     */

    if (n == NULL || a == NULL) {
        return;
    }

    /* Cas arbre NULL: placer n dans le noeud courant */
    if (*a == NULL) {
        //Determiner les coordonnees du nouvel arbre
        double xc = -1.0, yc = -1.0;
        if (n->x < parent->xc) {
            xc = parent->xc - parent->coteX / 4.0;
        }
        else {
            xc = parent->xc + parent->coteX / 4.0;
        }
        if (n->y < parent->yc) {
            yc = parent->yc - parent->coteY / 4.0;
        }
        else {
            yc = parent->yc + parent->coteY / 4.0;
        }
        //Creer le nouvel arbre puis placer n dedans
        *a = creerArbreQuat(xc, yc, parent->coteX / 2.0, parent->coteY / 2.0);
        (*a)->noeud = n;
    }

    /* Cas d'une feuille: inserer n, puis (*a)->noued dans un de ces fils */
    else if ((*a)->noeud != NULL) {
        // inserer n
        if (n->x < (*a)->xc) {     //dans no / so
            if (n->y < (*a)->yc) { //dans so
                insererNoeudArbre(n, &((*a)->so), *a);
            }
            else {                 // dans no
                insererNoeudArbre(n, &((*a)->no), *a);
            }
        }
        else {                     //dans ne / se
            if (n->y < (*a)->yc) { //dans se
                insererNoeudArbre(n, &((*a)->se), *a);
            }
            else {                 //ne
                insererNoeudArbre(n, &((*a)->ne), *a);
            }
        }
        // inserer (*a)->noeud
        if ((*a)->noeud->x < (*a)->xc) {     //dans no / so
            if ((*a)->noeud->y < (*a)->yc) { //dans so
                insererNoeudArbre((*a)->noeud, &((*a)->so), *a);
            }
            else {                           // dans no
                insererNoeudArbre((*a)->noeud, &((*a)->no), *a);
            }
        }
        else {                               //dans ne / se
            if ((*a)->noeud->y < (*a)->yc) { //dans se
                insererNoeudArbre((*a)->noeud, &((*a)->se), *a);
            }
            else {                 //dans ne
                insererNoeudArbre((*a)->noeud, &((*a)->ne), *a);
            }
        }
        (*a)->noeud = NULL;
    }

    /* Cas noued interne: rappeler recursivement */
    else {
        if (n->x < (*a)->xc) {     //dans no / so
            if (n->y < (*a)->yc) { //dans so
                insererNoeudArbre(n, &((*a)->so), *a);
            }
            else {                 // dans no
                insererNoeudArbre(n, &((*a)->no), *a);
            }
        }
        else {                     //dans ne / se
            if (n->y < (*a)->yc) { //dans se
                insererNoeudArbre(n, &((*a)->se), *a);
            }
            else {                 //ne
                insererNoeudArbre(n, &((*a)->ne), *a);
            }
        }
    }
}


Noeud *rechercheCreeNoeudArbre(Reseau *R, ArbreQuat **a, ArbreQuat *parent, double x, double y){
    /**
     * Recherche le noeud des coordonnes (x,y) dans l'abre a si il existe, 
     *  sinon le creer, l'inserer (dans R et dans *a) puis le retourner.
     * Param:
     *  - R      : le Reseau que represente l'arbre (*a)  (lors du premier appel);
     *  - a      : l'adresse du pointeur sur l'abre, dans lequel on effectue la recherche;
     *  - parent : parent de l'arbre represente par 'a' .
     * Return:
     *  Noeud de coordonnees (x,y) dans R et *a .
     */
    if (R == NULL || a == NULL) {
        return NULL;
    }

    /* Cas arbre NULL: inserer directement */
    if (*a == NULL) {
        Noeud *n = insereNoeud(R, x, y);
        insererNoeudArbre(n, a, parent);
        return n;
    }

    /* Cas d'une feuille */
    else if ((*a)->noeud != NULL) {
        if ((*a)->noeud->x == x && (*a)->noeud->y == y) { //Noeud trouve'
            return (*a)->noeud;
        }
        else { //Noeud n'existe pas dans R
            Noeud *n = insereNoeud(R, x, y);
            insererNoeudArbre(n, a, parent);
            return n;
        }
    }

    /* Cas d'un noeud interne: rappeler recursivement */
    else {
        /* L'argument de l'appel depend des (x,y) par rapport au centre de (*a) */
        if (x < (*a)->xc) {     //dans no / so
            if (y < (*a)->yc) { //dans so
                return rechercheCreeNoeudArbre(R, &(*a)->so, *a, x, y);
            }
            else {              //dans no
                return rechercheCreeNoeudArbre(R, &(*a)->no, *a, x, y);
            }
        }
        else {                  //dans  ne / se
            if (y < (*a)->yc) { //dans se
                return rechercheCreeNoeudArbre(R, &(*a)->se, *a, x, y);
            }
            else {              //dans ne
                return rechercheCreeNoeudArbre(R, &(*a)->ne, *a, x, y);
            }
        }
    }
}

Reseau *reconstitueReseauArbre(Chaines *C) {
    /**
     * Reconstituer en utilisant ArbreQuat le Reseau representant le reseau de Chaines C.
     * Param:
     *  - C : le Chaines du Reseau a construire.
     * Return:
     *  Reseau donne' par le Chaines C.
     */
    if (C == NULL) {
        return NULL;
    }

    Reseau *R = NULL;
    ArbreQuat *racine = NULL;
    double xmin=-1, xmax=-1;
    double ymin=-1, ymax=-1;

    /* Creer le Reseau (vide) */
    R = creerReseauVide(C->gamma);

    /* Creer la racine */
    chaineCoordMinMax(C, &xmin, &ymin, &xmax, &ymax);
    racine = creerArbreQuat((xmax+xmin)/2, (ymax+ymin)/2, xmax-xmin, ymax-ymin);
    if (racine == NULL) {
        fprintf(stderr, "Probleme d'allocation dans reconstitueReseauArbre.\n");
        return NULL;
    }

    CellChaine *ch = C->chaines;
    CellPoint *pt = NULL;
    CellCommodite *commod = NULL;
    CellNoeud *voisin = NULL;
    Noeud *n = NULL;
    Noeud *prev_n = NULL;

    while (ch) {
        /* Mise a jour de la commodite de la Chaine courante */
        commod = (CellCommodite*)malloc(sizeof(CellCommodite));
        if (commod == NULL) {
            fprintf(stderr, "Probleme d'allocation de CellCommodite.\n");
            free(R);
            return NULL;
        }
        /* Insertion en tete de la commodite */
        commod->suiv = R->commodites;
        R->commodites = commod;

        /* Premier Noeud = commodite->extrA */
        pt = ch->points;
        n = rechercheCreeNoeudArbre(R, &racine, NULL, pt->x, pt->y);
        commod->extrA = n;

        pt = pt->suiv;
        while (pt) {

            // prev_n : le noeud precedent
            prev_n = n;
            // n : le noeud courant
            n = rechercheCreeNoeudArbre(R, &racine, NULL, pt->x, pt->y);

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
        /* Dernier Noeud enregistre' est l'autre extremite de la commodite */
        commod->extrB = n;

        ch = ch->suiv;
    }

    libererArbreQuat(racine);

    return R;
}


void libererArbreQuat(ArbreQuat *racine) {
    /**
     * Liberer l'espace memoire occupe' par l'arbre.
     * Le champs a->noeud ne sera PAS libere.
     */
    if (racine != NULL) {
        libererArbreQuat(racine->no);
        libererArbreQuat(racine->ne);
        libererArbreQuat(racine->so);
        libererArbreQuat(racine->se);
    
        free(racine);
    }
}
