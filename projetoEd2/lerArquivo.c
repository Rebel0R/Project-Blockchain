#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include "openssl/crypto.h"
#include "openssl/sha.h"

//função utilizada para gerar o arquivo blocosMinerados.txt
//para gerar o .txt execute o gcc e em seguida digite" ./a.out > blocosMinerados.txt"

typedef struct BlocoNaoMinerado{
	unsigned int numero;//4    identificador do bloco
    unsigned int nonce;//4	   identifica se o hash foi atualizado
    unsigned char data[184];//Deve ser inicializado com zeros.  //contém total de transações, end ini, end fim, bitcoin
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH]; //32  //hash prévio, o primeiro bloco deve ter hash inicializado com zero
}BlocoNaoMinerado;

typedef struct BlocoMinerado{
	BlocoNaoMinerado bloco;
	unsigned char hash[SHA256_DIGEST_LENGTH]; //deve ter 000000... para ser considerado minerado
}BlocoMinerado;

void printHash(unsigned char hash[], int length){
  int i;
  printf("HASH ATUAL:\n");
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);

  printf("\n");
}

//ok
void imprimirBlocoMinerado (BlocoMinerado *blocoM){
    int cont = 0;
    printf("Bloco: %d\n", blocoM->bloco.numero);
    printf("Noce: %d\n",  blocoM->bloco.nonce);
    printf("End Ini | End Fim | Bitcoins\n");
    for(int j=0; j < 185; j++){
        if(blocoM->bloco.data[j] == 0 && blocoM->bloco.data[j+1] == 0 && blocoM->bloco.data[j+2] == 0){
            break;
        }else{
            printf("  %d\t ",  blocoM->bloco.data[j]);
            cont ++;
            if(cont == 3){
                printf("\n");
                cont = 0;
            }
        } 
    }
    printf("\nHASH ANTERIOR: \n");
    for(int j=0; j<SHA256_DIGEST_LENGTH; j++){
        printf("%02x",  blocoM->bloco.hashAnterior[j]);
    }
    printf("\n");
    printHash(blocoM->hash, SHA256_DIGEST_LENGTH);
    printf("\n");
}

void imprimirTodosBlocosMinerados(){
    int control, op;
    BlocoMinerado blocoLeitura;
    FILE *file = fopen("blocosMinerados.bin", "r+b");
    while(!feof(file)){
        control = fread(&(blocoLeitura), sizeof(struct BlocoMinerado), 1, file);
        if(ferror(file)){
           printf("Erro!\n");
        }else{
            if(control != 0){
                imprimirBlocoMinerado(&blocoLeitura);
            }
        }
    }
    fclose(file);
}

int main(){
    imprimirTodosBlocosMinerados();
    return 0;
}