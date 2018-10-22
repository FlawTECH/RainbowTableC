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
#define FILE_BUFFER 10

typedef struct Node {
	char 	value[(PASSWORD_LENGTH*2)+2];
	struct 	Node* next;
} Node;

typedef struct MultiNode {
	char 	head[PASSWORD_LENGTH+1];
	char 	tail[PASSWORD_LENGTH+1];
	struct 	MultiNode* next;
} MultiNode;

typedef Node* 		LinkedList;
typedef MultiNode* 	MultiLinkedList;

void add(LinkedList*, char*);
int isEmptyList(LinkedList);
int isEmptyMultiList(MultiLinkedList);
char* getPassword(LinkedList*, char*);
