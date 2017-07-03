/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdio.h>
#include "auxFunctions.h"
#include "file.h"
#include "index.h"

int main (int argc, char *argv[]){

	int option;
	INDEX *indexSizeIndicator = initIndex();
	INDEX *indexDelimiterRegister = initIndex();
	INDEX *indexFixedFields = initIndex();
	int ticket;
	int topo1 = -1, topo2 = -1, topo3 = -1;
	int byteOffset;

	do {
		printOptions();
		scanf("%d", &option);

		switch(option){
			case 1: 
				readRegisters(indexSizeIndicator, indexDelimiterRegister, indexFixedFields);
				printf("Digite o byteoffset do registro a ser impresso: ");
				scanf("%d",&byteOffset);
				printSizeIndicatorRegister(byteOffset);
				break;
			case 2: 
				printf("Qual o ticket a ser removido?\n");
				scanf("%d",&ticket);
				//printSizeIndicatorRegister(162);
				removeRegister(indexSizeIndicator,ticket,1,&topo1);
				removeRegister(indexDelimiterRegister,ticket,2,&topo2);
				removeRegister(indexFixedFields,ticket,3,&topo3);
				printSizeIndicatorRegisterUpdated(byteOffset);
				//printIndexFile(indexSizeIndicator);
				break;
			case 3: printf("opção 3\n");
				break;
			case 4: showStatisticsIndex(indexSizeIndicator, indexDelimiterRegister, indexFixedFields);
				break;
			case 5: printf("opção 5\n");
				break;
		}

		if(option < 0 || option > 5)
			printf("Opção inválida. Digite novamente.\n");

	} while(option != 0);

	deleteIndex(indexSizeIndicator);
	deleteIndex(indexDelimiterRegister);
	deleteIndex(indexFixedFields);
	printf("Programa encerrado.\n");

}