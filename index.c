/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdio.h>
#include "file.h"
#include "index.h"

// Função writeIndexFiles: adiciona um registro no arquivo de índice.
int writeIndexFiles(int ticket, int byteOffset, FILE *indexFile){

	int compareTicket, byte, insertByte = 0;
	char end;

	fseek(indexFile, 0, SEEK_SET);

	if(end = getc(indexFile) != EOF){

	// Procura a posição em que o registro deve ser inserido
		do {
			insertByte = ftell(indexFile);
			fscanf(indexFile, "%d", &compareTicket);
			fscanf(indexFile, "%d", &byte);
		} while(compareTicket < ticket && (end = getc(indexFile)) != EOF);
	
	}
	
	// Escreve no registro
	fprintf(indexFile, "%d", ticket);
	fprintf(indexFile, "%d",  byteOffset);

	return TRUE;

}

// Procura o byte em que um arquivo deve ser inserido.
int searchIndex(){



}