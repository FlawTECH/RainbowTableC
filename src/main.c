#include "./rainbow_table.h"

/* alphanumeric: [a-z0-9] */
const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";

/**
 * return un entier [0, n]. 
 * n : la longeur de alphabet.e
 */
int intN(int n) {
    return rand() % n;
}

/**
 * Input: longeur du string random [a-z0-9] à générer
 */
char* randomString(int length) {
    //alloue la taille de randstr de longeur length
    char *randstr = malloc((length + 1));
    int i;
    
    for (i = 0; i < length; i++) {
        randstr[i] = alphabet[intN(strlen(alphabet))];
    }

    randstr[length] = '\0';
    return randstr;
}

void passwordHashing(char* password, unsigned char* hash) {
    SHA256_CTX ctx;
	sha256_init(&ctx);
    sha256_update(&ctx, password, sizeof password);
 	sha256_final(&ctx, hash);
}

void printHex(unsigned char* data) {
    for (int i = 0; i < 32; i++) { 
    	printf("%02x",data[i]); 
    }
}

char* reduce_hash(unsigned char* hash, char* new_password, int indexReduc) {
    char hash_entier[65];
    int i, number_to_pick;
    char * alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* hash_separe = malloc(8);
    unsigned int number;
    int leftFromMax;

    for (i = 0; i < 32; i++){
        sprintf(hash_entier + i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }
   
    for (i = 0; i < 8; i++) {
        strncpy(hash_separe, hash_entier + (i * 8), 8);
        hash_separe[8] = '\0';
        number = (unsigned int)strtoul(hash_separe, NULL, 16);
        leftFromMax = UINT_MAX - number;
        
        if (indexReduc > leftFromMax) {
            indexReduc -= leftFromMax;
            number = 0;
        }

        number += indexReduc;
        number_to_pick = number % 62;
        new_password[i] = alphabet[number_to_pick];    
    }

    new_password[9] = '\0';
    return new_password;
}

void writeFile(char* password) {
    long x;
    FILE* file = NULL;
    
    file = fopen(FILEPATH, "a+"); // changer par a+ par la suite

    if (file != NULL) {     
        fprintf(file, "%s", password);     
        fclose(file);
    } else {
        perror("[*] Error, impossible to open the file \n");
    }     
}

void hash2string(unsigned char* hash, int length, char* string) {
    for(int i = 0; i<length; i++) {
        sprintf(string + i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }
}

void readFile(void) {
    FILE* file = NULL;
    char chaine[LENGTH_MAX] = "";    
    file = fopen(FILEPATH, "r");

    if (file != NULL) {
         while (fgets(chaine, LENGTH_MAX, file) != NULL) {
                printf("[*] %s", chaine);
        }
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
	int i, j;
    LinkedList list = NULL;
    char* password;
    char new_password[9];
    unsigned char hash[33];
    char finalHash[65];

    srand(time(0));

    for (i = 0; i < NB_PASS; i++) {
        list = NULL;
        password = randomString(PASSWORD_LENGTH);
        
        add(&list, strcat(password, ":"));
        writeFile(password);
        
        for (j = 0; j < 50000; j++) {
            if (j % 2 == 0) {
                passwordHashing(password, hash);
                add(&list, hash);
            } else {
                password = reduce_hash(hash, new_password, i);
                add(&list, password);
            }
        }

        hash2string(hash, LENGTH_STRING, finalHash);
        writeFile(strcat(finalHash, "\n"));
    }

    printf("Rainbow Table successfuly created ! Enjoy ...");
    
}
