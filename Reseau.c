#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Reseau.h"
#include "Chaine.h"
#include "SVGwriter.h"


/* fonction auxiliaire */
Reseau* creerReseauVide(int gamma) {
    /**
     * Creer un Reseau R vide
     */
    Reseau *R = (Reseau*)malloc(sizeof(Reseau));
    if (R == NULL) {
        fprintf(stderr, "Proble d'allocation de Reseau.\n");
        return NULL;
    }

    /* Initialisation de champs */
    R->nbNoeuds = 0;
    R->gamma = gamma;
    R->noeuds = NULL;
    R->commodites = NULL;

    return R;
}

/* fonction auxiliaire */
Noeud* insereNoeud(Reseau *R, double x, double y) {
    /**
     * Creer et inserer un noeud de coord (x,y) sans verifier l'existence
     */
    CellNoeud *new = (CellNoeud*)malloc(sizeof(CellNoeud));
    if (new == NULL) {
        fprintf(stderr, "Probleme d'allocation de CellNoeud\n");
        return NULL;
    }
    new->nd = (Noeud*)malloc(sizeof(Noeud));
    if (new->nd == NULL) {
        fprintf(stderr, "Probleme d'allocation de Noeud\n");
        free(new);
        return NULL;
    }
    new->suiv = R->noeuds;
    R->noeuds = new;
    new->nd->x = x;
    new->nd->y = y;
    new->nd->num = ++(R->nbNoeuds);
    new->nd->voisins = NULL;

    return new->nd;
}
    

Noeud* rechercheCreeNoeudListe(Reseau *R, double x, double y) {
    if (R == NULL) return NULL;

    /* Chercher le Noeud dans R */
    CellNoeud *n = R->noeuds;
    while (n) {
        if (n->nd->x == x && n->nd->y == y) {
            return n->nd;
        }
        n = n->suiv;
    }

    /* Creer le Noeud et l'inserer dans R*/
    Noeud *nd = insereNoeud(R, x, y);

    return nd;
}


Reseau* reconstitueReseauListe(Chaines *C) {
    if (C == NULL) return NULL;

    Reseau *R = NULL;
    CellCommodite *commod = NULL;
    Noeud *n = NULL;
    Noeud *prev_n = NULL;
    CellNoeud *voisin = NULL;
    CellChaine *c = NULL;
    CellPoint *pt = NULL;

    /* Creation de Reseau vide */
    R = creerReseauVide(C->gamma);

    /* Insertion de CellNoeud et de CellCommodite */
    c = C->chaines;
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
        n = rechercheCreeNoeudListe(R, pt->x, pt->y);
        commod->extrA = n;
        pt = pt->suiv;
        while (pt) {
            // prev_n : le noeud precedent
            prev_n = n;
            // n : le noeud courant
            n = rechercheCreeNoeudListe(R, pt->x, pt->y);

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

    return R;
}


int nbCommodites(Reseau *R) {
    if (R == NULL) {
        return 0;
    }

    int cnt = 0;
    CellCommodite *commod = R->commodites;
    while (commod) {
        ++cnt;
        commod = commod->suiv;
    }

    return cnt;
}

int nbLiaisons(Reseau *R) {
    /* Retourne le nombre de liaison dans le reseau.
     * Il s'agit d'une moitié de la somme des nombres de voisins de tous les noeuds.
     */
    if (R == NULL) {
        return 0;
    }

    int cnt = 0; //compte le nb de liaisons
    CellNoeud *cn = NULL;
    CellNoeud *voisin = NULL;

    cn = R->noeuds;
    while (cn) {
        voisin = cn->nd->voisins;
        while (voisin) {
            if (cn->nd->num < voisin->nd->num) { //pour eviter de compte 2 fois la meme liaison
                ++cnt;
            }
            voisin = voisin->suiv;
        }
        cn = cn->suiv;
    }

    return cnt;
}


void ecrireReseau(Reseau *R, FILE *f) {
    if (R == NULL) {
        return;
    }

    CellNoeud *cn = NULL;
    CellNoeud *voisin = NULL;
    CellCommodite *cmd = NULL;
    int nbN = R->nbNoeuds;
    int nbL = nbLiaisons(R);
    int nbC = nbCommodites(R);

    fprintf(f, "NbNoeuds: %d\n", nbN);
    fprintf(f, "NbLiaisons: %d\n", nbL);
    fprintf(f, "NbCommodites: %d\n", nbC);
    fprintf(f, "Gamma: %d\n\n", R->gamma);

    /* Affichage des noeuds */
    cn = R->noeuds;
    while (cn) {
        fprintf(f, "v %d %f %f\n", cn->nd->num, cn->nd->x, cn->nd->y);
        cn = cn->suiv;
    }
    fprintf(f, "\n");

    /* Affichage des liaisons */
    cn = R->noeuds;
    while (cn) {
        voisin = cn->nd->voisins;
        while (voisin) {
            /* Pour eviter d'afficher 2 fois la meme liaison */
            if (cn->nd->num < voisin->nd->num) {
                fprintf(f, "l %d %d\n", cn->nd->num, voisin->nd->num);
            }
            voisin = voisin->suiv;
        }
        cn = cn->suiv;
    }
    fprintf(f, "\n");

    /* Affichage des commodites */
    cmd = R->commodites;
    while (cmd) {
        fprintf(f, "k %d %d\n", cmd->extrA->num, cmd->extrB->num);
        cmd = cmd->suiv;
    }
}



void afficheReseauSVG(Reseau *R, char* nomInstance){
    CellNoeud *courN,*courv;
    SVGwriter svg;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;

    courN=R->noeuds;
    while (courN!=NULL){
        if (maxx<courN->nd->x) maxx=courN->nd->x;
        if (maxy<courN->nd->y) maxy=courN->nd->y;
        if (minx>courN->nd->x) minx=courN->nd->x;
        if (miny>courN->nd->y) miny=courN->nd->y;
        courN=courN->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    courN=R->noeuds;
    while (courN!=NULL){
        SVGpoint(&svg,500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
        courv=courN->nd->voisins;
        while (courv!=NULL){
            if (courv->nd->num<courN->nd->num)
                SVGline(&svg,500*(courv->nd->x-minx)/(maxx-minx),500*(courv->nd->y-miny)/(maxy-miny),500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
            courv=courv->suiv;
        }
        courN=courN->suiv;
    }
    SVGfinalize(&svg);
}


void libererReseau(Reseau *R) {
    if (R == NULL) {
        return;
    }

    CellNoeud *cnCur = NULL;
    CellNoeud *voisin = NULL;
    CellNoeud *cnTmp = NULL;
    CellCommodite *cmCur = NULL;
    CellCommodite *cmTmp = NULL;

    /* Liberer les CellNoeud et les Noeud */
    cnCur = R->noeuds;
    while (cnCur) {
        voisin = cnCur->nd->voisins;
        // Liberer les voisins (les CellNoeud)
        while (voisin) {
            cnTmp = voisin->suiv;
            // Liberer CellNoeud de chaque voisin (mais pas le Noeud a l'interieur)
            free(voisin);
            voisin = cnTmp;
        }
        // Liberer le Noeud et le CellNoeud courant
        cnTmp = cnCur->suiv;
        free(cnCur->nd);
        free(cnCur);
        cnCur = cnTmp;
    }

    /* Liberer les commodites */
    cmCur = R->commodites;
    while (cmCur) {
        cmTmp = cmCur->suiv;
        // Ne pas liberer les Noeud ici
        free(cmCur);
        cmCur = cmTmp;
    }

    free(R);
}
