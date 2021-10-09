#include <stdlib.h>
#include <stdio.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"


int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 4) {
        printf("ARguments attendus: nomFic entier\n");
        printf("- nomFic: un fichier .cha\n");
        printf("- entier: 1 pour liste, 2 pour table de hachage, 3 pour arbre.\n");
        printf("- (si necessaire) la taille de la table de hachage.\n");
        return EXIT_FAILURE;
    }
    if (atoi(argv[2]) == 2 && argc != 4) {
        printf("Veuillez donner la taille de la table de hachage.\n");
        return EXIT_FAILURE;
    }

    char *nomfe = "tmp/tmp.res"; //nom du fichier dans lequel ecrire le resultat

    FILE *fl = NULL; //fichier de lecture
    FILE *fe = NULL; //fichier d'ecriture
    Chaines *C = NULL;
    Reseau *R = NULL;
    

    /* Fichier dans lequel le Chaines est enregistre */
    fl = fopen(argv[1], "r");
    if (fl == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* Fichier dans lequel enregistrer le resultat */
    fe = fopen(nomfe, "w");
    if (fe == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", nomfe);
        fclose(fl);
        return EXIT_FAILURE;
    }

    /* Creation de Chaines */
    C = lectureChaines(fl);

    /* Construction de Reseau */
    switch (atoi(argv[2])) {
        case 1:
            R = reconstitueReseauListe(C);
            break;

        case 2:
            R = reconstitueReseauHachage(C, atoi(argv[3]));
            break;

        case 3:
            R = reconstitueReseauArbre(C);
            break;
            
        default:
            printf("Argument inconnu.\n");

    }

    /* Ecriture du resultat */
    ecrireReseau(R, fe);
    printf("reseau ecrit dans le ficheir '%s'\n", nomfe);

    /* Liberation de ressources */
    libererReseau(R);
    libererChaines(C);
    fclose(fe);
    fclose(fl);

    return EXIT_SUCCESS;
}
