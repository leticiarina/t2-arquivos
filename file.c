/* Donizeti Carlos dos Santos Junior	NUSP: 9393882
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "indexVector.h"
#include "indexFile.h"

// Função readRegisters: realiza a leitura de um arquivo de registros.
void readRegisters(INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields){

	char *filename = (char*) malloc(sizeof(char)*256);
	printf("Insira o nome do arquivo que deseja ler: ");
	scanf("%s", filename);

	createOutputFiles(filename, indexSizeIndicator, indexDelimiterRegister, indexFixedFields);

	free(filename);
}

// Função createOutputFiles: cria os arquivos de saída dos três tipos desejados e os arquivos de índice.
int createOutputFiles(char *filename, INDEX *indexSizeIndicator, INDEX *indexDelimiterRegister, INDEX *indexFixedFields){

	FILE *registersFile = fopen(filename, "r");
    REG *regist = (REG*)malloc(sizeof(REG));

	FILE *sizeIndicator = fopen("indicador-tamanho.bin","wb");
	FILE *delimiterRegister = fopen("delimitador-registros.bin","wb");
	FILE *fixedFields = fopen("numero-fixo-campos.bin","wb");

	// Arquivos de índice
	FILE *indexFileSizeIndicator = fopen("indice-indicador-tamanho.bin","wb");
	FILE *indexFileDelimiterRegister = fopen("indice-delimitador-registros.bin","wb");
	FILE *indexFileFixedFields = fopen("indice-numero-fixo-campos.bin","wb");

	if(registersFile == NULL){
		printf("Erro ao abrir o arquivo solicitado.\n\n");
		return FALSE;
	}

    if(sizeIndicator == NULL || delimiterRegister == NULL || fixedFields == NULL){
        printf("Erro ao criar arquivos de saída.\n\n");
        return FALSE;
    }

    if(indexFileSizeIndicator == NULL || indexFileDelimiterRegister == NULL || indexFileFixedFields == NULL){
        printf("Erro ao criar arquivos de índice.\n\n");
        return FALSE;
    }

    int sizeIndicatorByte, delimiterRegisterByte, fixedFieldsByte;
    char end;

    while((end = getc(registersFile)) != EOF){

        fseek(registersFile, -1, SEEK_CUR);

        // Armazena o byte em que o próximo registro será armazenado
        sizeIndicatorByte = ftell(sizeIndicator);
        delimiterRegisterByte = ftell(delimiterRegister);
        fixedFieldsByte = ftell(fixedFields);

	    // Leitura de cada campo de cada registro
		regist->dominio = readField(registersFile);
		strcpy(regist->documento, readField(registersFile));
		regist->nome = readField(registersFile);
		regist->cidade = readField(registersFile);
		regist->uf = readField(registersFile);
		strcpy(regist->dataHoraCadastro, readField(registersFile));
		strcpy(regist->dataHoraAtualiza, readField(registersFile));
        fscanf(registersFile, "%d\n", &(regist->ticket));
 
 		// Escrita do registro no arquivo de saída
		writeOutputFiles(regist, sizeIndicator, 1);
		writeOutputFiles(regist, delimiterRegister, 2);
		writeOutputFiles(regist, fixedFields, 3);

		// Escrita no vetor de índice
		writeIndexVector(regist->ticket, sizeIndicatorByte, indexSizeIndicator);
		writeIndexVector(regist->ticket, delimiterRegisterByte, indexDelimiterRegister);
		writeIndexVector(regist->ticket, fixedFieldsByte, indexFixedFields);

		// Libera memória utilizada para guardar cada campo
	    free(regist->dominio);
	    free(regist->nome);
	    free(regist->cidade);
	    free(regist->uf);

    }

    //Ordena os vetores de índice
	orderIndex(indexSizeIndicator);
	orderIndex(indexDelimiterRegister);
	orderIndex(indexFixedFields);

	// Escreve nos arquivos de índice
	writeIndexFile(indexSizeIndicator, indexFileSizeIndicator);
	writeIndexFile(indexDelimiterRegister, indexFileDelimiterRegister);
	writeIndexFile(indexFixedFields, indexFileFixedFields);

    fclose(registersFile);

    fclose(sizeIndicator);
    fclose(delimiterRegister);
    fclose(fixedFields);

    fclose(indexFileSizeIndicator);
    fclose(indexFileDelimiterRegister);
    fclose(indexFileFixedFields);

    free(regist);

    printf("\nArquivo lido com sucesso. Criação de arquivos de saída efetuado.\n\n");
    
    return TRUE;

}

// Função readField: realiza a leitura de um campo no arquivo de registros.
char *readField(FILE *registersFile){
	
	char *str = NULL;
    char c;
    int counter = 0;
    
    fscanf(registersFile, "%c", &c);
    
    while(c != ';'){
        str = (char*) realloc(str, sizeof(char)*(counter+1));
        str[counter] = c;
        counter++;
        fscanf(registersFile,"%c", &c);
    }

    str = (char*) realloc(str, sizeof(char)*(counter+1));
    str[counter] = '\0';
    
    return str;

}

/* Função writeOutputFiles: escreve nos arquivos de saída.
** Type 1 - registros com indicador de tamanho
** 		2 - registros com delimitador
**		3 - registros com número fixo de campos */
void writeOutputFiles(REG *regist, FILE *output, int type){
	
	int size;

	/* 4 ints para armazenar o tamanho dos campos variáveis
	** 1 int para o ticket	
	** 3 de 20 bytes: documento, dataHoraCadastro, dataHoraAtualiza
	** 4 de campos variáveis */
	int regSize = 4*sizeof(int) + sizeof(int) + 3*(20*sizeof(char)) + (int)strlen(regist->dominio) + 
    (int)strlen(regist->nome)+ (int)strlen(regist->cidade)+ (int)strlen(regist->uf);
    

    if(type == 1)
		fwrite(&regSize, 1, sizeof(int), output);

    // Escrita dos campos de tamanho fixo
	if(type == 3) fprintf(output, "%c", '0');
	fwrite(&(regist->ticket), 1, sizeof(int), output);
	if(type == 3) fprintf(output, "%c", '1');
	fwrite(regist->documento, 20, sizeof(char), output);
	if(type == 3) fprintf(output, "%c", '2');
	fwrite(regist->dataHoraCadastro, 20, sizeof(char), output);
	if(type == 3) fprintf(output, "%c", '3');
	fwrite(regist->dataHoraAtualiza, 20, sizeof(char), output);

    // Escrita dos campos de tamanho variável: leitura do tamanho, escrita do tamanho e escrita do campo.
	if(type == 3) fprintf(output, "%c", '4');
    size = (int) strlen(regist->dominio);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->dominio, (int)strlen(regist->dominio), sizeof(char), output);

	if(type == 3) fprintf(output, "%c", '5');
    size = (int)strlen(regist->nome);
    fwrite(&size, 1, sizeof(int), output);
	fwrite(regist->nome, (int)strlen(regist->nome), sizeof(char), output);

    if(type == 3) fprintf(output, "%c", '6');
	size = (int)strlen(regist->cidade);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->cidade, (int)strlen(regist->cidade), sizeof(char), output);

	if(type == 3) fprintf(output, "%c", '7');
    size = (int)strlen(regist->uf);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->uf, (int)strlen(regist->uf), sizeof(char), output);
    
    if(type == 2)
		fprintf(output, "%c", '#');
	else if(type == 3)
		fprintf(output, "%c", '8');

}

