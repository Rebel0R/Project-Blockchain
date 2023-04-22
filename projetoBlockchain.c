#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mtwister.h"
#include "mtwister.c"
#include "openssl/crypto.h"
#include "openssl/sha.h"


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

//necessário em todo novo bloco não minerado o data se inicialize com zero
void inicializaDataZero(BlocoNaoMinerado *x){
	for (int i = 0; i < 185; ++i){
		x->data[i]=0;
	}
}

//inicializa o hash anterior do bloco genises com zero
void inicializaHashAntZero(BlocoNaoMinerado *bloco){
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		bloco->hashAnterior[i] = 0;
	}
}
//ok
void copiaHash(unsigned char *copia, unsigned char *copiado){
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		*(copia+i) = *(copiado+i);
	}
}
//ok
void copiaData(unsigned char *copia, unsigned char *copiado){
    for(int i=0; i<185; i++){
        *(copia+i) = *(copiado+i);
    }
}
//ok
void imprimirBlocoMinerado (BlocoMinerado *blocoM, int *qtdTransacoes){
    int aux = blocoM->bloco.numero -1;
    int transacao, cont = 0;
    transacao = *(qtdTransacoes + aux) * 3;
    printf("Bloco: %d\n", blocoM->bloco.numero);
    printf("Noce: %d\n",  blocoM->bloco.nonce);
    printf("Nº de Transações: %d\n", *(qtdTransacoes + aux));
    printf("End Ini | End Fim | Bitcoins\n");
    for(int j=0; j < transacao; j++){
        printf("  %d\t ",  blocoM->bloco.data[j]);
        cont ++;
        if(cont == 3){
            printf("\n");
            cont = 0;
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
//ok
void imprimirTodosBlocosMinerados(int *qtdTransacoes){
    int control;
    BlocoMinerado blocoLeitura;
    FILE *file = fopen("blocosMinerados.bin", "r+b");
    while(!feof(file)){
        control = fread(&(blocoLeitura), sizeof(struct BlocoMinerado), 1, file);
        if(ferror(file)){
           printf("Erro!\n");
        }else{
            if(control != 0){
                imprimirBlocoMinerado(&blocoLeitura, qtdTransacoes);
            }
        }
    }
    fclose(file);
}
//ok
int buscaQuantBitcoinEnd(unsigned int *transacoes, int end){
    int resultadoBusca;
    for(int i=0; i<256; i++){
        if(end == i){
            resultadoBusca = *(transacoes+i);
            return resultadoBusca;
        }
    }
    return -1;
}
//ok
void verificaMaiorBitcoinEnd(unsigned int *transacoes){
    int maior=0;
    int end=0;
    for(int i=0; i<256; i++){
        if(*(transacoes+i) > maior){
            maior = *(transacoes+i);
            end = i;
        }
    }
    printf("\tEndereço que possuí mais bitcoins\n");
    printf("\tEndereço %d -> Bitcoins: %d\n\n", end, maior);
}
//ok
int buscaBloco(int id, int *qtdTransacoes){
    int control;
    BlocoMinerado blocoLeitura;
    FILE *file = fopen("blocosMinerados.bin", "r+b");
    while(!feof(file)){
        control = fread(&(blocoLeitura), sizeof(struct BlocoMinerado), 1, file);
        if(ferror(file)){
           printf("Erro!\n");
        }else{
            if(control != 0){
                if(blocoLeitura.bloco.numero == id){
                    imprimirBlocoMinerado(&blocoLeitura, qtdTransacoes);
                    fclose(file);
                    return 0;
                }
            }
        }
    }
    fclose(file);
    printf("\tBLOCO NÃO ENCONTRADO!\n");
    return -1;
}
//ok
void trocaVetor(int *vetorA, int *vetorB){
    int x;
    x = *vetorA;
    *vetorA = *vetorB;
    *vetorB = x;
}
//ok
void selecionaIndices(unsigned int *vetor, unsigned int *enderecos, int ini, int fim){
    int i, indMenor = ini; //comeca valendo 0

    for(i=ini+1; i<fim; i++){
        if(*(vetor+indMenor)>*(vetor+i)){
            indMenor=i;
        }
    }
    //printf("\tIndice menor -> Vetor [%d]\n", indMenor);
    trocaVetor(vetor+ini, vetor+indMenor);
    trocaVetor(enderecos+ini, enderecos+indMenor);
}
//ok
void ordenarEnderecos(unsigned int *transacoes){
    unsigned int quantBitcoins[256] = {0}; 
    unsigned int enderecos[256] = {0};
    int i;
    int ini = 0, fim = 256;
    for(i=ini; i<fim; i++){
        quantBitcoins[i] = *(transacoes+i);
        enderecos[i] = i;
    }
    for(i=ini; i<fim; i++){
        selecionaIndices(quantBitcoins, enderecos, i, fim);
    }
    for(i=ini; i<fim; i++){
        printf("-Endereço %d -> Bitcoin: %d\n", enderecos[i], quantBitcoins[i]);
    }
    printf("\n");
}

int main(){
    MTRand r = seedRand(1234567);
    BlocoNaoMinerado blocoAMinerar;
    BlocoMinerado blocoMinerado;
    FILE *file;
    int opcao, aux, controle=0;
    unsigned char qtTransacao;
    unsigned int transBlocos[100]; //o tamanho desse vetor deve ser alterado mediante a qtd de blocos desejada
    unsigned int carteira[256] = {0};
    unsigned char auxHashAnterior[SHA256_DIGEST_LENGTH];
    unsigned char auxHashAtual[SHA256_DIGEST_LENGTH];

    //GERA A MINERAÇÃO    alterar para a quantidade de blocos que deseja minerar
    for(int contBlocos=0; contBlocos<100; contBlocos++){
        blocoAMinerar.numero = contBlocos+1; //id do bloco
        blocoAMinerar.nonce = 0; //inicializa-se com zero - critério de mineração
        inicializaDataZero(&blocoAMinerar); //inicializa as posições do data com zero
        //verifica se é o bloco genesis
        if(contBlocos == 0){
            inicializaHashAntZero(&blocoAMinerar);
        }else{
            copiaHash(blocoAMinerar.hashAnterior, auxHashAnterior);
        }
        qtTransacao = (unsigned char) (1 + (genRandLong(&r) % 61)); //para as 61 transações necessárias
        transBlocos[contBlocos] = qtTransacao;
        unsigned int endOrg, endDest;
		for(int j=0; j<(qtTransacao*3); j++){
            unsigned char endOrigem = (unsigned char) genRandLong(&r) % 256;
            unsigned char endDestino = (unsigned char) genRandLong(&r) % 256;
            unsigned char quantidadeBitcoins = (unsigned char) (1+ (genRandLong(&r)%50));

            blocoAMinerar.data[j] = endOrigem; //end de origem	
            blocoAMinerar.data[j+1] = endDestino; //end de destino
            blocoAMinerar.data[j+2] = quantidadeBitcoins; //quantidade de transações

            endOrg = blocoAMinerar.data[j];
            endDest = blocoAMinerar.data[j+1];
            //debitar do endereco
            carteira[endOrg] = carteira[endOrg] - blocoAMinerar.data[j+2];
            aux = carteira[endOrg];
            if(aux<0){
                carteira[endOrg]=0;
            }
            aux = 0;
            //creditar no endereco
            carteira[endDest] = carteira[endDest] + blocoAMinerar.data[j+2];
            j = j + 2;	//avança o for
		}
        //função para criptografar o código                            hash que indica se minerou
        SHA256((unsigned char *)&blocoAMinerar, sizeof(blocoAMinerar), auxHashAtual);
        while(auxHashAtual[0] != 0 || auxHashAtual[1]!= 0 || auxHashAtual[2]!= 0){
            if(blocoAMinerar.nonce < 4294967295){
                blocoAMinerar.nonce += 1; //soma 1 ao nonce cada vez que o calcula o hash
			    SHA256((unsigned char *)&blocoAMinerar, sizeof(blocoAMinerar), auxHashAtual);
            }else{
                controle ++;
                break;
            }
	    }
        if(controle<0){
            //reinicia o nonce
            blocoAMinerar.nonce = 0;
            while(auxHashAtual[0] != 0 || auxHashAtual[1]!= 0){
                blocoAMinerar.nonce += 1; //soma 1 ao nonce cada vez que o calcula o hash
			    SHA256((unsigned char *)&blocoAMinerar, sizeof(blocoAMinerar), auxHashAtual);
            }
	    }
        controle = 0;
        //     guarda nessa var o valor do hash anterior
        copiaHash(auxHashAnterior, auxHashAtual);
        //apos a mineracao, passando os dados do bloco não minerado pro minerado
        blocoMinerado.bloco.numero = blocoAMinerar.numero;
        blocoMinerado.bloco.nonce = blocoAMinerar.nonce;
        copiaData(blocoMinerado.bloco.data, blocoAMinerar.data);
        copiaHash(blocoMinerado.bloco.hashAnterior, blocoAMinerar.hashAnterior);
        copiaHash(blocoMinerado.hash, auxHashAtual);
    
        file = fopen("blocosMinerados.bin", "a+b");
        if(file == NULL){
            printf("\tArquivo não pode ser aberto!\n");
            exit(0);
        }
        fwrite(&(blocoMinerado), sizeof(struct BlocoMinerado), 1, file);
        if(ferror(file)){
            printf("Erro!\n");
        }else{
            printf("Tudo certo!-arqBin\n");
        }
        fclose(file);
    }
    
    opcao = -1;
    do{
		printf("\t--- OPÇÕES ---\n");
        printf("\t1- IMPRIMIR OS DADOS DE UM BLOCO\n");
		printf("\t2- INFORMAR QUANT BITCOINS DE UM ENDEREÇO\n");
        printf("\t3- VERIFICAR QUAL END POSSUÍ MAIS BITCOINS\n");
        printf("\t4- LISTAR END COM QUANT DE BITCOINS EM ORDEM CRESCENTE\n");
        printf("\t5- IMPRIMIR TODOS OS BLOCOS\n");
		printf("\t0- ENCERRAR PROGRAMA\n");
		printf("Insira a opção escolhida: ");
		scanf("%d", &opcao);

		switch(opcao){
			int pesq, result;
			case 1:
				printf("\tIMPRIMIR OS DADOS DE UM BLOCO\n");
				printf("\tDigite o número do Bloco desejado: ");
                scanf("%d", &pesq);
                buscaBloco(pesq, transBlocos);
				break;
			
			case 2:
				printf("\n\tINFORMAR QUANT BITCOINS DE UM ENDEREÇO\n");
				printf("\tDigite o endereço que deseja pesquisar: ");
				scanf("%d", &pesq);
                result = buscaQuantBitcoinEnd(carteira, pesq);
                printf("\tEndereço %d -> Bitcoins: %d\n", pesq, result);
				break;

            case 3:
                printf("\n\tVERIFICAR QUAL END POSSUÍ MAIS BITCOINS\n");
                verificaMaiorBitcoinEnd(carteira);
                break;
            
            case 4:
                printf("\n\tLISTAR END COM QUANT DE BITCOINS EM ORDEM CRESCENTE\n");
                ordenarEnderecos(carteira);
                break;
            
            case 5:
				printf("\n\tIMPRIMIR TODOS OS BLOCOS\n");
				imprimirTodosBlocosMinerados(transBlocos);
				break;

			case 0:
				printf("\tPrograma Encerrado!\n");
				break;

			default:
				printf("Erro!");
				break;
		}

	}while(opcao!=0);

    return 0;
}