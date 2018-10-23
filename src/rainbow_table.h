#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../lib/sha256.h"
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define PASSWORD_LENGTH 8
#define LENGTH_HASH 64
<<<<<<< HEAD
=======
#define FILE_BUFFER 1
>>>>>>> refs/remotes/origin/master

typedef struct Node {
	char 	value[(PASSWORD_LENGTH*2)+2];
	struct 	Node* next;
} Node;

typedef struct MultiNode {
	char 	head[PASSWORD_LENGTH+1];
	char 	tail[PASSWORD_LENGTH+1];
	struct 	MultiNode* next;
} MultiNode;

<<<<<<< HEAD
typedef struct crack_hash_args {
	char*	fileName;
	char	hashToCrack[LENGTH_HASH+1];
} CrackHashArgs;

typedef struct generate_args {
	char* fileName;
	int user_number;
	int number;
	int thread_count;
} GenerateArgs;

=======
>>>>>>> refs/remotes/origin/master
typedef Node* 		LinkedList;
typedef MultiNode* 	MultiLinkedList;

void add(LinkedList*, char*);
int isEmptyList(LinkedList);
int isEmptyMultiList(MultiLinkedList);
char* getPassword(LinkedList*, char*);
