#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include "openssl/crypto.h"
#include "openssl/sha.h"

//compilar com: gcc projeto.c -lcrypto

typedef struct BlocoNaoMinerado{
	unsigned int numero;//4    identificador do bloco
    unsigned int nonce;//4	   identifica se o hash foi atualizado
    unsigned char data[184];//Deve ser inicializado com zeros.  //contém total de transações, end ini, end fim, bitcoin
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH]; //32  //hash prévio, o primeiro bloco deve ter hash null
}BlocoNaoMinerado;

typedef struct BlocoMinerado{
	BlocoNaoMinerado bloco;
	unsigned char hash[SHA256_DIGEST_LENGTH]; //deve ter 0000... para ser considerado minerado
}BlocoMinerado;

typedef struct No{
	BlocoMinerado *blocoNo;
	struct No *prox;
}No;

void printHash(unsigned char hash[], int length){
  int i;
  printf("\tValor de HASH\n");
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);

  printf("\n");
}

No *criaNo(){
	No *novo = (No *)malloc(sizeof(No));
	if(novo == NULL){
		printf("Erro!\n");
		exit(0);
	}
	return novo;
}

void alocaNoFim(No **lista, BlocoMinerado *b){
    No *novo = criaNo(novo);
    No *aux = *lista;
    if(novo == NULL){
        printf("Erro!\n");
        exit(0);
    }
    novo->blocoNo = b;
	//se a lista estiver vazia
	 if(*lista == NULL){
        *lista = novo;
        novo->prox = NULL;
    }else{
		//encontro o ultimo elemento
		while(aux->prox!= NULL){
			aux = aux->prox;
		}
		aux->prox = novo;
		novo->prox = NULL;
	}
}

//imprime toda a lista
void imprimirLista(No *lista){
    if(lista == NULL){
        printf("\tNão é posspivel imprimir, lista Vazia!\n");
    }
    //assim não mexemos na Lista
    No *aux = lista;
    //enquanto estiver com elementos na lista
    while(aux != NULL){
        printf("Bloco: %d\n", aux->blocoNo->bloco.numero);
		printf("Noce: %d\n", aux->blocoNo->bloco.nonce);
		printf("Nº de Transações: %d\n", aux->blocoNo->bloco.data[0]);
		printf("End Ini | End Fim | Bitcoins\n");
		int cont = 0;
		for(int j=1; j < aux->blocoNo->bloco.data[0]; j++){
			printf("  %d\t ", aux->blocoNo->bloco.data[j]);
			cont ++;
			if(cont == 3){
				printf("\n");
				cont = 0;
			}
		}
		printf("\nHASH ANTERIOR: \n");
		for(int j=0; j<SHA256_DIGEST_LENGTH; j++){
			printf("%02x", aux->blocoNo->bloco.hashAnterior[j]);
		}
		printf("\n");
		printHash(aux->blocoNo->hash, SHA256_DIGEST_LENGTH);
		printf("\n");
        //vai avançar na lista
        aux = aux->prox;
    }
}

//necessário em todo novo bloco não minerado
void inicializaZero(BlocoMinerado *x){
	for (int i = 0; i < 185; ++i)
	{
		x->bloco.data[i]=0;
	}
}