int removeRegister(INDEX* index, int ticket, int type, int *topo){
	int local, aux, tamanho;
	char asterisco = '*';
	FILE *output = NULL;
	FILE *indexFile = NULL;

	if(type == 1) {
		output = fopen("indicador-tamanho.bin","ab+");
		indexFile = fopen("indice-indicador-tamanho.bin", "wb");
	} else if(type == 2) {
		output = fopen("delimitador-registros.bin","ab+");
		indexFile = fopen("indice-delimitador-registros.bin","wb");
	} else if(type == 3) {
		output = fopen("numero-fixo-campos.bin","ab+");
		indexFile = fopen("indice-numero-fixo-campos.bin","wb");
	} else{
		printf("ERRO AO ABRIR O ARQUIVO DE DADOS!\n");
		return 0;
	}

	local = searchIndex(index,ticket); //faz a busca binaria no indice primario

	if(ticket == index->indexReg[local]->ticket){
		fseek(output,index->indexReg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro
		tamanho = sizeOfRegister(output, type); //verifica tamanho do registro
		fseek(output,index->indexReg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro novamente
		fwrite(&asterisco,1,sizeof(char),output); //insere o *
		fwrite(&tamanho,1,sizeof(int),output); //insere o tamanho do registro
		fwrite(topo,1,sizeof(int),output); //insere o antigo topo
		*topo = index->indexReg[local]->byteOffset; //atualiza o topo
		removeIndexVector(index,local); //remove no vetor indice primario
		writeIndexFile(index, indexFile); // atualiza no disco
		
		return TRUE;
	}

	return FALSE;
}

void showRemovedStatistics(int topo1, int topo2, int topo3){

	FILE *sizeIndicator = fopen("indicador-tamanho.bin","rb");
	FILE *delimiterRegister = fopen("delimitador-registros.bin","rb");
	FILE *fixedFields = fopen("numero-fixo-campos.bin","rb");

	int size;
	char ast;

	if(topo1 == -1)
		printf("Nenhum registro foi removido.\n\n");
	
	else {

		// Coloca o ponteiro no byte com o tamanho do registro removido que está no topo
		if(topo1 != -1)
			fseek(sizeIndicator, topo1, SEEK_SET);
		if(topo2 != -1)
			fseek(delimiterRegister, topo2, SEEK_SET);
		if(topo3 != -1)
			fseek(fixedFields, topo3, SEEK_SET);
			
		fread(&ast, sizeof(char), 1, sizeIndicator);
		printf("ast = %c ", ast);
		fread(&ast, sizeof(char), 1, delimiterRegister);
		printf("ast = %c ", ast);
		fread(&ast, sizeof(char), 1, fixedFields);
		printf("ast = %c \n", ast);

		// Realiza a leitura do tamanho do registro em cada arquivo
		printf("Estou nos bytes: %d %d %d\n", ftell(sizeIndicator), ftell(delimiterRegister), ftell(fixedFields));
		fread(&size, sizeof(int), 1, sizeIndicator);
		printf("Tamanho: %d", size);
		fread(&size, sizeof(int), 1, delimiterRegister);
		printf("Tamanho: %d", size);
		fread(&size, sizeof(int), 1, fixedFields);
		printf("Tamanho: %d\n", size);


	}



	fclose(sizeIndicator);
	fclose(delimiterRegister);
	fclose(fixedFields);	

}