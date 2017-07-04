/* José Augusto Noronha de Menezes Neto	NUSP: 9293049
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
int writeIndexVector(int ticket, int byteOffset, INDEX *index);
int searchIndex(INDEX *index, int ticket);
int insertIndex(INDEX *index, INDEXREG *insert);
void Quick(INDEX* index, int inicio, int fim);
void orderIndex(INDEX *index);
void removeIndexVector(INDEX* index, int local);
void showStatisticsIndex(INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields);
void deleteIndex(INDEX *index);
int sizeOfRegister(FILE *output, int type);

#endif