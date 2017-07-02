/* Donizeti Carlos dos Santos Junior	NUSP: 9393882
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdio.h>
#include "indexFile.h"

// Realiza escrita no arquivo de índice.
void writeIndexFile(INDEX *index, FILE *indexFile){

	int i;

	for(i=0; i<index->size; i++){

		fwrite(&(index->indexReg[i]->ticket), 1, sizeof(int), indexFile);
		fwrite(&(index->indexReg[i]->byteOffset), 1, sizeof(int), indexFile);

	}

}