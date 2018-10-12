main : main.o linkedList.o
	cc -o main main.o linkedList.o
main.o : main.c rainbow_table.h
	cc -c main.c
linkedList.o : linkedList.c rainbow_table.h
	cc -c linkedList.c
