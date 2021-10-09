#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Chaine.h"
#include "SVGwriter.h"


Chaines *lectureChaines(FILE *f) {
    int nb = -1;
    Chaines *C = NULL;
    CellPoint *pt = NULL;
    CellChaine *ch = NULL;

    C = (Chaines*)malloc(sizeof(Chaines));
    if (C == NULL) {
        fprintf(stderr, "Probleme d'allocation de Chaines.\n");
        return NULL;
    }
    C->chaines = NULL;
    if (fscanf(f, "NbChain: %d Gamma: %d", &(C->nbChaines), &(C->gamma))!=2) {
        fprintf(stderr, "Format fichier incorrect.(nbChaines, gamma)\n");
        free(C);
        return NULL;
    }
    for (int i=0; i < C->nbChaines; ++i) {
        ch = (CellChaine*)malloc(sizeof(CellChaine));
        if (ch == NULL) {
            fprintf(stderr, "Probleme d'allocation de CellChaine.\n");
            libererChaines(C);
            return NULL;
        }
        ch->suiv = NULL;
        ch->points = NULL;
        if (fscanf(f, " %d %d", &(ch->numero), &nb)!=2) {
            fprintf(stderr, "Format fichier incorrect. (ch->numero, nb)\n");
            libererChaines(C);
            return NULL;
        }
        for (int j=0; j<nb; ++j) {
            pt = (CellPoint*)malloc(sizeof(CellPoint));
            if (pt == NULL) {
                fprintf(stderr, "Probleme d'allocation de CellPoint.\n");
                libererChaines(C);
                return NULL;
            }
            if (fscanf(f, " %lf %lf", &(pt->x), &(pt->y))!=2) {
                fprintf(stderr, "Format fichier incorrect. (pt->x, pt->y)\n");
                libererChaines(C);
                return NULL;
            }
            pt->suiv = ch->points;
            ch->points = pt;
        }
        ch->suiv = C->chaines;
        C->chaines = ch;
    }
    return C;
}

void ecrireChaines(Chaines *C, FILE *f) {
    if (C == NULL) return;

    CellChaine *ch = C->chaines;
    CellPoint *pt = NULL;
    int nbPts = -1;

    fprintf(f, "NbChain: %d\n", C->nbChaines);
    fprintf(f, "Gamma: %d\n\n", C->gamma);
    while (ch != NULL) {
        nbPts = 0;
        pt = ch->points;
        while (pt != NULL) {
            pt = pt->suiv;
            ++nbPts;
        }
        fprintf(f, "%d %d", ch->numero, nbPts);
        pt = ch->points;
        while (pt != NULL) {
            fprintf(f, " %g %g", pt->x, pt->y);
            pt = pt->suiv;
        }
        fprintf(f, "\n");

        ch = ch->suiv;
    }
}

void libererCellChaine(CellChaine *ch) {
    /*      */
    if (ch == NULL) return;

    CellPoint *pt = ch->points;
    CellPoint *tmp = NULL;

    while (pt != NULL) {
        tmp = pt->suiv;
        free(pt);
        pt = tmp;
    }
    free(ch);
}

void libererChaines(Chaines *C){
    /*    */
    if (C == NULL) return;

    CellChaine *ch = C->chaines;
    CellChaine *tmp = NULL;

    while (ch != NULL) {
        tmp = ch->suiv;
        libererCellChaine(ch);
        ch = tmp;
    }
    free(C);
}



/* Creer fichier html 'nomInstance' permettant d'afficher les chaines */
void afficheChainesSVG(Chaines *C, char* nomInstance){
    //int i;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;
    CellChaine *ccour;
    CellPoint *pcour;
    double precx,precy;
    SVGwriter svg;
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        while (pcour!=NULL){
            if (maxx<pcour->x) maxx=pcour->x;
            if (maxy<pcour->y) maxy=pcour->y;
            if (minx>pcour->x) minx=pcour->x;
            if (miny>pcour->y) miny=pcour->y;  
            pcour=pcour->suiv;
        }
    ccour=ccour->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        SVGlineRandColor(&svg);
        SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny)); 
        precx=pcour->x;
        precy=pcour->y;  
        pcour=pcour->suiv;
        while (pcour!=NULL){
            SVGline(&svg,500*(precx-minx)/(maxx-minx),500*(precy-miny)/(maxy-miny),500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            precx=pcour->x;
            precy=pcour->y;    
            pcour=pcour->suiv;
        }
        ccour=ccour->suiv;
    }
    SVGfinalize(&svg);
}


double longueurChaine(CellChaine *c) {
    if (c == NULL) return 0.0;

    double x, y;
    double sum = 0.0;
    CellPoint *pt = c->points;
    if (pt == NULL) {
        return 0.0;
    }
    x = pt->x;
    y = pt->y;
    pt = pt->suiv;
    while (pt != NULL) {
        sum += sqrt((pt->x - x)*(pt->x - x) + (pt->y - y)*(pt->y - y) );
        x = pt->x;
        y = pt->y;
        pt = pt->suiv;
    }

    return sum;
}

double longueurTotale(Chaines *C) {
    if (C == NULL) {
        return 0.0;
    }
    CellChaine *c = C->chaines;
    double sum = 0.0;
    while (c != NULL) {
        sum += longueurChaine(c);
        c = c->suiv;
    }

    return sum;
}


int comptePointsTotal(Chaines *C) {
    if (C == NULL) {
        return 0;
    }
    int cpt = 0;
    CellChaine *c = C->chaines;
    CellPoint *pt = NULL;

    while (c) {
        pt = c->points;
        while (pt) {
            ++cpt;
            pt = pt->suiv;
        }
        c = c->suiv;
    }

    return cpt;
}


Chaines *generationAleatoire(int nbChaines, int nbPointsChaine, int xmax, int ymax) {
    int nb = -1;
    Chaines *C = NULL;
    CellPoint *pt = NULL;
    CellChaine *ch = NULL;

    C = (Chaines*)malloc(sizeof(Chaines));
    if (C == NULL) {
        fprintf(stderr, "Probleme d'allocation de Chaines.\n");
        return NULL;
    }
    C->chaines = NULL;
    C->gamma=-1; //TODO
    C->nbChaines= nbChaines;

    for (int i=0; i < nbChaines; ++i) {  //Cr'eation de nbChaines
        ch = (CellChaine*)malloc(sizeof(CellChaine));
        if (ch == NULL) {
            fprintf(stderr, "Probleme d'allocation de CellChaine.\n");
            libererChaines(C);
            return NULL;
        }
        ch->suiv = NULL;
        ch->points = NULL;
        ch->numero = ++nb;
        for (int j=0; j < nbPointsChaine; ++j) { //Cr'eation de nbPointsChaine points par chaine
            pt = (CellPoint*)malloc(sizeof(CellPoint));
            if (pt == NULL) {
                fprintf(stderr, "Probleme d'allocation de CellPoint.\n");
                libererChaines(C);
                return NULL;
            }
            pt->x = (rand()%(100*(xmax)))/100.0 ; //On affecte al'eatoirement x entre 0 et xmax
            pt->y = (rand()%(100*(ymax)))/100.0 ; //On affecte al'eatoirement y entre 0 et ymax
            pt->suiv = ch->points;
            ch->points = pt;
        }
        ch->suiv = C->chaines;
        C->chaines = ch;
    }
    return C;
}


