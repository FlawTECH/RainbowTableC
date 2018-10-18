#include <stdio.h>
#include <stdlib.h>
#include "../lib/sha256.h"
#define LENGTH_MAX 10000
#define LENGTH_STRING 1000
#define NB_PASS 200000
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

void PrintHex(unsigned char * data) 
{
	
    char tmp[16];
    char hashFinal[32];
    for (int i=0; i<32; i++) { 
		
    	printf("%02x",data[i]); 
   	 	
    }
	
}

unsigned char * reduce_hash(unsigned char hash[])
{
    for(int x =0; x<64;x++)
    {
        if(hash[x]=='9')
        {
            hash[x]='a';
        }
        else{
            hash[x]=hash[x]+1;
        }
        
    }
    
    return hash;
}


void createFile(char var [][10])
    {
        FILE* file = NULL;
        file = fopen(FILEPATH, "w+"); // changer par a+ par la suite
        if (file != NULL)
            {     
                for(long x = 0;x<NB_PASS;x++)
                {
                    fprintf(file, "%s%s \n",var[x]); //ECriture dans le fichier
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

	unsigned char hash[32];
	SHA256_CTX ctx;
	sha256_init(&ctx);
    char tableau [NB_PASS][10];
    
    for(int y=0;y<5;y++)
    {
            for(long x = 0; x<NB_PASS ;x++)
        {
            char* password = randomString(8);
            strcpy(tableau[x], password); 
        }
        createFile(tableau);
    }
    
    //readfile();
	
    char* password = randomString(8);

	printf("password a trouver : %s\n", password);
	sha256_update(&ctx,password,sizeof password);
 	sha256_final(&ctx,hash);
	printf("hash du password: ");
    PrintHex(hash);
	printf("\n");
    unsigned char test[65]="a6e6b4068d8318dfa8c31f9173cf764dc10f8ec7a168b62372044c247fa3c689\0";
    unsigned char test2[65];
    strcpy(test2,reduce_hash(test));
    printf("%s",test2);
    system("pause");

        return 0;
    }