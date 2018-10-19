#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../lib/sha256.h"
#define LENGTH_MAX 10000
#define LENGTH_STRING 74
#define NB_PASS 10
#define FILEPATH "bin/password.txt"

#include <time.h>
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
    return randstr;
}

void passwordHashing(char* password, unsigned char* hash)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);

    sha256_update(&ctx,password,sizeof password);
    sha256_final(&ctx,hash);

}

void PrintHex(unsigned char * data)
{

    for (int i=0; i<32; i++) {

        printf("%02x",data[i]);

    }

}

void hash2string(unsigned char* hash, int length, char* string) {
    for(int i = 0; i<length; i++) {
        sprintf(string+i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }
}

void reduce_hash(unsigned char * hash, char * new_password, int indexreduc)
{
    char hash_entier[65];
    int i,x;
    char * alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* hash_separe = malloc(8);

    for(i = 0; i<32; i++) {
        sprintf(hash_entier+i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }

    //printf("Hash:  %s\n", hash_entier);
    for(x = 0; x < 8; x++) {
        strncpy(hash_separe, hash_entier+(x*8), 8);
        hash_separe[8] = '\0';
        unsigned int number = (unsigned int)strtoul(hash_separe, NULL, 16);
        int leftfrommax = UINT_MAX - number;
        if(indexreduc > leftfrommax) {
            indexreduc -= leftfrommax;
            number = 0;
        }
        number += indexreduc;
        int number_to_pick = number%62;
        new_password[x] = alphabet[number_to_pick];
    }
    new_password[9] = '\0';
    //printf("PASS: %s \n",new_password);
    //printf("-------------------------\n");


}


void createFile(char var [][LENGTH_STRING])
{
    FILE* file = NULL;
    file = fopen(FILEPATH, "a+"); // changer par a+ par la suite
    if (file != NULL)
    {
        for(long x = 0; x<NB_PASS; x++)
        {
            fprintf(file, "%s\n",var[x]); //Ecriture dans le fichier
        }

        fclose(file);
    }
    else
    {
        printf("[*] Error, impossible to open the file \n");
    }
}


void readfile(void)
{
    FILE* file = NULL;
    char chaine[LENGTH_MAX] = "";
    file = fopen(FILEPATH, "r");

    if (file != NULL)
    {
        while (fgets(chaine, LENGTH_MAX, file) != NULL)
        {
            printf("[*] %s", chaine);
        }
        fclose(file);
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    unsigned char hash[33];
    char tableau [NB_PASS][LENGTH_STRING];
    char new_password[9];
    char tail_hash[65];

    for(int y=0; y<5; y++)
    {
        for(long x = 0; x<NB_PASS ; x++)
        {
            char* password = randomString(8);
            for(int j=0; j<100; j++)
            {
                passwordHashing(password,hash);
                reduce_hash(hash,new_password,j);
            }
            hash2string(hash, 32, tail_hash);
            snprintf(tableau[x], LENGTH_STRING, "%s:%s",password,tail_hash);
            printf("%s\n",tableau[x]);
        }
        createFile(tableau);
    }

    // readfile();


    printf("DONE");
    system("pause");

    return 0;
}