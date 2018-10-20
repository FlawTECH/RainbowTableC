#include "./rainbow_table.h"

/* alphanumeric: [a-z0-9] */
const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

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
void randomString(char* output, int length) {
    int i;
    
    for (i = 0; i < length; i++) {
        output[i] = alphabet[intN(strlen(alphabet))];
    }

    output[length] = '\0';
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

void reduce_hash(unsigned char* hash, char* new_password, int indexReduc) {
    char hash_entier[65];
    int i, number_to_pick;
    char hash_separe[9];
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

    new_password[8] = '\0';
}

void writeFile(LinkedList* baseList) {
    FILE* file;
    file = fopen(FILEPATH, "a+");
    LinkedList walker = *baseList;
    LinkedList lastValue;

    fprintf(file, "%s\n", walker->value);

    //Walking through linked list
    while(walker->next != NULL) {
        lastValue = walker;
        walker = walker->next;
        //Freeing linked list members after write
        free(lastValue);
        lastValue = NULL;
        fprintf(file, "%s\n", walker->value);
    }

    //Freeing tail and head of linked list
    free(walker);
    walker = NULL;
    *baseList = NULL;
    free(*baseList);

    fclose(file);  
}

void hash2string(unsigned char* hash, int length, char* string) {
    for(int i = 0; i<length; i++) {
        sprintf(string + i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }
}

void readFile(LinkedList* list) {
    FILE* file = NULL;
    char chaine[LENGTH_MAX] = "";    
    file = fopen(FILEPATH, "r");

    if (file != NULL) {
         while (fgets(chaine, LENGTH_MAX, file) != NULL) {

        }
        fclose(file);
    }
}

void generate_table(char* fileName) {
    int i, j, k;
    LinkedList list = NULL;
    char password[9];
    char new_password[9];
    unsigned char hash[33];
    char finalHash[65];
    char fullChain[74];

    srand(time(0));
    for(k = 0; k<5; k++) {
        for (i = 0; i < WRITE_BUFFER; i++) {
            randomString(password, PASSWORD_LENGTH);
            for (j = 0; j < 50000; j++) {
                passwordHashing(new_password, hash);
                reduce_hash(hash, new_password, j);
            }
            hash2string(hash, LENGTH_HASH/2, finalHash);
            snprintf(fullChain, 74, "%s:%s", password, finalHash);
            add(&list, fullChain);
        }
        writeFile(&list);
    }

    printf("Rainbow Table successfuly created ! Enjoy ...");
}

void crack_hash(char* fileName, char* hashToCrack) {
    
}

void sort_alphabetically(LinkedList* list) {
    
}

int main(int argc, char *argv[]) {
	#pragma region Declarations

    char* fileName;
    char hashToCrack[LENGTH_HASH+1];
    int opt;
    enum {GENERATE_MODE, CRACK_MODE} mode = GENERATE_MODE;

    #pragma endregion

    #pragma region Argument Parsing
    /**
     * Argument Parsing
     */

    //If no arguments
    if(argc == 1) {
        fprintf(stderr, "Usage: %s [-gc] [-f TABLEFILENAME]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Checking all arguments
    while((opt = getopt(argc, argv, "gc:f:")) != -1) {
        switch(opt) {
            case 'g':
                mode = GENERATE_MODE;
                break;
            case 'c':
                mode = CRACK_MODE;
                if(strlen(optarg) == LENGTH_HASH) {
                    strcpy(hashToCrack, optarg);
                }
                else {
                    fprintf(stderr, "Invalid SHA256 Hash.");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                fileName = malloc(strlen(optarg));
                strcpy(fileName, optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-gc] [-f TABLEFILENAME]\n", argv[0]);
                exit(EXIT_FAILURE); 
        }
    }

    /**
     * End Argument Parsing
     */
    #pragma endregion

    #pragma region Mode selection
    
    switch(mode) {
        case GENERATE_MODE:
            generate_table(fileName);           break;
        case CRACK_MODE:
            crack_hash(fileName, hashToCrack);  break;
        default:
            fprintf(stderr, "The program encountered an error and must close. Please try again.");
            exit(EXIT_FAILURE);
    }
    #pragma endregion
    
    free(fileName);
    return EXIT_SUCCESS;
}