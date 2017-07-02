/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "index.h"

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
	FILE *fileSizeIndicator = fopen("indice-indicador-tamanho.bin","wb");
	FILE *fileDelimiterRegister = fopen("indice-delimitador-registros.bin","wb");
	FILE *fileFixedFields = fopen("indice-numero-fixo-campos.bin","wb");

	if(registersFile == NULL){
		printf("Erro ao abrir o arquivo solicitado.\n");
		return FALSE;
	}

    if(sizeIndicator == NULL || delimiterRegister == NULL || fixedFields == NULL){
        printf("Erro ao criar arquivos de saída.\n");
        return FALSE;
    }

    if(fileSizeIndicator == NULL || fileDelimiterRegister == NULL || fileFixedFields == NULL){
        printf("Erro ao criar arquivos de índice.\n");
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

		// Escrita no arquivo de índice
		writeIndexFiles(regist->ticket, sizeIndicatorByte, fileSizeIndicator, indexSizeIndicator);
		writeIndexFiles(regist->ticket, delimiterRegisterByte, fileDelimiterRegister, indexDelimiterRegister);
		writeIndexFiles(regist->ticket, fixedFieldsByte, fileFixedFields, indexFixedFields);

		// Libera memória utilizada para guardar cada campo
	    free(regist->dominio);
	    free(regist->nome);
	    free(regist->cidade);
	    free(regist->uf);

    }

    fclose(registersFile);

    fclose(sizeIndicator);
    fclose(delimiterRegister);
    fclose(fixedFields);

    fclose(fileSizeIndicator);
    fclose(fileDelimiterRegister);
    fclose(fileFixedFields);

    free(regist);

    printf("\nArquivo lido com sucesso. Criação de arquivos de saída efetuado.\n");
    
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
	if(type == 3) fprintf(output, "%d", 0);
	fwrite(&(regist->ticket), 1, sizeof(int), output);
	if(type == 3) fprintf(output, "%d", 1);
	fwrite(regist->documento, 20, sizeof(char), output);
	if(type == 3) fprintf(output, "%d", 2);
	fwrite(regist->dataHoraCadastro, 20, sizeof(char), output);
	if(type == 3) fprintf(output, "%d", 3);
	fwrite(regist->dataHoraAtualiza, 20, sizeof(char), output);

    // Escrita dos campos de tamanho variável: leitura do tamanho, escrita do tamanho e escrita do campo.
	if(type == 3) fprintf(output, "%d", 4);
    size = (int) strlen(regist->dominio);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->dominio, (int)strlen(regist->dominio), sizeof(char), output);

	if(type == 3) fprintf(output, "%d", 5);
    size = (int)strlen(regist->nome);
    fwrite(&size, 1, sizeof(int), output);
	fwrite(regist->nome, (int)strlen(regist->nome), sizeof(char), output);

    if(type == 3) fprintf(output, "%d", 6);
	size = (int)strlen(regist->cidade);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->cidade, (int)strlen(regist->cidade), sizeof(char), output);

	if(type == 3) fprintf(output, "%d", 7);
    size = (int)strlen(regist->uf);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->uf, (int)strlen(regist->uf), sizeof(char), output);
    
    if(type == 2)
		fprintf(output, "%c", "#");
	else if(type == 3)
		fprintf(output, "%d", 8);

}

int removeRegister(INDEX* index, int ticket, FILE *output, int *topo){
	int local;
	int aux;
	char asterisco = '*';

	local = searchIndex(index,ticket); //faz a busca binaria no indice primario
	if(ticket == index->indexreg[local]->ticket){
		//verifica tamanho do registro
		
			
		fseek(output,indexreg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro
		fwrite(&asterisco,1,sizeof(char),output); //insere o *
		fwrite(&tamanho,1,sizeof(int),output); //insere o tamanho do registro
		fwrite(topo,1,sizeof(int),output); //insere o antigo topo
		*topo = index->indexreg[local]->byteOffset; //atualiza o topo
		removeIndex(index,local); //remove no indice primario

		printf("REMOÇÃO FOI REALIZADA!\n");
		return 1;
	}

	printf("REMOÇÃO NÃO FOI REALIZADA!\n");
	return 0;
}