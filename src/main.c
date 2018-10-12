#include "rainbow_table.h"

/* alphanumeric: [a-z0-9] */
const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";

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
char *randomString(int length) {
	//alloue la taille de randstr de longeur length
	char *randstr = malloc((length + 1));
	int i;

	for (i = 0; i < length; i++) {
		randstr[i] = alphabet[intN(strlen(alphabet))];
	}
	randstr[length] = '\0';
	//printf("\nrandstr:%d\n\n",strlen(randstr));
	return randstr;
}

int main()
{
	//fait en sorte que la fonction "rand" genère un nouveau nombre random en fonction du temps
	int i, j; 
	LinkedList list = NULL;
	char* password;
	
	char* test = randomString(PASSWORD_LENGTH);
	add(&list, test);
	printf("password à trouver : %s\n", test);
	
	srand(time(NULL));

	for (i = 0; i < 5; i++) {
		password = randomString(PASSWORD_LENGTH);
		add(&list, password);
//		printf("%s\n", password);		
	}
	
	printf("test : %s\n", getPassword(&list, test));

	return 0;
}



