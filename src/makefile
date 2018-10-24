main : main.o linkedList.o sha256.o
	cc -pthread -o main main.o linkedList.o sha256.o
main.o : main.c rainbow_table.h
	cc -c -lpthread main.c
linkedList.o : linkedList.c rainbow_table.h
	cc -c linkedList.c
sha256.o: ../lib/sha256.c ../lib/sha256.h
	cc -c ../lib/sha256.c