//função utilizada apenas para o 1º BLOCO da blockchain
void inicializaZeroHash(BlocoMinerado *x){
	for(int a=0; a<SHA256_DIGEST_LENGTH; a++){
		x->bloco.hashAnterior[a] = 0;
	}

}
//função que copia o hash de um bloco anterior para o proximo
void copiaHash(BlocoMinerado *x, BlocoMinerado *y){
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		x->bloco.hashAnterior[i] = y->hash[i];
	}
}
//função que apresenta o hash de um bloco
void buscaHash(No *lista, int k){
	No *busca = lista;
	int cont = 0;
	while(busca != NULL){
		if(busca->blocoNo->bloco.numero == k){
			printf("\tBusca Encontrada!\n");
			printf("BLOCO: %d\n", busca->blocoNo->bloco.numero);
			printf("NONCE: %d\n", busca->blocoNo->bloco.nonce);
			printHash(busca->blocoNo->hash, SHA256_DIGEST_LENGTH);
			printf("\n");
			cont++;
		}
		busca = busca->prox;
	}
	if(cont == 0){
		printf("Busca Não Encontrada!\n");
	}
}
//recebe um endereco como parametro, buscando o end em cada bloco
void buscaQuantBitcoinEnd(No *lista, int k){
	No *busca = lista;
	int cont = 0;
	while(busca != NULL){
		for(int i=1; i<busca->blocoNo->bloco.data[0]; i++){
			if(busca->blocoNo->bloco.data[i] == k){
				printf("\tEndereço Inicial da Transação Encontrado!\n");
				printf("Bloco: %d\n", busca->blocoNo->bloco.numero);
				printf("End Ini --> %d\nQuantidade de Bitcoins: %d\n", busca->blocoNo->bloco.data[i], busca->blocoNo->bloco.data[i+2]);
				cont++;
				i+=2;
			}else{
				i+=2;
			}
		}
		printf("\n");
		busca = busca->prox;
	}
	if(cont == 0){
		printf("\tBusca Não Encontrada!\n\n");
	}
}
//encontra endereco com maior bitcoin em cada bloco
void encontraEndMaiorBitcoin(No *lista){
	No *busca = lista;
	int maior = 0, end = 0;
	while(busca != NULL){
		printf("----------\n");
		printf("Bloco: %d\n", busca->blocoNo->bloco.numero);
		printf("Num de transação: %d\n", busca->blocoNo->bloco.data[0]);
		for(int i=1; i<busca->blocoNo->bloco.data[0]; i++){
			if(busca->blocoNo->bloco.data[i+3] == 0){
				break;
			}
			if(busca->blocoNo->bloco.data[i+2]>maior){
				end = busca->blocoNo->bloco.data[i];
				maior = busca->blocoNo->bloco.data[i+2];
				i+=2;
			}else{
				i+=2;
			}
		}
		printf("Endereço com maior quantidade de bitcoins do Bloco\n");
		printf("End Ini: %d --> Qtd de Bitcoins: %d\n", end, maior);
		busca = busca->prox;
		maior = 0;
		end = 0;
	}
}

int main(){
	No *lista = NULL;
	//contem a semente de numeros gerados aleatoriamente
	MTRand r = seedRand(1234567);
	int tran, op;
	//alterar o indice do vetor conforme a quantidade de blocos
	BlocoMinerado block[1000];
	
	for(int i=0; i<(sizeof(block)/sizeof(block[0])); i++){
		(block+i)->bloco.numero = i+1;
		(block+i)->bloco.nonce = 0;
		inicializaZero(block+i);
		//primeiro bloco
		if(i ==0){
			inicializaZeroHash(block+i);
		}else{
			//passar o valor do hash para o hash anterior quando for o segundo bloco
			copiaHash(block+i, block+(i-1));
		}
		tran = genRandLong(&r)%61 + 1; //para as 61 transações necessárias
		for(int j=0; j<tran; j++){
			if(j == 0){
				(block+i)->bloco.data[0] = tran;
			}else{
				(block+i)->bloco.data[j] = genRandLong(&r)%256;
				(block+i)->bloco.data[j+1] = genRandLong(&r)%256;
				(block+i)->bloco.data[j+2] = genRandLong(&r)%50 + 1;
				j = j + 2;	
			}
		}
		SHA256((unsigned char *)&block[i], sizeof(block+i), (block+i)->hash);	
		//minera o bloco
		while((block+i)->hash[0] != 0 || (block+i)->hash[1]!= 0){
			(block+i)->bloco.nonce += 1; //soma 1 ao nonce cada vez que o calcula o hash
			SHA256((unsigned char *)&block[i], sizeof(block+i), (block+i)->hash);
	    }
		//aloca o bloco
		alocaNoFim(&lista, block+i);
	}

	
	do{
		printf("\t---OPÇÕES---\n");
		printf("\t1- Imprimir toda a lista\n");
		printf("\t2- Buscar um Bloco dentro da Lista e imprimir o hash\n");
		printf("\t3- Buscar quantidade de Bitcoins de um Endereço Inicial\n");
		printf("\t4- Apresentar maior quantidade de Bitcoins em cada Bloco\n");
		printf("\t0- Encerrar programa!\n");
		printf("Insira a opção escolhida: ");
		scanf("%d", &op);

		switch(op)
		{
			int pesq;
			case 1:
				printf("\tIMPRIMIR LISTA\n");
				imprimirLista(lista);
				break;
			
			case 2:
				printf("\tBUSCAR BLOCO\n");
				printf("\tInforme o bloco que deseja imprimir o hash: ");
				scanf("%d", &pesq);
				buscaHash(lista, pesq);
				break;

			case 3:
				printf("\tINFORMAR QUANTIDADE BITCOINS\n");
				printf("\tInforme o endereço de transação inicial: ");
				scanf("%d", &pesq);
				buscaQuantBitcoinEnd(lista, pesq);
			
			case 4:
				printf("\tMAIOR QUANTIDADE DE BITCOINS POR BLOCO\n");
				encontraEndMaiorBitcoin(lista);

			case 0:
				printf("\tPrograma Encerrado!\n");
				break;

			default:
				printf("Erro!");
				break;
		}

	}while(op!=0);
	return 0;
}