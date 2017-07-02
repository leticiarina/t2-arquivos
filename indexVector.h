/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#ifndef _INDEX_VECTOR_H_
#define _INDEX_VECTOR_H_

// Struct para armazenar um registro
typedef struct indexReg {
	int ticket;
	int byteOffset;
} INDEXREG;

typedef struct index {
	int size; // tamanho do vetor de registros
	INDEXREG **indexReg; // vetor contendo vários registros
} INDEX;

INDEX *initIndex();
int writeIndexVector(int ticket, int byteOffset, FILE *indexFile, INDEX *index);
int searchIndex(INDEX *index, int ticket);
void insertAndShift(INDEX *index, INDEXREG* insert, int local);
int insertIndex(INDEX *index, INDEXREG *insert);
void showStatisticsIndex(INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields);
void deleteIndex(INDEX *index);
int removeRegister(INDEX* index, int ticket, int type, int *topo);
int sizeOfRegister(FILE *output, int type);
void Quick(INDEX* index, int inicio, int fim);
void orderIndex(INDEX *index);
void printIndexFile(INDEX* index);

#endif