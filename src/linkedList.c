#include "./rainbow_table.h"

// adds a node to the list, if the list is empty it creates it

void add(LinkedList* list, char* value) {
	Node* node;
	if ((node = (Node*)malloc(sizeof(Node))) == NULL) {
		perror("malloc add");
		exit(1);
	}

	strcpy(node->value, value);
	node->next = NULL;
	
	if (isEmpty(*list)) {
		*list = node;
	} else {
		LinkedList walker = *list;
		
		while (walker->next != NULL) {
			walker = walker->next;
		}

		walker->next = node;
	}
}

// checks if the list is empty

int isEmpty(LinkedList list) {
	return list == NULL;
}

// runs through the list to find a match with the password

char* getPassword(LinkedList* list, char* hash) {
	Node* walker = *list;
	
	int result;
	
	while (walker->next != NULL && (result = strcmp(walker->next->value, hash)) != 0) {
		walker = walker->next;
	}

	if (result == 0) {
		return walker->value; 
	}

	return NULL;
}
