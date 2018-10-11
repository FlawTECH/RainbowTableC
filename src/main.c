#include <time.h>
#include <stdio.h>
#include "../lib/sha256.h"
#include <stdlib.h>
#include <string.h>

/* alphanumeric: [a-z0-9] */
const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";

/**
 * return un entier [0, n]. 
 * n : la longeur de alphabet.
 */
int intN(int n) {
    return rand() % n; 
}

/**
 * Input: longeur du string random [a-z0-9] à générer
 */
char *randomString(int length) {
    //alloue la taille de randstr de longeur length
    char *randstr = malloc((length + 1));
    int i;
    
    for (i = 0; i < length; i++) {
        randstr[i] = alphabet[intN(strlen(alphabet))];
    }
    randstr[length] = '\0';
    printf("\nrandstr:%d\n\n",strlen(randstr));
    return randstr;
}

int main()
{
    //fait en sorte que la fonction "rand" genère un nouveau nombre random en fonction du temps
    srand(time(NULL));

    int hello;
    int hellos[5] = {0,2,4,6,8};
    char hellow[6] = "Hello";
    hello = 3;
    printf("%5s world !%d", hellow, hellos[2]);

    printf("\n\n%s\n", randomString(8));

    system("pause");
    return 0;
}


