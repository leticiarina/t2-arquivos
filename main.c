/* José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdio.h>
#include "auxFunctions.h"
#include "file.h"
#include "indexVector.h"

int main (int argc, char *argv[]){

	int option;
	INDEX *indexSizeIndicator = initIndex();
	INDEX *indexDelimiterRegister = initIndex();
	INDEX *indexFixedFields = initIndex();
	REG *reg = NULL;
	int ticket;
	int topo1 = -1, topo2 = -1, topo3 = -1;
	int byteOffset;

	do {
		printOptions();
		scanf("%d", &option);

		switch(option){
			case 1: 
				readRegisters(indexSizeIndicator, indexDelimiterRegister, indexFixedFields);
				break;
			case 2: 
				printf("Qual o ticket a ser removido? ");
				scanf("%d", &ticket);
				if(removeRegister(indexSizeIndicator, ticket, 1, &topo1) == TRUE && 
					removeRegister(indexDelimiterRegister,ticket, 2, &topo2) == TRUE &&
					removeRegister(indexFixedFields, ticket, 3, &topo3) == TRUE)
					printf("Remoção do ticket feita com sucesso.\n\n");
				else 
					printf("Erro na remoção. Ticket não encontrado.\n\n");
				break;
			case 3: 
				reg = readNewRegist();
				if(insertNewRegister(1, &topo1, reg, indexSizeIndicator) == TRUE  &&
					insertNewRegister(2, &topo2, reg, indexDelimiterRegister) == TRUE &&
					insertNewRegister(3, &topo3, reg, indexFixedFields) == TRUE)
					printf("Inserção feita com sucesso.\n\n");
				else
					printf("Erro na inserção.\n\n");
				break;
			case 4: 
				showStatisticsIndex(indexSizeIndicator, indexDelimiterRegister, indexFixedFields);
				break;
			case 5:
				showRemovedStatistics(topo1, topo2, topo3);
				break;
		}

		if(option < 0 || option > 5)
			printf("Opção inválida. Digite novamente.\n");

	} while(option != 0);

	deleteIndex(indexSizeIndicator);
	deleteIndex(indexDelimiterRegister);
	deleteIndex(indexFixedFields);

	free(reg);

	printf("Programa encerrado.\n");

}