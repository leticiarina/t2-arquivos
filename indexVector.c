/* Donizeti Carlos dos Santos Junior	NUSP: 9393882
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdlib.h>
#include <stdio.h>
#include "file.h"
#include "indexVector.h"
#include "indexFile.h"

// Função initIndex: inicializa os vetores de registro
INDEX *initIndex(){

	INDEX *index = (INDEX*) malloc (sizeof(INDEX));

	if(index != NULL){
		index->size = 0;
		index->indexReg = (INDEXREG**) malloc(sizeof(INDEXREG*));
	}

	return index;

}

// Função writeIndexVector: adiciona um registro no vetor de índice.
int writeIndexVector(int ticket, int byteOffset, INDEX *index){

	INDEXREG *insert = (INDEXREG*) malloc(sizeof(INDEXREG));

	if(insert != NULL){
		insert->ticket = ticket;
		insert->byteOffset = byteOffset;
	}

	// Procura o byte em que o registro deve ser inserido no arquivo de índice
	insertIndex(index, insert);
	orderIndex(index);

	return TRUE;

}

// Função searchIndex: realiza uma busca binária para encontrar o local de inserção 
// de um registro
int searchIndex(INDEX *index, int ticket){
	int first, last, middle;
	first = 0;
	last = index->size - 1;

	while(first <= last){

		middle = (first + last)/2;

		if(index->indexReg[middle]->ticket < ticket)
			first = middle + 1;
		else if(index->indexReg[middle]->ticket == ticket)
			return middle;
		else
			last = middle - 1;

		if(last < first)
			return middle;

	}

	return middle;
}

void insertAndShift(INDEX *index, INDEXREG* insert, int local){

	int i = index->size-1;
	index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size)+1);

	while(i > local){
		index->indexReg[i] = index->indexReg[i-1];
		i--;
	}

	index->size++;
	index->indexReg[local] = insert; 

}

// Realiza a inserção no vetor de índice.
int insertIndex(INDEX *index, INDEXREG *insert){

	index->indexReg = (INDEXREG**) realloc(index->indexReg,sizeof(INDEXREG*)*(index->size+1));
	index->indexReg[index->size] = insert;
	(index->size)++;

}

void Quick(INDEX* index, int inicio, int fim){
	int pivo, i, j, meio;
   INDEXREG *aux;

	i = inicio;
	j = fim;
   
	meio = (int) ((i + j) / 2);
	pivo = index->indexReg[meio]->ticket;
   
	do{
		while (index->indexReg[i]->ticket < pivo) i = i + 1;
		while (index->indexReg[j]->ticket > pivo) j = j - 1;
      
      	if(i <= j){
        	aux = index->indexReg[i];
        	index->indexReg[i] = index->indexReg[j];
        	index->indexReg[j] = aux;
        	i = i + 1;
        	j = j - 1;
     	}
   }while(j > i);
   
   if(inicio < j) Quick(index, inicio, j);
   if(i < fim) Quick(index, i, fim);   
}

void orderIndex(INDEX *index){
	Quick(index, 0, index->size - 1);
}

// Remove um registro do vetor armazenado na memória principal
void removeIndexVector(INDEX* index, int local){
	int i = local;
	while(i+1 < index->size){
		index->indexReg[i] = index->indexReg[i+1];
		i++;
	}
	index->size--;
	index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size));
}


// Exibe as estatísticas dos arquivos de índice.
void showStatisticsIndex(INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields){

	char enter;
	int count = 0;

	printf("Pressione ENTER para visualizar o próximo registro e 0 para voltar ao menu.\n\n");

	printf(" -------------------------------------------------------------------------------------- \n");
	printf("| Indicador de tamanho 	| Delimitador de registros  | Quantidade fixa de campos       \n");
	printf("|--------------------------------------------------------------------------------------\n");
	printf("| Registros: %d 	 	Registros: %d	     	Registros: %d 	     	  \n", indexSizeIndicator->size, indexDelimiterRegister->size, indexFixedFields->size);
	printf("|--------------------------------------------------------------------------------------\n");
	
	scanf("%c", &enter);

	// Imprime um registro por vez
	while(enter == '\n' && count < indexSizeIndicator->size){
		printf("| Ticket: %d 			", indexSizeIndicator->indexReg[count]->ticket);
		printf("Ticket: %d 		", indexDelimiterRegister->indexReg[count]->ticket);
		printf("Ticket: %d 			\n", indexFixedFields->indexReg[count]->ticket);
		printf("| Byte offset: %d 		", indexSizeIndicator->indexReg[count]->byteOffset);
		printf("Byte offset: %d 		", indexDelimiterRegister->indexReg[count]->byteOffset);
		printf("Byte offset: %d 	\n", indexFixedFields->indexReg[count]->byteOffset);
		printf("|--------------------------------------------------------------------------------------\n");
		count++;
		scanf("%c", &enter);
	} 

	if(count == indexSizeIndicator->size)
		printf("Todos os registros impressos.\n\n");
	else if(enter != '\n')
		printf("Retorno ao menu principal.\n\n");

}

// Libera a memória alocada para um index
void deleteIndex(INDEX *index){

	int i;

	for(i=0; i<index->size; i++){
		free(index->indexReg[i]);
	}

	free(index->indexReg);
	free(index);

}

// Retorna o tamanho de um registro.
int sizeOfRegister(FILE *output, int type){

	int size = 0, aux;
	// Registro com indicador de tamanho
	if(type == 1){
		fread(&size,1,sizeof(int),output);
		return size;

	// Registros com delimitador
	} else if(type == 2){

		/** Ticket: int
		***	documento, dataHoraCadastro, dataHoraAtualiza: 20 bytes cada um **/
		size += sizeof(int) + 3*(20*sizeof(char));

		fseek(output, size, SEEK_CUR);

		// Dominio: campo variável
		fread(&aux,1,sizeof(int),output); //Leitura do tamanho		
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Nome: campo variável	
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Cidade: campo variável
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);
		
		// UF: campo variável		
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo

		size += sizeof(char); // Tamanho do delimitador de registro

		return size;

	// Número fixo de campos
	} else if(type == 3){

		/** Ticket: int
		***	documento, dataHoraCadastro, dataHoraAtualiza: 20 bytes cada um
		*** 4 bytes, um para indicar cada campo atual **/
		size += sizeof(int) + 3*(20*sizeof(char)) + 4*sizeof(char);
		fseek(output, size, SEEK_CUR);

		// Dominio: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Nome: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Cidade: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// UF: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fread(&aux,1,sizeof(int),output); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo

		size += sizeof(char); // Contabiliza o último indicador de campo (final do registro)

		return size;

	}

}