#include <stdio.h>
#include <stdlib.h>
#include "Chaine.h"
#include "SVGwriter.h"

int main(int argc, char *argv[]) {
    Chaines *C = NULL;
    FILE *fl = NULL;
    FILE *fe = NULL;
    char *nomAff = "tmp/test_aff";

    if (argc != 3) {
        fprintf(stderr, "Arguments attendus: ");
        fprintf(stderr, "nom du fichier a lire, nom du fichier a ecrire.\n");
        return EXIT_FAILURE;
    }

    fl = fopen(argv[1], "r");
    if (fl == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    fe = fopen(argv[2], "w");
    if (fe == NULL) {
        fprintf(stderr, "Probleme d'ouverture du fichier: %s\n", argv[2]);
        fclose(fl);
        return EXIT_FAILURE;
    }

    C = lectureChaines(fl);
    ecrireChaines(C, fe);

    double sum = longueurTotale(C);
    printf("longueur totale: %lf\n", sum);
    int cpt = comptePointsTotal(C);
    printf("point total: %d\n", cpt);

    afficheChainesSVG(C, nomAff);
    printf("Chaines enregistrees dans %s.html\n", nomAff);

    libererChaines(C);
    fclose(fl);
    fclose(fe);

    return EXIT_SUCCESS;
}
