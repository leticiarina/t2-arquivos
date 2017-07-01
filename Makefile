all: auxFunctions.o file.o main.o
	gcc -g auxFunctions.o file.o main.o -o t2

auxFunctions.o:
	gcc -g -c auxFunctions.c

file.o:
	gcc -g -c file.c

main.o:
	gcc -g -c main.c

clean:
	rm *.o t2

run:
	./t2

vl:
	valgrind --leak-check=full ./t2