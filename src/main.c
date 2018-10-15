#include <stdio.h>
#include <stdlib.h>
#include "../lib/sha256.h"
#define LENGTH_MAX 10000
#define LENGTH_STRING 12
#define FILEPATH "bin/password.txt"


void createFile(char var[LENGTH_STRING])
    {
        FILE* file = NULL;
        file = fopen(FILEPATH, "a+");
        if (file != NULL)
            {
            printf("[*] Success : File open \n");           
            fprintf(file, "%s \n", var);
            fclose(file);
            }
        else
            {
            printf("[*] Error, impossible to open the file \n");
            }       
        return 0;
    }


void readfile(void)
    {
    FILE* file = NULL;
    char chaine[LENGTH_MAX] = "";    
    file = fopen(FILEPATH, "r");

    if (file != NULL)
    {
         while (fgets(chaine, LENGTH_MAX, file) != NULL)
            {
                printf("[*] %s", chaine);
            }
            fclose(file);
        }
    }

int main(int argc, char *argv[])
    {
        char stringToWrite [LENGTH_STRING] = "Something";
        createFile(stringToWrite);
        readfile();
        system("pause");

        return 0;
    }