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
		//printf("-------------------------------------------\n");
		//printf("Dominio Lido: %s\n",regist->dominio);
		strcpy(regist->documento, readField(registersFile));
		//printf("Documento Lido: %s\n",regist->documento);
		regist->nome = readField(registersFile);
		//printf("Nome Lido: %s\n",regist->nome);
		regist->cidade = readField(registersFile);
		//printf("Cidade Lida: %s\n",regist->cidade);
		regist->uf = readField(registersFile);
		//printf("UF Lido: %s\n",regist->uf);
		strcpy(regist->dataHoraCadastro, readField(registersFile));
		//printf("Data Hora Cadastro Lido: %s\n",regist->dataHoraCadastro);
		strcpy(regist->dataHoraAtualiza, readField(registersFile));
		//printf("Data Hora Atualiza Lido: %s\n",regist->dataHoraAtualiza);
        fscanf(registersFile, "%d\n", &(regist->ticket));
        //printf("Ticket Lido: %d\n",regist->ticket);
        //printf("--------------------------------------------\n");
 
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

void printSizeIndicatorRegisterUpdated(int byteOffset){
	FILE *fp = fopen("indicador-tamanho.bin","rb");
	int tamanho, topo;
	char asterisco;

	fseek(fp,byteOffset,SEEK_SET);

	fread(&asterisco,1,sizeof(char),fp);
	printf("CHAR LIDO: %c\n",asterisco);

	fread(&tamanho,1,sizeof(int),fp);
	printf("TAMANHO LIDO: %d\n",tamanho);

	fread(&topo,1,sizeof(int),fp);
	printf("TOPO LIDO: %d\n",topo);

	fclose(fp);
}

void printSizeIndicatorRegister(int byteOffset){
	FILE *fp = fopen("indicador-tamanho.bin","rb");
	int tamanho, ticket;

	fseek(fp,byteOffset,SEEK_SET);

	fread(&tamanho,1,sizeof(int),fp);
	printf("Tamanho do registro: %d\n",tamanho);

	fread(&ticket,1,sizeof(int),fp);
	printf("Ticket lido: %d\n",ticket);

	fclose(fp);

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

// Função removeRegister: remove um registro do índice e do arquivo de saída
int removeRegister(INDEX* index, int ticket, int type, int *topo){
	int local, aux, tamanho;
	char asterisco = '*';
	FILE *output = NULL;
	FILE *indexFile = NULL;

	if(type == 1) {
		output = fopen("indicador-tamanho.bin","r+b");
		indexFile = fopen("indice-indicador-tamanho.bin", "wb");
	} else if(type == 2) {
		output = fopen("delimitador-registros.bin","r+b");
		indexFile = fopen("indice-delimitador-registros.bin","wb");
	} else if(type == 3) {
		output = fopen("numero-fixo-campos.bin","r+b");
		indexFile = fopen("indice-numero-fixo-campos.bin","wb");
	} else{
		printf("ERRO AO ABRIR O ARQUIVO DE DADOS!\n");
		return 0;
	}

	local = searchIndex(index,ticket); //faz a busca binaria no indice primario

	if(ticket == index->indexReg[local]->ticket){
		fseek(output,index->indexReg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro
		tamanho = sizeOfRegister(output, type); //verifica tamanho do registro
		printf("tamanho do registro %d\n", tamanho);
		fseek(output,index->indexReg[local]->byteOffset,SEEK_SET); //vai para o inicio do registro novamente
		printf("vou apagar o arquivo, estou no byte %d\n", ftell(output));
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

// Função showRemovedStatistics: mostra estatísticas sobre os registros já removidos
void showRemovedStatistics(int topo1, int topo2, int topo3){

	FILE *sizeIndicator = fopen("indicador-tamanho.bin","rb+");
	FILE *delimiterRegister = fopen("delimitador-registros.bin","rb+");
	FILE *fixedFields = fopen("numero-fixo-campos.bin","rb+");

	int size, num;
	char ast;

	if(topo1 == -1)
		printf("Nenhum registro foi removido.\n\n");
	
	else {
		// Coloca o ponteiro no byte com o tamanho do registro removido que está no topo
		while(topo1 != -1){
			printf("Digite 0 para passar pelos registros removidos!");
			scanf("%d",&num);
		
			fseek(sizeIndicator, topo1 + 1, SEEK_SET);
			fseek(delimiterRegister, topo2 + 1, SEEK_SET);
			fseek(fixedFields, topo3 + 1, SEEK_SET);

			printf("Delimitador de tamanho: Tamanho: %d Byte offset %d\n", size, (int)ftell(sizeIndicator)-1);
			fread(&size, sizeof(int), 1, delimiterRegister);
			printf("Delimitador de registros: Tamanho: %d Byte offset %d\n", size, (int)ftell(delimiterRegister)-1);
			fread(&size, sizeof(int), 1, fixedFields);
			printf("Fixo: Tamanho: %d Byte offset %d\n", size, (int)ftell(fixedFields)-1);
			fread(&topo1, sizeof(int), 1, sizeIndicator);
			fread(&topo2, sizeof(int), 1, delimiterRegister);
			fread(&topo3, sizeof(int), 1, fixedFields);
		}
	}

	fclose(sizeIndicator);
	fclose(delimiterRegister);
	fclose(fixedFields);	

}