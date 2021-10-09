#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Struct_File.h"
#include "Struct_Liste.h"
#include "Reseau.h"
#include "Graphe.h"

/* Exo 7.1 */
Graphe *creerGraphe(Reseau *r) {
    /* Creer la strucutre Graphe correspondant au Reseau r.
     * numero du sommet dans le Graphe egal a num du noeud dans r -1.
     */
    if (r == NULL) {
        return NULL;
    }

    /* Creation de graphe vide */
    Graphe *G = (Graphe*)malloc(sizeof(Graphe));
    if (G == NULL) {
        fprintf(stderr, "Probleme d'allocation de Graphe\n");
        return NULL;
    }
    G->nbsom = r->nbNoeuds;
    G->gamma = r->gamma;
    G->T_commod = NULL;

    /* Allocation de tableau T_som */
    G->T_som = (Sommet**)malloc(sizeof(Sommet*)*G->nbsom);
    if (G->T_som == NULL) {
        fprintf(stderr, "Probleme d'allocation de T_som.\n");
        free(G);
        return NULL;
    }

    /* Ajout des sommets (en 2 etapes) */
    CellNoeud *cn = NULL;
    CellNoeud *voisin = NULL;
    Cellule_arete *ca = NULL;
    Arete *a = NULL;
    /* - Creation de tous les sommets (sans voisins) */
    cn = r->noeuds;
    while (cn) {
        Sommet *s = (Sommet*)malloc(sizeof(Sommet));
        if (s == NULL) {
            fprintf(stderr, "Probleme d'allocation de Sommet\n");
            libererGraphe(G);
            return NULL;
        }
        s->num = cn->nd->num - 1;
        s->x = cn->nd->x;
        s->y = cn->nd->y;
        s->L_voisin = NULL;
        G->T_som[cn->nd->num-1] = s;

        cn = cn->suiv;
    }

    /* - Ajouts des voisins */
    cn = r->noeuds;
    while (cn) {
        Sommet *s = G->T_som[cn->nd->num-1];

        voisin = cn->nd->voisins;
        while (voisin) {
            // Pour eviter de creer 2 fois la meme arete
            if (s->num < voisin->nd->num-1) {
                //Creer l'arete
                a = (Arete*)malloc(sizeof(Arete));
                if (a == NULL) {
                    fprintf(stderr, "Probleme d'allocation d'Arete\n");
                    libererGraphe(G);
                    return NULL;
                }
                a->u = s->num;
                a->v = voisin->nd->num-1;

                //Creer le Cellule_arete pour s
                ca = (Cellule_arete*)malloc(sizeof(Cellule_arete));
                if (ca == NULL) {
                    fprintf(stderr, "Probleme d'allocation de Cellule_arete\n");
                    libererGraphe(G);
                    return NULL;
                }
                ca->a = a;
                //Inserer le nouvel element en tete de L_voisin de s
                ca->suiv = s->L_voisin;
                s->L_voisin = ca;

                //Creer le Cellule_arete pour voisin
                ca = (Cellule_arete*)malloc(sizeof(Cellule_arete));
                if (ca == NULL) {
                    fprintf(stderr, "Probleme d'allocation de Cellule_arete\n");
                    libererGraphe(G);
                    return NULL;
                }
                ca->a = a;
                //Inserer le nouvel element en tete de L_voisin de sommets correspondant a voisin
                ca->suiv = G->T_som[voisin->nd->num-1]->L_voisin;
                G->T_som[voisin->nd->num-1]->L_voisin = ca;
            }
            voisin = voisin->suiv;
        }
        cn = cn->suiv;
    }


    /* Ajout des commodites */

    //Calculons d'abord le nombre de commodites */
    CellCommodite *cmd = r->commodites;
    int nbCmd = 0;
    while (cmd) {
        ++nbCmd;
        cmd = cmd->suiv;
    }
    G->nbcommod = nbCmd;

    //Creation de table de commodites
    G->T_commod = (Commod*)malloc(sizeof(Commod)*nbCmd);
    if (G->T_commod == NULL) {
        fprintf(stderr, "Probleme d'allocation de la table de Commod\n");
        libererGraphe(G);
        return NULL;
    }

    //Ajout de commodites dans la table
    cmd = r->commodites;
    int i = 0;
    while (cmd) {
        G->T_commod[i].e1 = cmd->extrA->num -1;
        G->T_commod[i].e2 = cmd->extrB->num -1;
        ++i;
        cmd = cmd->suiv;
    }


    return G;
}


