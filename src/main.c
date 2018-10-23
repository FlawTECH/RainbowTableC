#include "./rainbow_table.h"

/* alphanumeric: [a-z0-9] */
const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

//Shared resources between cracking threads
int global_reduction_index = 0;
MultiLinkedList chains = NULL;
int chainCount = 0;

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

void* generate_table(void* fileName) {
    int i, j, k;
    LinkedList list = NULL;
    char head[PASSWORD_LENGTH+1];
    char tail[PASSWORD_LENGTH+1];
    unsigned char hash[(LENGTH_HASH/2)+1];
    char fullChain[(PASSWORD_LENGTH*2)+2];

    srand(time(0));
    
        for (i = 0; i < FILE_BUFFER; i++) {
            printf("Thread n %d: %d%% of pass generated\n",pthread_self(),((i+1)*100)/FILE_BUFFER);
            randomString(head, PASSWORD_LENGTH);
            strcpy(tail, head);
            for (j = 0; j < 50000; j++) {
                passwordHashing(tail, hash);
                reduce_hash(hash, tail, FALSE, j);
                if(j==49995) {
                    printf("\n: %s", tail);
                }
            }
            snprintf(fullChain, (PASSWORD_LENGTH*2)+2, "%s:%s", head, tail);
            add(&list, fullChain);
        }
        writeFile(fileName, &list);
}

void* crack_hash(void* hashToCrack) {
    int i, j, reducIndex;
    int found = FALSE;
    int startFlag = TRUE;
    int isLongHash = TRUE;
    int progress;

   
    char tempHash[LENGTH_HASH+1];
    char tempPassword[PASSWORD_LENGTH+1];
    MultiLinkedList walker;

    char tmp[LENGTH_HASH+1] = "4eb0305b91e1d04f93edd5c7b67489f9a513e2b7634fd79c02b6baf6ab8756e2";
    // pthread_mutex_lock(&mutex);
    char* startHash;
    startHash = malloc(sizeof(LENGTH_HASH+1));
    strcpy(startHash, tmp);
    // pthread_mutex_unlock(&mutex);

    // char startHash[LENGTH_HASH+1] = "4eb0305b91e1d04f93edd5c7b67489f9a513e2b7634fd79c02b6baf6ab8756e2";
    printf("YES");


    printf("\n%s", startHash);

    for(i=0; i<50000; i++) {
        pthread_mutex_lock(&mutex);
        i = global_reduction_index;
        global_reduction_index++;
        pthread_mutex_unlock(&mutex);
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
                pthread_mutex_unlock(&mutex);
                startFlag = FALSE;
            } else {
                walker = walker->next;
            } if (strcmp(tempPassword, walker->tail) == 0) {
                found = TRUE;
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

    if (found) {
        printf("Hash found ! Computing plaintext ...\n");
        strcpy(tempPassword, walker->head);

        for (i = 0; i < reducIndex; i++) {
            passwordHashing(tempPassword, tempHash);
            reduce_hash(tempHash, tempPassword, FALSE, i);
        }

        printf("Password found for %64s. \n==> %8s\n", startHash, tempPassword);
    } else {
        printf("Password not found, generate more hashes.");
    }
    startHash = NULL;
    free(startHash);
    system("pause");
}

void sort_alphabetically(LinkedList* list) {
    
}

int main(int argc, char *argv[]) {
	#pragma region Declarations

    char* fileName;
    char hashToCrack[LENGTH_HASH+1];
    int opt, i, user_number, thread_number = 4;
    enum {GENERATE_MODE, CRACK_MODE} mode = GENERATE_MODE;
    pthread_t threads[thread_number];
    clock_t begin, end;

    #pragma endregion

    #pragma region Argument Parsing
    /**
     * Argument Parsing
     */

    //If no arguments
    if(argc == 1) {
        fprintf(stderr, "Usage: %s [-gc] [-f TABLEFILENAME] [-n NB_PASSTOGENERATE] [-t THREADNUMBER]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Checking all arguments
    while((opt = getopt(argc, argv, "gc:f:n:t:")) != -1) {
        switch(opt) {
            case 'g':
                printf("Generate mode selected.\n");
                mode = GENERATE_MODE;
                break;
            case 'c':
                mode = CRACK_MODE;
                if(strlen(optarg) == LENGTH_HASH) {
                    strcpy(hashToCrack, optarg);
                }
                else {
                    fprintf(stderr, "Invalid SHA256 Hash.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'f':
                fileName = malloc(strlen(optarg));
                strcpy(fileName, optarg);
                break;
            case 'n':
                user_number = strtol(optarg, NULL, 10);
                if(user_number<=0){
                    printf("Warning: Wrong number of passwords to generate.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                thread_number = strtol(optarg, NULL, 10);
                if(thread_number>8){
                    printf("Warning: 8 Threads maximum.\n");
                    exit(EXIT_FAILURE);
                }else if(thread_number<0){
                    printf("Warning: Wrong number of threads.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-gc] [-f TABLEFILENAME] [-n NB_PASSTOGENERATE] [-t THREADNUMBER]\n", argv[0]);
                exit(EXIT_FAILURE); 
        }
    }
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
            printf("Number of threads: %d\n",thread_number);
            for (i = 0; i < thread_number; i++) {
                pthread_create(&threads[i], NULL, generate_table, (void*)fileName);
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
                pthread_create(&threads[i], NULL, crack_hash, NULL);
            }
            
            break;
        default:
            fprintf(stderr, "The program encountered an error and must close. Please try again.");
            exit(EXIT_FAILURE);
    }
    #pragma endregion
    
    #pragma region Destructors

    for (i = 0; i < thread_number; i++) {
        pthread_join(threads[i], NULL);
    }

    end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Rainbow Table successfuly created in %f ! %d Threads used for %d pass generated! \n", time_spent, thread_number, FILE_BUFFER*thread_number);
    system("pause");

    fileName = NULL;
    free(fileName);

    #pragma endregion

    return EXIT_SUCCESS;
}