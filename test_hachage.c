#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Hachage.h"


int main(){

    /* On teste si la fonction donnant la cle d'un point est bien appropri'ee */
    for(double x=1; x<=10; x++){
        for(double y=1; y<=10; y++){
            printf("f(x=%f, y=%f) = %d \n", x, y, f(x,y));
        }
    }
    /* Rmrq : les r'esultats montrent que elle est appropri'ee : on n'a jamais 2 points qui ont la meme cl'e */

    return 0;
}