/* Exo 7.2 */
int distance(Graphe *G, int u, int v) {
    /*
     * Calculer la distance (le plus petit nb d'arêtes) entre u et v,
     *  deux sommets de G.
     * Algo:
     *  Il s'agit d'un parcours en largeur des sommets a partir de u.
     *  la fonction retourne -1 si la bordure est vide mais v n'est pas rencontre',
     *  ou la distance entre u et v si v est rencontre'.
     */
    if (u == v) {
        return 0;
    }

    // tableau qui stocke les sommets visites par leurs distance a' u
    int *visit = (int*)malloc(sizeof(int) * G->nbsom);
    if (visit == NULL) {
        fprintf(stderr, "Probleme d'allocation de 'visit'.\n");
        return -2;
    }
    for (int i=0; i<G->nbsom; ++i) {
        visit[i] = -1; //valeur -1 si pas encore rencontre'
    }
    visit[u] = 0;

    // file qui stocke la bordure du sous-parcours courant
    S_file bordure;
    Init_file(&bordure);
    enfile(&bordure, u);

    while (!estFileVide(&bordure)) {
        int w = defile(&bordure);

        Cellule_arete *voisin = G->T_som[u]->L_voisin;
        /* parcours les sommets adjacents */
        while (voisin) {
            // Numero du sommet adjacent
            int num_adj = voisin->a->u != w ? voisin->a->u : voisin->a->v;
            if (visit[num_adj] == -1) { //si le sommet n'est pas encore visite'
                visit[num_adj] = visit[w] + 1; //distance à u est celle entre u et w augmente' de 1.
                if (num_adj == v) { //v est trouve', retourne de la fonction
                    /* Liberation de ressources */
                    free(visit);
                    while (!estFileVide(&bordure)) {
                        defile(&bordure); //vider la file
                    }
                    return visit[num_adj];
                }
                enfile(&bordure, num_adj); 
            }
            voisin = voisin->suiv;
        }
    }

    /* Il n'existe pas de chaine entre u et v */
    /* Liberation de ressources */
    free(visit);
    while (!estFileVide(&bordure)) {
        defile(&bordure); //vider la file
    }

    return -1;
}


/* Exo 7.3 */
ListeEntier plus_courte_chaine(Graphe *G, int u, int v) {
    /*
     * Calculer la plus courte chaine entre u et v, deux sommets de G.
     * La fonction suppose que u != v
     * 
     * Il s'agit d'un parcours en largeur des sommets a partir de u.
     * La fonction va construire une 'arborescence' des chemins issus de u, 
     *  à difference d'une arborescence que chaque noeud enregistre le numero de son pere.
     * La fonction retourne NULL si la bordure est vide avant que v soit rencontre',
     *  ou la liste d'entier correspondant a la plus courte chaine de u à v.
     */
    if (u == v) {
        fprintf(stderr, "Attention, dans la fonction 'plus_courte_chaine': u == v\n");
        return NULL;
    }

    /* tableau: visit[u] contient -1 si u n'est pas encore visite', 
     *  numero de son pere dans l'arboresence des chemins sion.
     */
    int visit[G->nbsom];
    for (int i=0; i<G->nbsom; ++i) {
        visit[i] = -1; //valeur -1 si pas encore rencontre'
    }
    visit[u] = u; //cette valeur n'a vraiement pas d'importance, u etant la racine

    // file qui stocke la bordure du sous-parcours courant
    S_file bordure;
    Init_file(&bordure);
    enfile(&bordure, u);

    int trouve = 0;
    while (!estFileVide(&bordure) && !trouve) {
        int w = defile(&bordure);

        Cellule_arete *voisin = G->T_som[w]->L_voisin;
        /* parcours les sommets adjacents */
        while (voisin) {
            // Numero du sommet adjacent
            int num_adj = voisin->a->u != w ? voisin->a->u : voisin->a->v;
            if (visit[num_adj] == -1) { //si le sommet n'est pas encore visite'
                visit[num_adj] = w;
                if (num_adj == v) { //v est trouve', arreter la recherche
                    trouve = 1;
                    break;
                }
                enfile(&bordure, num_adj); 
            }
            voisin = voisin->suiv;
        }
    }

    ListeEntier LE = NULL;
    if (trouve) {  // Il existe une chaine entre u et v, creons-le
        Init_Liste(&LE);
        int w = v;
        while (w != u) {
            ajoute_en_tete(&LE, w);
            w = visit[w];
        }
        ajoute_en_tete(&LE, u);
    }

    /* Liberation de ressources */
    while (!estFileVide(&bordure)) {
        defile(&bordure); //vider la file
    }

    return LE;
}


int **alloueMatrice(int taille) {
    /*
     * Allouer un tableau taille * taille, les cases initialisees a' 0.
     * Utilise' par 'reorganiseReseau'
     */
    int **mat = (int**)malloc(sizeof(int*)*taille);
    if (mat == NULL) {
        fprintf(stderr, "Probleme d'allocation de la matrice (1)\n");
        return NULL;
    }
    for (int i=0; i<taille; ++i) {
        mat[i] = (int*)malloc(sizeof(int)*taille);
        if (mat[i] == NULL) {
            fprintf(stderr, "Probleme d'allocation de la matrice (2)\n");
            /* Liberer les memoires allouees */
            for (int j=0; j<i; ++j) {
                free(mat[j]);
            }
            free(mat);
            return NULL;
        }
        /* Initialiser les cases a 0 */
        memset(mat[i], 0, sizeof(int)*taille);
    }
    return mat;
}

