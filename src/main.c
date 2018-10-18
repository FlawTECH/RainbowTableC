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
    return randstr;
}

void PrintHex(unsigned char * data) 
{
	
    
    for (int i=0; i<32; i++) { 
		
    	printf("%02x",data[i]); 
   	 	
    }
	
}

void passwordHashing(char* password, unsigned char* hash)
{
    SHA256_CTX ctx;
	sha256_init(&ctx);

    sha256_update(&ctx,password,sizeof password);
 	sha256_final(&ctx,hash);
    
}

int main()
{
    //fait en sorte que la fonction "rand" genère un nouveau nombre random en fonction du temps
    srand(time(NULL));
	
	

	char* password = randomString(8);
    unsigned char hash[65];


	printf("password a trouver : %s\n", password);
    printf("hash du password: ");
    passwordHashing(password,hash);
	PrintHex(hash);
	printf("\n");
    system("pause");
    return 0;
}


