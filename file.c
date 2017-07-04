/* Donizeti Carlos dos Santos Junior	NUSP: 9393882
** José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "auxFunctions.h"
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
	char zero = '0', um = '1', dois = '2', tres = '3', quatro = '4';
	char cinco = '5', seis = '6', sete = '7', oito = '8', delim = '#';

	/* 4 ints para armazenar o tamanho dos campos variáveis
	** 1 int para o ticket
	** 3 de 20 bytes: documento, dataHoraCadastro, dataHoraAtualiza
	** 4 de campos variáveis */
	int regSize = 4*sizeof(int) + sizeof(int) + 3*(20*sizeof(char)) + (int)strlen(regist->dominio) +
    (int)strlen(regist->nome)+ (int)strlen(regist->cidade)+ (int)strlen(regist->uf);

    if(type == 1)
		fwrite(&regSize, 1, sizeof(int), output);

    // Escrita dos campos de tamanho fixo
	if(type == 3) fwrite(&zero, 1, sizeof(char), output);
	fwrite(&(regist->ticket), 1, sizeof(int), output);
	if(type == 3) fwrite(&um, 1, sizeof(char), output);
	fwrite(regist->documento, 20, sizeof(char), output);
	if(type == 3) fwrite(&dois, 1, sizeof(char), output);
	fwrite(regist->dataHoraCadastro, 20, sizeof(char), output);
	if(type == 3) fwrite(&tres, 1, sizeof(char), output);
	fwrite(regist->dataHoraAtualiza, 20, sizeof(char), output);

    // Escrita dos campos de tamanho variável: leitura do tamanho, escrita do tamanho e escrita do campo.
	if(type == 3) fwrite(&quatro, 1, sizeof(char), output);
    size = (int) strlen(regist->dominio);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->dominio, (int)strlen(regist->dominio), sizeof(char), output);

	if(type == 3) fwrite(&cinco, 1, sizeof(char), output);
    size = (int)strlen(regist->nome);
    fwrite(&size, 1, sizeof(int), output);
	fwrite(regist->nome, (int)strlen(regist->nome), sizeof(char), output);

    if(type == 3) fwrite(&seis, 1, sizeof(char), output);
	size = (int)strlen(regist->cidade);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->cidade, (int)strlen(regist->cidade), sizeof(char), output);

	if(type == 3) fwrite(&sete, 1, sizeof(char), output);
    size = (int)strlen(regist->uf);
    fwrite(&size, 1, sizeof(int), output);
    fwrite(regist->uf, (int)strlen(regist->uf), sizeof(char), output);

    if(type == 2)
		fwrite(&delim, 1, sizeof(char), output);
	else if(type == 3)
		fwrite(&oito, 1, sizeof(char), output);
}

// Função removeRegister: remove um registro do índice e do arquivo de saída
int removeRegister(INDEX* index, int ticket, int type, int *topo){
	int local, aux, tamanho;
	char asterisco = '*';
	FILE *output = NULL;
	FILE *indexFile = NULL;

	if(type == 1) {
		output = fopen("indicador-tamanho.bin","rb+");
		indexFile = fopen("indice-indicador-tamanho.bin", "wb");
	} else if(type == 2) {
		output = fopen("delimitador-registros.bin","rb+");
		indexFile = fopen("indice-delimitador-registros.bin","wb");
	} else if(type == 3) {
		output = fopen("numero-fixo-campos.bin","rb+");
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
		
		fclose(output);

		return TRUE;
	}

	fclose(output);

	return FALSE;
}

