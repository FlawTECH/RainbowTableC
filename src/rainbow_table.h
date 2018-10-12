#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define PASSWORD_LENGTH 8

typedef struct Node {
	char* value;
	struct Node* next;
} Node;

typedef Node* LinkedList;

int add(LinkedList* list, char* value);

int isEmpty(LinkedList list);

char* getPassword(LinkedList* list, char* password);
