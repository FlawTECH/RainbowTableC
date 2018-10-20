#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../lib/sha256.h"

#define TRUE 1
#define FALSE 0
#define PASSWORD_LENGTH 8
#define LENGTH_HASH 64
#define FILE_BUFFER 5

typedef struct Node {
	char value[LENGTH_HASH+PASSWORD_LENGTH+2];
	struct Node* next;
} Node;

typedef Node* LinkedList;

void add(LinkedList* list, char* value);

int isEmpty(LinkedList list);

char* getPassword(LinkedList* list, char* password);
