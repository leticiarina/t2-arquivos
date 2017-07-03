# Donizeti Carlos dos Santos Junior		NUSP: 9393882
# Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
# José Augusto Noronha de Menezes Neto	NUSP: 9293049
# Letícia Rina Sakurai					NUSP: 9278010

all: auxFunctions.o file.o indexVector.o indexFile.o main.o
	gcc -g -Wall auxFunctions.o file.o indexVector.o indexFile.o main.o -o t2

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
	valgrind --leak-check=full ./t2