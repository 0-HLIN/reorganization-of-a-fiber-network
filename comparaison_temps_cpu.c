#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"

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

    libererReseau(R);
    return duree;
}


int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Arguments attendus:\n");
        printf(" - nom fichier contenant les Chaines\n");
        printf(" - nom fichier dans lequel sauvegarder les temps de calculs\n");
        printf(" - la taille de la table de hachage\n");
        return EXIT_FAILURE;
    }

    FILE *fl = NULL; //fichier de lecture
    FILE *fe = NULL; //fichier d'ecriture
    Chaines *C = NULL;
    int M = atoi(argv[3]); //taille de la table de hachage
    double duree = -1;
    
    /* Fichier dans lequel le Chaines est enregistre */
    fl = fopen(argv[1], "r");
    if (fl == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* Fichier dans lequel enregistrer le resultat */
    fe = fopen(argv[2], "a");  //ecrire a la fin du fichier
    if (fe == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", argv[2]);
        fclose(fl);
        return EXIT_FAILURE;
    }


    /* Creation de Chaines */
    C = lectureChaines(fl);


    /* ecrire d'abord le nb de Chaines */
    fprintf(fe, "%d ", C->nbChaines);

    /* Le temps de cpu de l'implementation par liste chainee */
    duree = temps_cpu(C, 1, -1);
    fprintf(fe, "%f ", duree);

    /* Le temps de cpu de l'implementation par table de hachage */
    duree = temps_cpu(C, 2, M);
    fprintf(fe, "%f ", duree);

    /* Le temps de cpu de l'implementation par ArbreQuat */
    duree = temps_cpu(C, 3, -1);
    fprintf(fe, "%f \n", duree);


    /* Liberation de ressources */
    fclose(fl);
    fclose(fe);
    libererChaines(C);

    return EXIT_SUCCESS;
}
