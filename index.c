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

	INDEXREG *insert = (INDEXREG*) malloc(sizeof(INDEXREG));

	if(insert != NULL){
		insert->ticket = ticket;
		insert->byteOffset = byteOffset;
	}

	// Procura o byte em que o registro deve ser inserido no arquivo de índice
	insertIndex(index, insert);

	return TRUE;

}

int searchIndex(INDEX *index, int ticket){
	int first, last, middle;
	first = 0;
	last = index->size - 1;

	while(first <= last){

		middle = (first + last)/2;

		//printf("Comparando %d com %d[%d]\n", ticket, index->indexReg[middle]->ticket, middle);

		//if(middle+1 <= last && index->indexReg[middle]->ticket > ticket && index->indexReg[middle+1]->ticket < ticket)
		//	return middle + 1;
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

	//printf("vou inserir no local %d\n", local);

	int i = index->size-1;
	index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size)+1);

	while(i > local){
		index->indexReg[i] = index->indexReg[i-1];
		i--;
	}

	index->size++;
	index->indexReg[local] = insert; 

}

// Procura o byte em que um arquivo deve ser inserido.
int insertIndex(INDEX *index, INDEXREG *insert){

	index->indexReg = (INDEXREG**) realloc(index->indexReg,sizeof(INDEXREG*)*(index->size)+1);
	index->indexReg[index->size] = insert;
	(index->size)++;
}

void Quick(INDEX* index, int inicio, int fim){
	int pivo, aux, i, j, meio;
   
	i = inicio;
	j = fim;
   
	meio = (int) ((i + j) / 2);
	pivo = index->indexReg[meio]->ticket;
   
	do{
		while (index->indexReg[i]->ticket < pivo) i = i + 1;
		while (index->indexReg[j]->ticket > pivo) j = j - 1;
      
      	if(i <= j){
        	aux = index->indexReg[i]->ticket;
        	index->indexReg[i]->ticket = index->indexReg[j]->ticket;
        	index->indexReg[j]->ticket = aux;
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

void removeIndex(INDEX* index, int local){
	int i = local;
	while(i < index->size){
		index->indexReg[i] = index->indexReg[i+1];
		i++;
	}
	index->size--;
	index->indexReg = (INDEXREG**) realloc(index->indexReg, sizeof(INDEXREG*)*(index->size));
}


// Exibe as estatísticas dos arquivos de índice.
void showStatisticsIndex(INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields){

	printf("_________________________________________________________________________________\n");
	printf("|		Quantidade de entradas no arquivo de índice			|\n");
	printf("|_______________________________________________________________________________|\n");
	printf("| Indicador de tamanho | Delimitador de registros  | Quantidade fixa de campos  |\n");
	printf("|_______________________________________________________________________________|\n");
	printf("|		%d 			%d	     			%d 	|\n", indexSizeIndicator->size, indexDelimiterRegister->size, indexFixedFields->size);
	printf("|_______________________________________________________________________________|\n");

}

void deleteIndex(INDEX *index){

	int i;

	for(i=0; i<index->size; i++){
		free(index->indexReg[i]);
	}

	free(index->indexReg);
	free(index);

}

int removeRegister(INDEX* index, int ticket, int type, int *topo){
	int local, aux, tamanho;
	char asterisco = '*';
	FILE *output = NULL;

	if(type == 1) output = fopen("indicador-tamanho.bin","wb");
	else if(type == 2) output = fopen("delimitador-registros.bin","wb");
	else if(type == 3) output = fopen("numero-fixo-campos.bin","wb");
	else{
		printf("ERRO AO ABRIR O ARQUIVO DE DADOS!\n");
		return 0;
	}

	local = searchIndex(index,ticket); //faz a busca binaria no indice primario
	//local++;
	printf("TICKET ENCONTRADO NA POSIVCAO %d!\n",local);
	if(ticket == index->indexReg[local]->ticket){
		fseek(output,index->indexReg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro
		tamanho = sizeOfRegister(output, type); //verifica tamanho do registro
		fseek(output,index->indexReg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro novamente
		fwrite(&asterisco,1,sizeof(char),output); //insere o *
		fwrite(&tamanho,1,sizeof(int),output); //insere o tamanho do registro
		fwrite(topo,1,sizeof(int),output); //insere o antigo topo
		*topo = index->indexReg[local]->byteOffset; //atualiza o topo
		removeIndex(index,local); //remove no indice primario

		printf("REMOÇÃO FOI REALIZADA!\n");
		return TRUE;
	}

	printf("REMOÇÃO NÃO FOI REALIZADA!\n");
	return FALSE;
}

// Retorna o tamanho de um registro.
int sizeOfRegister(FILE *output, int type){

	int size = 0, aux;

	// Registro com indicador de tamanho
	if(type == 1){
		fscanf(output, "%d", &size);
		return size;

	// Registros com delimitador
	} else if(type == 2){

		/** Ticket: int
		***	documento, dataHoraCadastro, dataHoraAtualiza: 20 bytes cada um **/
		size += sizeof(int) + 3*(20*sizeof(char));
		fseek(output, size, SEEK_CUR);

		// Dominio: campo variável		
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Nome: campo variável		
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Cidade: campo variável
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(int) + aux; // Int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);
		
		// UF: campo variável		
		fscanf(output, "%d", &aux); // Leitura do tamanho
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
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Nome: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// Cidade: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo
		fseek(output, aux, SEEK_CUR);

		// UF: pula o byte de indicação do campo atual
		fseek(output, sizeof(char), SEEK_CUR);
		fscanf(output, "%d", &aux); // Leitura do tamanho
		size += sizeof(char) + sizeof(int) + aux; // Char indicador de campo, int que armazena o tamanho e os bytes ocupados pelo campo

		size += sizeof(char); // Contabiliza o último indicador de campo (final do registro)

		return size;

	}

}

void printIndexFile(INDEX* index){
	int i;
	printf("IMPRESSAO DO ARQUIVO INDICE:\n");
	for(i = 0; i < index->size; i++){
		printf("POSICAO: %d TICKET: %d BYTE OFFSET: %d\n",i,index->indexReg[i]->ticket,index->indexReg[i]->byteOffset);
	}
	printf("---------------------------------------\n");
}