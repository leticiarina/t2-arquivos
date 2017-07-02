all: auxFunctions.o file.o indexVector.o indexFile.o main.o
	gcc -g auxFunctions.o file.o indexVector.o indexFile.o main.o -o t2

auxFunctions.o:
	gcc -g -c auxFunctions.c

file.o:
	gcc -g -c file.c

indexVector.o:
	gcc -g -c indexVector.c

indexFile.o:
	gcc -g -c indexFile.c

main.o:
	gcc -g -c main.c

clean:
	rm *.o t2

run:
	./t2

vl:
	valgrind ./t2