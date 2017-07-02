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
	insertByte = insertIndex(index, insert);

	return TRUE;

}

int searchIndex(INDEX *index, int ticket){
	int first, last, middle;
	first = 0;
	last = index->size - 1;
	middle = (first + last)/2;

	while(first <= last){
		if(index->indexReg[middle]->ticket < ticket) first = middle + 1;
		else if(index->indexReg[middle]->ticket == ticket) return middle;
		else last = middle - 1;

		if(first <= last) return middle;

	}

	return middle;
}

void insertAndShift(INDEX *index, INDEXREG* insert, int local){
	local++;
	int i = index->size;
	index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size)+1);

	while(i > local){
		index->indexReg[i+1] = index->indexReg[i];
		i--;
	}
	index->size++;
	index->indexReg[local] = insert; 
}

// Procura o byte em que um arquivo deve ser inserido.
int insertIndex(INDEX *index, INDEXREG *insert){

	int local;
	// Nenhum registro foi inserido ainda
	if(index->size == 0){
		index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size)+1);
		index->indexReg[index->size] = insert;
		(index->size)++;
	
	} else {
		local = searchIndex(index, insert->ticket);
		insertAndShift(index, insert, local);	
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