// Função showRemovedStatistics: mostra estatísticas sobre os registros já removidos
void showRemovedStatistics(int topo1, int topo2, int topo3){

	FILE *sizeIndicator = fopen("indicador-tamanho.bin","rb+");
	FILE *delimiterRegister = fopen("delimitador-registros.bin","rb+");
	FILE *fixedFields = fopen("numero-fixo-campos.bin","rb+");

	int size, byteSizeIndicator, byteDelimiterRegister, byteFixedFields;
	char ast, enter;

	if(topo1 == -1)
		printf("Nenhum registro foi removido.\n\n");

	else {

		printf("Pressione ENTER para visualizar o próximo registro removido ou 0 para voltar ao menu.\n\n");
		scanf("%c", &enter);

		// Coloca o ponteiro no byte com o tamanho do registro removido que está no topo
		while(enter == '\n' && topo1 != -1){
	
			byteSizeIndicator = topo1;
			byteDelimiterRegister = topo2;
			byteFixedFields = topo3;

			fseek(sizeIndicator, topo1 + 1, SEEK_SET);
			fseek(delimiterRegister, topo2 + 1, SEEK_SET);
			fseek(fixedFields, topo3 + 1, SEEK_SET);

			// Realiza a leitura do tamanho do registro em cada arquivo

			fread(&size, sizeof(int), 1, sizeIndicator);
			printf(" --------------------------------\n");
			printf("|     Indicador de tamanho      |\n");
			printf(" --------------------------------\n");
			printf(" Tamanho: %d    Byte offset %d \n ", size, byteSizeIndicator);
			printf(" ------------------------------\n\n");

			fread(&size, sizeof(int), 1, delimiterRegister);
			printf(" --------------------------------\n");
			printf("|   Delimitador de registros    |\n");
			printf(" --------------------------------\n");
			printf(" Tamanho: %d    Byte offset %d \n", size, byteDelimiterRegister);
			printf(" --------------------------------\n\n");
		
			fread(&size, sizeof(int), 1, fixedFields);
			printf(" ---------------------------------\n");
			printf("|    Quantidade fixa de campos   |\n");
			printf(" ---------------------------------\n");
			printf(" Tamanho: %d    Byte offset %d \n", size, byteFixedFields);
			printf(" ---------------------------------\n\n");
			fread(&topo1, sizeof(int), 1, sizeIndicator);
			fread(&topo2, sizeof(int), 1, delimiterRegister);
			fread(&topo3, sizeof(int), 1, fixedFields);

			scanf("%c", &enter);
		}

		if(topo1 == -1)
			printf("Todos os registros removidos impressos.\n\n");
		else if(enter != '\n')
			printf("Retorno ao menu principal.\n\n");
	}

	fclose(sizeIndicator);
	fclose(delimiterRegister);
	fclose(fixedFields);

}

REG* readNewRegist(){
	REG *regist = (REG*)malloc(sizeof(REG));
	char *str = NULL;

	printf("Digite o dominio: ");
	getchar();
	regist->dominio = readLine();
	printf("lido: %s\n",regist->dominio);

	printf("Digite o numero do documento da seguinte forma: ###.###.###/###-## ");
	str = readLine();
	strcpy(regist->documento,str);
	printf("lido: %s\n",regist->documento);

	printf("Digite o nome: ");
	regist->nome = readLine();
	printf("lido: %s\n",regist->nome);

	printf("Digite a UF: ");
	regist->uf = readLine();
	printf("lido: %s\n",regist->uf);

	printf("Digite uma cidade: ");
	regist->cidade = readLine();
	printf("lido: %s\n",regist->cidade);

	printf("Digite a data e hora de cadastro no formato: dd/mm/aaaa hh:mm:ss ");
	str = readLine();
	strcpy(regist->dataHoraCadastro,str);
	printf("lido: %s\n",regist->dataHoraCadastro);

	printf("Digite a data e hora de atualizacao no formato: dd/mm/aaaa hh:mm:ss ");
	str = readLine();
	strcpy(regist->dataHoraAtualiza,str);
	printf("lido: %s\n",regist->dataHoraAtualiza);

 	printf("Digite o numero do ticket: ");
	scanf("%d",&(regist->ticket));
	printf("lido: %d\n",regist->ticket);

	return regist;

}

