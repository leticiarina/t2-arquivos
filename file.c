/* Donizeti Carlos dos Santos Junior	NUSP:
** Gabriel Oliveira da Silva Hirga 		NUSP: 9278052
** José Augusto Noronha de Menezes Neto	NUSP:
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file.h"

// Função readRegisters: realiza a leitura de um arquivo de registros.
void readRegisters(){

	char *filename = (char*) malloc(sizeof(char)*256);
	printf("Insira o nome do arquivo que deseja ler: ");
	scanf("%s", filename);

	createOutputFiles(filename);

	free(filename);

}

// Função createOutputFiles: cria os arquivos de saída dos três tipos desejados.
int createOutputFiles(char *filename){

	FILE *registersFile = fopen(filename, "r");
    REG *regist = (REG*)malloc(sizeof(REG));

	FILE *sizeIndicator = fopen("indicador-tamanho.bin","wb");
	FILE *delimiterRegister = fopen("delimitador-registros.bin","wb");
	FILE *fixedFields = fopen("numero-fixo-campos.bin","wb");
    
	if(registersFile == NULL){
		printf("Erro ao abrir o arquivo solicitado.\n");
		return FALSE;
	}

    if(sizeIndicator == NULL || delimiterRegister == NULL || fixedFields == NULL){
        printf("Erro ao criar arquivos de saída.\n");
        return FALSE;
    }

    char end;

    while((end = getc(registersFile)) != EOF){

        fseek(registersFile, -1, SEEK_CUR);

		regist->dominio = readField(registersFile);
		strcpy(regist->documento, readField(registersFile));
		regist->nome = readField(registersFile);
		regist->cidade = readField(registersFile);
		regist->uf = readField(registersFile);
		strcpy(regist->dataHoraCadastro, readField(registersFile));
		strcpy(regist->dataHoraAtualiza, readField(registersFile));
        fscanf(registersFile, "%d\n", &(regist->ticket));
 
		writeOutputFiles(regist, sizeIndicator, 1);
		writeOutputFiles(regist, delimiterRegister, 2);
		writeOutputFiles(regist, fixedFields, 3);

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

// 
void writeOutputFiles(REG *regist, FILE *output, int type){
	
	int size;
	int regSize = sizeof(int) + (int)strlen(regist->dominio) + 20*sizeof(char) + sizeof(int) + 
    (int)strlen(regist->nome) + sizeof(int) + (int)strlen(regist->cidade) + sizeof(int) + 
    (int)strlen(regist->uf) + 20*sizeof(char) + 20*sizeof(char) + sizeof(int);
    
    // Registro com indicador de tamanho
    if(type == 1)
		fwrite(&regSize, 1, sizeof(int), output);

    // Escrita dos campos de tamanho fixo
	fwrite(&(regist->ticket), 1, sizeof(int), output);
	fwrite(regist->documento, 20, sizeof(char), output);
	fwrite(regist->dataHoraCadastro, 20, sizeof(char), output);
	fwrite(regist->dataHoraAtualiza, 20, sizeof(char), output);

    // Escrita dos campos de tamanho variável: leitura do tamanho, escrita do tamanho e escrita do campo.
    size = (int) strlen(regist->dominio);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->dominio, (int)strlen(regist->dominio), sizeof(char), output);

    size = (int)strlen(regist->nome);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->nome, (int)strlen(regist->nome), sizeof(char), output);

    size = (int)strlen(regist->cidade);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->cidade, (int)strlen(regist->cidade), sizeof(char), output);

    size = (int)strlen(regist->uf);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->uf, (int)strlen(regist->uf), sizeof(char), output);
    
    // Uso de delimitadores entre registros
    if(type == 2)
		fprintf(output, "%s", "#");

}