void desalloueMatrice(int **mat, int taille) {
    /*
     * desallouer une table taille * taille.
     */
    for (int i=0; i<taille; ++i) {
        free(mat[i]);
    }
    free(mat);
}

/* Exo 7.4 */
int reorganiseReseau(Reseau *r) {
    /*
     * Retourne 
     *  1 si pour toute arete du graphe minimise', 
     *   le nombre de chaines passant par cette arete est inferieur a' gamma.
     *  0 sinon.
     */
    Graphe *G = creerGraphe(r);

    // La matrice le nombre de chaines entre chaque pair de sommets
    int **nbCh = alloueMatrice(G->nbsom); //les cases sont initilialisees a 0

    /* Parcours des commodites */
    for (int i=0; i<G->nbcommod; ++i) {
        Commod cmd = G->T_commod[i];
        /* Calculer la plus courte chaine */
        ListeEntier pcc = plus_courte_chaine(G, cmd.e1, cmd.e2);
        if (pcc == NULL) {
            fprintf(stderr, "Erreur: il n'y a pas de chaine entre %d et %d\n", cmd.e1, cmd.e2);
        }
        /* Parcourir cette chaine et mettre a jour la matrice 'nbCh' */
        Cell_entier *ce1 = pcc;         //Cell_entier contenant cmd.e1
        Cell_entier *ce2 = pcc->suiv;   //Cell_entier contenant le sucesseur de cmd.e1
        while (ce1->u != cmd.e2) {
            ++(nbCh[ce1->u][ce2->u]);
            ++(nbCh[ce2->u][ce1->u]);
            if (nbCh[ce1->u][ce2->u] > G->gamma) {
                /* Liberation de ressources */
                desalloue(&pcc);
                desalloueMatrice(nbCh, G->nbsom);
                libererGraphe(G);
                return 0;
            }
            ce1 = ce2;
            ce2 = ce1->suiv;
        }
        /* Libération de ListeEntier pcc */
        desalloue(&pcc);
    }

    desalloueMatrice(nbCh, G->nbsom);
    libererGraphe(G);
    return 1;
}


void libererGraphe(Graphe *G) {
    /* Si G n'est pas correctement construit, la fonction peut provoquer des erreurs.
     * Par exemple si elle est appelee par la fonction creerGraphe en cours de la construction de G.
     */
    if (G == NULL) {
        return;
    }

    /* liberation de commodites */
    free(G->T_commod);

    /* liberation de sommets */
    Sommet *s = NULL;
    Cellule_arete *voisin = NULL;
    Cellule_arete *tmp = NULL;

    for (int i=0; i<G->nbsom; ++i) {
        s = G->T_som[i];

        /* liberation de voisins */
        voisin = s->L_voisin;
        while (voisin) {
            tmp = voisin->suiv;
            /*
             * Pour eviter de liberer 2 fois la meme arete
             * on met le numero de sommet u dans l'arete a' -1 lors de la 1e rencontre,
             * et la liberer lors de la deuxieme rencontre. 
             */
            if (voisin->a->u == -1) { //c'est la deuxieme rencontre
                free(voisin->a);
            } else { //c'est la premiere rencontre
                voisin->a->u = -1;
            }
            free(voisin);
            voisin = tmp;
        }
        free(s);
    }

    free(G->T_som);
    free(G);

}

void ecrireGraphe(Graphe *G, FILE *f) {
    /* Ecrire les informations (sommets, voisins, commodites) dans le fichier f.
     * Utilise' pour deboggage.
     */

    fprintf(f, "Gamma: %d\n\n", G->gamma);

    /* Affichage des sommets (numero, coordonnees, voisins) */
    fprintf(f, "Sommets: (num x y voisins..)\n");
    for (int i=0; i<G->nbsom; ++i) {
        Sommet *s = G->T_som[i];
        fprintf(f, "%2d %6g %6g", s->num, s->x, s->y);

        /* Numero des voisins */
        Cellule_arete *v = s->L_voisin;
        while (v) {
            fprintf(f, " %2d", v->a->u != s->num ? v->a->u : v->a->v);
            v = v->suiv;
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");

    /* Affichage des commodites */
    fprintf(f, "Commidites:\n");
    for (int i=0; i<G->nbcommod; ++i) {
        fprintf(f, "%d %d\n", G->T_commod[i].e1, G->T_commod[i].e2);
    }

}