int insertNewRegister(int type, int *topo, REG* regist, INDEX *index){
	int byteOffset = *topo, removedSize, insertIn, size = 0, nextRemoved = 0, byteLeft;
	FILE *output = NULL;
    
    // First-fit
	if(type == 1){
		output = fopen("indicador-tamanho.bin","rb+");
		
		do{
			fseek(output, byteOffset+1,SEEK_SET); 	
			fread(&removedSize, sizeof(int), 1, output); // Leitura do tamanho ocupado pelo registro removido
			fread(&nextRemoved, sizeof(int), 1,	 output); // Byte do próximo registro removido
			size = sizeNewRegister(regist, 1); // Calcula o tamanho do registro a ser inserido

			// Registro antigo possui mesmo tamanho
			if(size == removedSize){
				fseek(output,byteOffset+1,SEEK_SET); 
				writeOutputFiles(regist,output,1);
				writeIndexVector(regist->ticket, byteOffset, index);
				orderIndex(index);
				fclose(output);
				*topo = nextRemoved;
				return TRUE;

			// Tratamento de fragmentação (topo é o mesmo)
			} else if(size < removedSize){
				byteLeft = removedSize-size;
				fseek(output,byteOffset+1,SEEK_SET); 
				fwrite(&byteLeft, 1, sizeof(int), output); // Quantidade de bytes que sobrou
				fseek(output, byteOffset+byteLeft, SEEK_SET);
				writeOutputFiles(regist,output,1);
				writeIndexVector(regist->ticket, byteOffset+byteLeft, index);
				orderIndex(index);
				return TRUE;
			}

			byteOffset = nextRemoved;

		} while(size > removedSize && byteOffset != -1);

	// Best-fit
	} else if(type == 2){

		output = fopen("delimitador-registros.bin","rb+");

		size = sizeNewRegister(regist, 2); // Calcula o tamanho do registro a ser inserido
		byteLeft = INT_MAX;

		// Verifica todos os registros removidos
		do {
			fseek(output, byteOffset+1,SEEK_SET); 	
			fread(&removedSize, sizeof(int), 1, output); // Leitura do tamanho ocupado pelo registro removido
			fread(&nextRemoved, sizeof(int), 1,	 output); // Byte do próximo registro removido			
		
			// Verifica se existe um espaço mais adequado para inserção
			if(byteLeft > removedSize-size && removedSize-size >= 0){
				byteLeft = removedSize-size;
				insertIn = byteOffset;
			}

			byteOffset = nextRemoved;

		} while(nextRemoved != -1);
	
		fseek(output,insertIn+1,SEEK_SET); 
		fwrite(&byteLeft, 1, sizeof(int), output); // Quantidade de bytes que sobrou
		fseek(output, insertIn+byteLeft, SEEK_SET);
		writeOutputFiles(regist,output,2);
		writeIndexVector(regist->ticket, insertIn+byteLeft, index);
		orderIndex(index);
		return TRUE;

	// Worst-fit
	} else if(type == 3){

		output = fopen("numero-fixo-campos.bin","rb+");

		size = sizeNewRegister(regist, 3); // Calcula o tamanho do registro a ser inserido
		byteLeft = 0;

		// Verifica todos os registros removidos
		do {
			fseek(output, byteOffset+1,SEEK_SET); 	
			fread(&removedSize, sizeof(int), 1, output); // Leitura do tamanho ocupado pelo registro removido
			fread(&nextRemoved, sizeof(int), 1,	 output); // Byte do próximo registro removido			
		
			// Verifica se existe um espaço mais adequado para inserção
			if(byteLeft < removedSize-size && removedSize-size >= 0){
				byteLeft = removedSize-size;
				insertIn = byteOffset;
			}

			byteOffset = nextRemoved;

		} while(nextRemoved != -1);
	
		fseek(output,insertIn+1,SEEK_SET); 
		fwrite(&byteLeft, 1, sizeof(int), output); // Quantidade de bytes que sobrou
		fseek(output, insertIn+byteLeft, SEEK_SET);
		writeOutputFiles(regist,output,3);
		writeIndexVector(regist->ticket, insertIn+byteLeft, index);
		orderIndex(index);
		return TRUE;

	}


}

int sizeNewRegister(REG *regist, int type){

	int size = 0;

	// Indicador de tamanho do registro
	if(type == 1){

		/** 6 int:
		** - tamanho do registro
		** - ticket
		** - tamanho do dominio
		** - tamanho do nome
		** - tamanho da cidade
		** - tamanho do UF
		** 20 bytes - documento
		** 20 bytes - dataHoraCadastro
		** 20 - dataHoraAtualiza
		** Bytes ocupados por cada campo
		**/
		size += 6*sizeof(int) + 3*(20*sizeof(char))+ (int)strlen(regist->dominio)+(int)strlen(regist->nome)+(int)strlen(regist->cidade)+(int)strlen(regist->uf);
		return size;
	
	// Delimitador de registro
	} else if(type == 2){

		/** 5 int:
		** - ticket
		** - tamanho do dominio
		** - tamanho do nome
		** - tamanho da cidade
		** - tamanho do UF
		** 1 char: delimitador de registro
		** 20 bytes - documento
		** 20 bytes - dataHoraCadastro
		** 20 - dataHoraAtualiza
		** Bytes ocupados por cada campo
		**/

		size += 5*sizeof(int) + sizeof(char) + 3*(20*sizeof(char))+(int)strlen(regist->dominio)+(int)strlen(regist->nome)+(int)strlen(regist->cidade)+(int)strlen(regist->uf);
		return size;

	// Número fixo de campos
	} else if(type == 3){

		/** 5 int:
		** - ticket
		** - tamanho do dominio
		** - tamanho do nome
		** - tamanho da cidade
		** - tamanho do UF
		** 9 char: indicador de inicio de cada campo
		** 20 bytes - documento
		** 20 bytes - dataHoraCadastro
		** 20 - dataHoraAtualiza
		** Bytes ocupados por cada campo
		**/
		size += 5*sizeof(int) + 9*sizeof(char) + 3*(20*sizeof(char))+(int)strlen(regist->dominio)+(int)strlen(regist->nome)+(int)strlen(regist->cidade)+(int)strlen(regist->uf);
		return size;
	
	}

}