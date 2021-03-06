/* Donizeti Carlos dos Santos Junior	NUSP: 9393882
** José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#ifndef _FILE_H_
#define _FILE_H_

#define FALSE 0
#define TRUE 1

#include "indexVector.h"

typedef struct reg{
    char *dominio; //variable size
    char documento[20]; //fixed size
    char *nome; //variable size
    char *cidade; //variable size
    char *uf; //variable size
    char dataHoraCadastro[20]; //fixed size
    char dataHoraAtualiza[20]; //fixed size
    int ticket; //fixed size
}REG;

void readRegisters(INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields);
int createOutputFiles(char *filename, INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields);
char *readField(FILE *registersFile);
void writeOutputFiles(REG *regist, FILE *output, int type);
int removeRegister(INDEX* index, int ticket, int type, int *topo);
void showRemovedStatistics(int topo1, int topo2, int topo3);
REG* readNewRegist();
int insertNewRegister(int type, int *topo, REG* regist, INDEX *index);
int sizeNewRegister(REG *regist, int type);


#endif