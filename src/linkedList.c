#include "rainbow_table.h"

// adds a node to the list, if the list is empty it creates it

int add(LinkedList* list, char* value) {
	Node* node;

	if ((node = (Node*)malloc(sizeof(Node))) == NULL) {
		perror("malloc add");
		exit(1);
	}

	node->value = value;

	if (isEmpty(*list)) {
		*list = node;
	} else {
		Node* walker = *list;

		while (walker->next != NULL) {
			walker = walker->next;
		}

		walker->next = node;
	}

	return TRUE;
}

// checks if the list is empty

int isEmpty(LinkedList list) {
	return list == NULL;
}

// runs through the list to find a match with the password

char* getPassword(LinkedList* list, char* password) {
	Node* walker = *list;
	
	int result;
	
	while (walker->next != NULL && (result = strcmp(walker->next->value, password)) != 0) {
		walker = walker->next;
	}

	if (result == 0) {
		return walker->value; 
	}

	return NULL;
}
