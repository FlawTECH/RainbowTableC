#include "./rainbow_table.h"

/* alphanumeric: [a-z0-9] */
const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

//Shared resources between cracking threads
int global_reduction_index = 0;
MultiLinkedList chains = NULL;
int chainCount = 0;
int global_hash_iscracked = FALSE;

//Mutex for shared resources
pthread_mutex_t mutex;

/**
 * return un entier [0, n]. 
 * n : la longeur de alphabet.e
 */
int intN(int n) {
    return (rand() + pthread_self()) % n;
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

void reduce_hash(unsigned char* hash, char* reduced_hash, int isLongHash, int indexReduc) {
    char hash_entier[LENGTH_HASH+1];
    int i, number_to_pick;
    char hash_separe[PASSWORD_LENGTH+1];
    unsigned int number;
    int leftFromMax;

    if(!isLongHash) {
        for (i = 0; i < LENGTH_HASH/2; i++){
            sprintf(hash_entier + i*2, "%02x", hash[i]); //Créer le hash avec l'argument "hash" que recoit la fonction
        }
    } else {
        strcpy(hash_entier, hash);
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
        reduced_hash[i] = alphabet[number_to_pick];    
    }

    reduced_hash[PASSWORD_LENGTH] = '\0';
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

    if (hash != NULL) {
        for(i=PASSWORD_LENGTH+1; i<PASSWORD_LENGTH+PASSWORD_LENGTH+1; i++) {
            hash[i-PASSWORD_LENGTH-1] = chain[i];
        }
        hash[PASSWORD_LENGTH] = '\0';
    }
}

int readFile(char* fileName, MultiLinkedList* list) {
    FILE* file = NULL;
    char chaine[(PASSWORD_LENGTH * 2) + 3];
    file = fopen(fileName, "r");
    MultiLinkedList walker;
    int counter = 0;

    if (file != NULL) {
        while (fgets(chaine, (PASSWORD_LENGTH*2)+3, file) != NULL) { // + 3 => ":","\0" & "\n"
            counter++;

            if(isEmptyMultiList(*list)) {
                *list = malloc(sizeof(Node));
                walker = *list;               
            } else {
                walker->next = malloc(sizeof(Node));
                walker = walker->next;
            }

            split_chain(chaine, walker->head, walker->tail);
        }
        
        fclose(file);
    }
    if(counter>0) {
        walker->next = NULL;
    }
    return counter;
}

void* generate_table(void* arguments) {

    GenerateArgs *args = (GenerateArgs*)arguments;

    int i, j, k;
    LinkedList list = NULL;
    char head[PASSWORD_LENGTH+1];
    char tail[PASSWORD_LENGTH+1];
    unsigned char hash[(LENGTH_HASH/2)+1];
    char fullChain[(PASSWORD_LENGTH*2)+2];

    srand(time(0));
    
        for (i = 0; i < args->rainbow_size; i++) {
            args->generated_count++;
            if((args->generated_count%(args->rainbow_size*8/10)) == 0)
            {
                printf("[*] Pass generated:  %d/%d -> %d%%\n",args->generated_count,args->rainbow_size*args->thread_count,((args->generated_count)*100)/(args->rainbow_size*args->thread_count));
            }     
            randomString(head, PASSWORD_LENGTH);
            strcpy(tail, head);
            for (j = 0; j < 50000; j++) {
                passwordHashing(tail, hash);
                reduce_hash(hash, tail, FALSE, j);
                if(j==48000) {
                    printf("\n%s", tail);
                }
            }
            snprintf(fullChain, (PASSWORD_LENGTH*2)+2, "%s:%s", head, tail);
            add(&list, fullChain);
        }
        writeFile(args->fileName, &list);
}

void* crack_hash(void* hashToCrack) {

    int i, j, reducIndex;
    int startFlag = TRUE;
    int isLongHash = TRUE;
    int waitToKill = FALSE;
    int progress;

    char* startHash;   
    char tempHash[LENGTH_HASH+1];
    char tempPassword[PASSWORD_LENGTH+1];
    MultiLinkedList walker;
    
    startHash = malloc(LENGTH_HASH+1);
    pthread_mutex_lock(&mutex);
    strcpy(startHash, hashToCrack);
    pthread_mutex_unlock(&mutex);

    for(i=0; i<50000; i++) {
        pthread_mutex_lock(&mutex);
        waitToKill = global_hash_iscracked;
        i = global_reduction_index;
        global_reduction_index++;
        pthread_mutex_unlock(&mutex);
        if(waitToKill) {
            pthread_exit(NULL);
        }
        strcpy(tempHash, startHash);
        isLongHash = TRUE;
        
        for(j=49999-i; j<50000; j++) {
            reduce_hash(tempHash, tempPassword, isLongHash, j);
            passwordHashing(tempPassword, tempHash);
            isLongHash = FALSE;
        }
        
        startFlag = TRUE;

        do {
            if(startFlag) {
                pthread_mutex_lock(&mutex);
                walker = chains;
                waitToKill = global_hash_iscracked;
                pthread_mutex_unlock(&mutex);
                if(waitToKill) {
                    pthread_exit(NULL);
                }
                startFlag = FALSE;
            } else {
                walker = walker->next;
            } if (strcmp(tempPassword, walker->tail) == 0) {
                pthread_mutex_lock(&mutex);
                global_hash_iscracked = TRUE;
                pthread_mutex_unlock(&mutex);
                reducIndex = 49999 - i;
                i = 50000;
                j = 50000;
                break;
            }
        } while(walker->next != NULL);
        if(i%500==0) {
            progress = (i/49999.)*100;
            printf("\nProgress: %d%%", progress);
        }
    }

    pthread_mutex_lock(&mutex);
    if (global_hash_iscracked) {
        printf("Hash found ! Computing plaintext ...\n");
        strcpy(tempPassword, walker->head);

        for (i = 0; i < reducIndex; i++) {
            passwordHashing(tempPassword, tempHash);
            reduce_hash(tempHash, tempPassword, FALSE, i);
        }

        printf("Password found for %64s. \n==> %8s\n", startHash, tempPassword);
        system("pause");
    } else {
        printf("Password not found, generate more hashes.");
    }
    pthread_mutex_unlock(&mutex);
    startHash = NULL;
    free(startHash);
}

void sort_alphabetically(LinkedList* list) {
    
}

int main(int argc, char *argv[]) {
	#pragma region Declarations

    char* fileName;
    char hashToCrack[LENGTH_HASH+1];
    int opt, i, rainbow_size = 1000000, thread_number = 4;
    enum {GENERATE_MODE, CRACK_MODE} mode = GENERATE_MODE;
    clock_t begin, end;

    GenerateArgs* generateArgs;


    #pragma endregion

    #pragma region Argument Parsing
    /**
     * Argument Parsing
     */

    //If no arguments
    if(argc == 1) {
        fprintf(stderr, "Usage: %s [-gc] [-f TABLE_FILENAME] [-n PASS_COUNT_PER_THREAD] [-t THREAD_COUNT]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Checking all arguments
    while((opt = getopt(argc, argv, "gc:f:n:t:")) != -1) {
        switch(opt) {
            case 'g':
                printf("[*] Generate mode selected.\n");
                mode = GENERATE_MODE;
                break;
            case 'c':
                printf("[*] Crack mode selected.\n");
                mode = CRACK_MODE;
                if(strlen(optarg) == LENGTH_HASH) {
                    strcpy(hashToCrack, optarg);
                }
                else {
                    fprintf(stderr, "[*] Invalid SHA256 Hash.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                fileName = malloc(strlen(optarg));
                strcpy(fileName, optarg);
                break;
            case 'n':
                rainbow_size = strtol(optarg, NULL, 10);
                if(rainbow_size<=0){
                    printf("[*] Warning: Wrong number of passwords to generate.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                thread_number = strtol(optarg, NULL, 10);
                if(thread_number>8){
                    printf("[*] Warning: 8 Threads maximum.\n");
                    exit(EXIT_FAILURE);
                }else if(thread_number<0){
                    printf("[*] Warning: Wrong number of threads.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-gc] [-f TABLE_FILENAME] [-n PASS_COUNT_PER_THREAD] [-t THREAD_COUNT]\n", argv[0]);
                exit(EXIT_FAILURE); 
        }
    }

    pthread_t threads[thread_number];

    /**
     * End Argument Parsing
     */
    #pragma endregion

    #pragma region Mode selection

    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    switch(mode) {
        case GENERATE_MODE:
            begin = clock();
            printf("[*] Threads used: %d\n\n",thread_number);
            generateArgs = malloc(sizeof(GenerateArgs));            
            generateArgs->fileName = malloc(sizeof(fileName));            
            strcpy(generateArgs->fileName, fileName);            
            generateArgs->rainbow_size = rainbow_size;
            generateArgs->generated_count = 0;
            generateArgs->thread_count = thread_number;

            for (i = 0; i < thread_number; i++) {
                pthread_create(&threads[i], NULL, generate_table, (void*)generateArgs);
            }

            break;
        case CRACK_MODE:
            //Reading chains from file
            chainCount = readFile(fileName, &chains);
            if(chainCount==0) {
                fprintf(stderr, "Unable to load any hashes. Make sure the file exists and that at least one chain is present.");
                exit(EXIT_FAILURE);
            }
            printf("%d entries loaded. Cracking ...\n", chainCount);
            for (i = 0; i < thread_number; i++) {
                pthread_create(&threads[i], NULL, crack_hash, (void*)hashToCrack);
            }
            
            break;
        default:
            fprintf(stderr, "[*] The program encountered an error and must close. Please try again.");
            exit(EXIT_FAILURE);
    }
    #pragma endregion
    
    #pragma region Destructors

    for (i = 0; i < thread_number; i++) {
        pthread_join(threads[i], NULL);
    }

    end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("\n[*] Rainbow Table successfuly created in %.2f secondes! %d Threads used for %d pass generated. \n\n", time_spent, thread_number, rainbow_size*thread_number);
    system("pause");

    generateArgs = NULL;
    fileName = NULL;
    free(generateArgs);
    free(fileName);

    #pragma endregion

    return EXIT_SUCCESS;
}