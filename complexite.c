#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"


//voir la fin du fichier pour la definition
double temps_cpu(Chaines *C, int recons_type, int M);


int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("ARgument attendu: \n");
        printf(" - Nom du fichier dans lequel ecrit le resultat.\n");
        return EXIT_FAILURE;
    }

    /* parametres sur les Chaines gengerees */
    const int nbPointsChaine = 100;
    const int xmax = 5000;
    const int ymax = 5000;

    /* parametre sur le type de l'implementation utilsee
     * 1 : liste chainee
     * 2 : table de hachage 
     * 3 : arbre quaternaire
     */
    const int recons_type = 3;
    const int M = 500000;  //taille de la table de hachage (pour l'implementation hachage)

    /* parametres sur le parcours (echantillonage) */
    const int nbMin = 500;   //valeur initiale de nbChaines
    const int nbMax = 5000;  //valeur finale de nbChaines
    const int pas   = 500;   //pas de iteration


    FILE *f_res;  //fichier enregistrant les resultats
    Chaines *C = NULL;
    double duree = -1.0;

    /* vider le fichier dans lequel ecrit les resultats */
    if ((f_res = fopen(argv[1], "w")) == NULL) {
        printf("Probleme d'ouverture du fichier: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* Afficher les informations concernant cette execution */
    printf("nbPointsChaine=%d  xmax=%d  ymax=%d\n", nbPointsChaine, xmax, ymax);
    printf("recons_type=%d  ", recons_type);
    if (recons_type == 2) {
        printf("M=%d\n", M);
    } else {
        printf("\n");
    }

    for (int nbChaines = nbMin; nbChaines <= nbMax; nbChaines += pas) {
        printf("nbMin=%d  nbChaines=%d  pas=%d  nbMax=%d\n", nbMin, nbChaines, pas, nbMax);

        /* generer des Chaines aleatoires */
        C = generationAleatoire(nbChaines, nbPointsChaine, xmax, ymax);

        /* Faire les calculs de complexite */
        duree = temps_cpu(C, recons_type, M);
        fprintf(f_res, "%d %f\n", nbChaines, duree);

        /* liberer les Chaines */
        libererChaines(C);
    }

    /* Liberation de ressources */
    fclose(f_res);

    return EXIT_SUCCESS;
}


double temps_cpu(Chaines *C, int recons_type, int M) {
    /*
     * Reconstitue Reseau a' partir de Chaines C, en utilisant la methode correspondant a recons_type.
     *
     * Arguments:
     *  - C           : Chaines a partir des quelles reconstitue Reseau
     *  - recons_type : l'implementation de la reconstrcution:
     *      =1  pour la liste chainee
     *      =2  pour la table de hachage
     *      =3  pour l'arbre quaternaire
     *  - M           : la taille de la table de hachage, il est ignore' si recons_type != 2
     *
     * Retourne: le temps de cpu utilise' en seconde pour la recontruction de Reseau.
     */
    ;
    clock_t tempsDebut;
    clock_t tempsFin;
    double duree = -1;
    Reseau *R = NULL;

    switch (recons_type) {
        /* Le temps de cpu de l'implementation par liste chainee */
        case 1:
        tempsDebut = clock();
        R = reconstitueReseauListe(C);
        tempsFin = clock();
        duree = (double)(tempsFin - tempsDebut) / CLOCKS_PER_SEC;
        libererReseau(R);
        break;

        /* Le temps de cpu de l'implementation par la table de hachage */
        case 2:
        tempsDebut = clock();
        R = reconstitueReseauHachage(C, M);
        tempsFin = clock();
        duree = (double)(tempsFin - tempsDebut) / CLOCKS_PER_SEC;
        libererReseau(R);
        break;

        /* Le temps de cpu de l'implementation par l'arbre quaternaire */
        default:
        tempsDebut = clock();
        R = reconstitueReseauArbre(C);
        tempsFin = clock();
        duree = (double)(tempsFin - tempsDebut) / CLOCKS_PER_SEC;
        libererReseau(R);
        break;
    }

    return duree;
}

