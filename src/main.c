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
    char hash_entier[LENGTH_HASH+1];
    int i, number_to_pick;
    char hash_separe[PASSWORD_LENGTH+1];
    unsigned int number;
    int leftFromMax;

    for (i = 0; i < LENGTH_HASH/2; i++){
        sprintf(hash_entier + i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }
   
    for (i = 0; i < PASSWORD_LENGTH; i++) {
        strncpy(hash_separe, hash_entier + (i * 8), 8);
        hash_separe[PASSWORD_LENGTH] = '\0';
        number = (unsigned int)strtoul(hash_separe, NULL, 16);
        leftFromMax = UINT_MAX - number;
        
        if (indexReduc > leftFromMax) {
            indexReduc -= leftFromMax;
            number = 0;
        }

        number += indexReduc;
        number_to_pick = number % strlen(alphabet);
        new_password[i] = alphabet[number_to_pick];    
    }

    new_password[PASSWORD_LENGTH] = '\0';
}

void writeFile(char* fileName, LinkedList* baseList) {
    FILE* file;
    file = fopen(fileName, "a+");
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
    walker = NULL;
    free(walker);
    *baseList = NULL;
    free(*baseList);

    fclose(file);  
}

void hash2string(unsigned char* hash, int length, char* string) {
    for(int i = 0; i<length; i++) {
        sprintf(string + i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
    }
}

/**
 * Separates hashes from passwords in a chain and stores them in two strings
 */
void split_chain(char* chain, char* password, char* hash) {
    int i;

    if(password!=NULL) {
        for(i=0; i<PASSWORD_LENGTH; i++) {
            password[i] = chain[i];
        }
        password[PASSWORD_LENGTH] = '\0';
    }

    if(hash!=NULL) {
        for(i=PASSWORD_LENGTH+1; i<LENGTH_HASH+PASSWORD_LENGTH+1; i++) {
            hash[i-PASSWORD_LENGTH-1] = chain[i];
        }
        hash[LENGTH_HASH] = '\0';
    }
}

int readFile(char* fileName, MultiLinkedList* list) {
    FILE* file = NULL;
    char chaine[LENGTH_HASH+PASSWORD_LENGTH+3];
    file = fopen(fileName, "r");
    MultiLinkedList walker;
    int counter = 0;

    if (file != NULL) {
        while (fgets(chaine, LENGTH_HASH+PASSWORD_LENGTH+3, file) != NULL) { // + 3 => ":","\0" & "\n"
            counter++;
            if(isEmptyMultiList(*list)) {
                *list = malloc(sizeof(Node));
                walker = *list;               
            }
            else {
                walker->next = malloc(sizeof(Node));
                walker = walker->next;
            }
            split_chain(chaine, walker->password, walker->hash);
        }
        fclose(file);
    }

    return counter;
}

void generate_table(char* fileName) {
    int i, j, k;
    LinkedList list = NULL;
    char password[PASSWORD_LENGTH+1];
    char new_password[PASSWORD_LENGTH+1];
    unsigned char hash[(LENGTH_HASH/2)+1];
    char finalHash[LENGTH_HASH+1];
    char fullChain[LENGTH_HASH+PASSWORD_LENGTH+2];

    srand(time(0));
    for(k = 0; k<5; k++) {
        for (i = 0; i < FILE_BUFFER; i++) {
            randomString(password, PASSWORD_LENGTH);
            strcpy(new_password, password);
            for (j = 0; j < 50000; j++) {
                if(j>0) {
                    reduce_hash(hash, new_password, j);
                }
                passwordHashing(new_password, hash);
            }
            hash2string(hash, LENGTH_HASH/2, finalHash);
            snprintf(fullChain, LENGTH_HASH+PASSWORD_LENGTH+2, "%s:%s", password, finalHash);
            add(&list, fullChain);
        }
        writeFile(fileName, &list);
    }
    printf("Rainbow Table successfuly created ! Enjoy ...");
}

void crack_hash(char* fileName, char* hashToCrack) {
    int chainCount;
    int i;
    int found = FALSE;
    char tempHash[LENGTH_HASH+1];
    MultiLinkedList chains = NULL;
    MultiLinkedList walker;

    for(i = 0; i<LENGTH_HASH; i++) {
        hashToCrack[i] = tolower(hashToCrack[i]);
    }

    //Reading chains from file
    chainCount = readFile(fileName, &chains);
    printf("%d hashes loaded. Cracking ...\n", chainCount);

    do {
        if(isEmptyMultiList(walker)) {
            walker = chains;
        }
        else {
            walker = walker->next;
        }
        if(strcmp(hashToCrack, walker->hash) == 0) {
            found = TRUE;
            break;
        }
    }
    while(walker->next != NULL);

    if(found) {
        printf("Hash found ! Computing plaintext ...\n");
        for(i = 0; i<50000; i++) {
            if(i>0) {
                reduce_hash(tempHash, walker->password, i);
            }
            passwordHashing(walker->password, tempHash);
        }
        printf("Password found for %64s. \n==> %8s\n", hashToCrack, walker->password);
    }
    else {
        printf("Password not found, generate more hashes.");
    }
    system("pause");
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
    
    #pragma region Destructors

    fileName = NULL;
    free(fileName);

    #pragma endregion

    return EXIT_SUCCESS;
}