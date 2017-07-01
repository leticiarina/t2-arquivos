/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#ifndef _FILE_H_
#define _FILE_H_

#define FALSE 0
#define TRUE 1

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

void readRegisters();
int createOutputFiles(char *filename);
char *readField(FILE *registersFile);
void writeOutputFiles(REG *regist, FILE *output, int type);

#endif