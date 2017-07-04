/* Donizeti Carlos dos Santos Junior	NUSP: 9393882
** José Augusto Noronha de Menezes Neto	NUSP: 9293049
** Letícia Rina Sakurai					NUSP: 9278010 */

#include <stdio.h>
#include "auxFunctions.h"

// Função printOptions: exibe na tela um menu com as opções disponíveis no programa.
void printOptions(){
	
	printf("Escolha a opção desejada:\n");
	printf("1. Leitura de registros a partir de um arquivo\n");
	printf("2. Remover registro\n");
	printf("3. Inserir registro adicional\n");
	printf("4. Visualizar estatisticas sobre o arquivo de índice\n");
	printf("5. Visualizar estatísticas sobre os registros removidos\n");
	printf("0. Sair\n");

}

char *readLine(){
	char *str = NULL;
	char c;
	int counter = 0;

	scanf("%c",&c);
	while(c != '\n'){
		str = (char*) realloc(str, sizeof(char)*(counter+1));
        str[counter] = c;
        counter++;
		scanf("%c",&c);
	}
	str = (char*) realloc(str, sizeof(char)*(counter+1));
	str[counter] = '\0';

	return str;
}

