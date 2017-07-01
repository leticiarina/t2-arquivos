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

	do {
		printOptions();
		scanf("%d", &option);

		switch(option){
			case 1: readRegisters(indexSizeIndicator, indexDelimiterRegister, indexFixedFields);
				break;
			case 2: printf("opção 2\n");
				break;
			case 3: printf("opção 3\n");
				break;
			case 4: printf("opção 4\n");
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