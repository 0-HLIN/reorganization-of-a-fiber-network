#include <stdio.h>
#include <stdlib.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"
#include "Graphe.h"
#include "SVGwriter.h"

int main(int argc, char *argv[]) {
    char *nomAff = "tmp/SVG_aff"; //nom du fichier pour enregistrer les chaines (format .html)
    Chaines *C = NULL;
    Reseau *R = NULL;
    FILE *fl = NULL;

    if (argc != 2) {
        fprintf(stderr, "Arguments attendus: ");
        fprintf(stderr, "nom du fichier .cha a' lire.\n");
        return EXIT_FAILURE;
    }

    fl = fopen(argv[1], "r");
    if (fl == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    printf("debut lecturechaine ...\n");
    C = lectureChaines(fl);
    printf("fin lectureChaines\n");

    afficheChainesSVG(C, nomAff);
    printf("les chaines enregistrees dans %s\n", nomAff);

    printf("reconstitueReseau...\n");
    R = reconstitueReseauArbre(C);
    //R = reconstitueReseauListe(C);
    printf("fin reconstruction\n");

    printf("reorganiseReseau...\n");
    printf(" resultat = %d\n", reorganiseReseau(R));
    printf("fin reorganiseReseau\n");

    libererChaines(C);
    libererReseau(R);
    fclose(fl);

    return EXIT_SUCCESS;
}
