#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/sha256.h"

#define TRUE 1
#define FALSE 0
#define PASSWORD_LENGTH 8
#define LENGTH_MAX 10000
#define LENGTH_HASH 64
#define FILEPATH "bin/password.txt"
#define WRITE_BUFFER 20

typedef struct Node {
	char value[74];
	struct Node* next;
} Node;

typedef Node* LinkedList;

void add(LinkedList* list, char* value);

int isEmpty(LinkedList list);

char* getPassword(LinkedList* list, char* password);
