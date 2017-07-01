/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "index.h"

// Função initIndex: inicializa os vetores de registro
INDEX *initIndex(){

	INDEX *index = (INDEX*) malloc (sizeof(INDEX));

	if(index != NULL){
		index->size = 0;
		index->indexReg = (INDEXREG**) malloc(sizeof(INDEXREG*));
	}

	return index;

}

// Função writeIndexFiles: adiciona um registro no arquivo de índice.
int writeIndexFiles(int ticket, int byteOffset, FILE *indexFile, INDEX *index){

	int insertByte;
	INDEXREG *insert = (INDEXREG*) malloc(sizeof(INDEXREG));

	if(insert != NULL){
		insert->ticket = ticket;
		insert->byteOffset = byteOffset;
	}

	// Procura o byte em que o registro deve ser inserido no arquivo de índice
	insertByte = searchIndex(index, insert);

	return TRUE;

}

// Procura o byte em que um arquivo deve ser inserido.
int searchIndex(INDEX *index, INDEXREG *insert){

	// Nenhum registro foi inserido ainda
	if(index->size == 0){
		index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size)+1);
		index->indexReg[index->size] = insert;
		(index->size)++;
	} else {

		// Verifica em qual registro começa a busca
		int search = (index->size)/2;

		while(search >= 0 && search < index->size){
			
		}

	}

}

void deleteIndex(INDEX *index){

	int i;

	for(i=0; i<index->size; i++){
		free(index->indexReg[i]);
	}

	free(index->indexReg);
	free(index);

}