/*
 *   Execício 3 - Lab_ED2 - DIG-MOLGRE-SQL 
 *   Autores:
 *   Gustavo Molina
 *   Gabriel Vinícius
 *   Luis Marcello 
 *   Pedrenrique Gonçalves
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <locale.h>

//Variáveis de controle
int proGlobal = 2;//HASH
//int proLocal = 2;//HASH
int static proMax = 4;//LIMITANTE
int dirty = -1;//VARIÁVEIS ALEATÓRIAS
int pin_count = -1;//VARIÁVEIS ALEATÓRIAS
int clientes = 0;//CONTA QUANTOS CLIENTES EXISTEM NO BANCO
FILE *arq;

//itoa(i,novoElemento->h,2); converte para binário

int convertBinaryToDecimal(int n){
    int decimalNumber = 0, i = 0, remainder;
    while (n!=0)
    {
        remainder = n%10;
        n /= 10;
        decimalNumber += remainder*pow(2,i);
        ++i;
    }
    return decimalNumber;
}

typedef struct cliente{//Estrutura auxiliar
	char nome[50];
	int conta, CPF,linha,valido;
	float limite, saldo;
	struct cliente *prox;
	struct cliente *ante;
}CLI;

typedef struct listaCliente{
	CLI *inicio;
	CLI *fim;
}LISTA;

typedef struct registro{// é o que fica nos buckets
	int id;//relaciona o registro com o cliente por meio do CPF
	int linha;//linha no arquivo de texto
	struct registro *prox;
	struct registro *ante;
}REG;

//permite uso de LISTAS simultâneas
typedef struct bucket {
    REG *inicio;
    REG *fim;
    //int profLocal;
}BUCKET;// funcionará como página

typedef struct no_diretorio{
	int profLocal;// é referente ao que ele está apontando!
	int posi;
	REG *apontaComeco;
	struct no_diretorio *prox;
	struct no_diretorio *ante;
}DIR;

typedef struct disco{//controla a estrutura "princiapl" da Tabela Hash
	DIR *inicio;
	DIR *fim;
}DISCO;

/*
BUCKET *criaBucket() {//inicializa um bucket ou página
    BUCKET *b = (BUCKET *) malloc (sizeof(BUCKET));
    b->fim = NULL;
    b->inicio = NULL;
    b->profLocal = 2;
    return b;
}
*/

LISTA *criaLista(){
	LISTA *l = (LISTA *)malloc(sizeof(LISTA));
	l->fim = NULL;
	l->inicio = NULL;
	return l;
}

LISTA *insereLista(LISTA *l, CLI *c ){//lógica de fila para esta inserção
	CLI *novo = c;
	if(l->inicio==NULL){
		novo->prox = NULL;
		novo->ante = NULL;
		l->inicio = novo;
		l->fim = novo;
	}else{
		novo->prox = NULL;
		novo->ante = l->fim;
		l->fim->prox = novo;
		l->fim = novo;
	}
	
	return l;
}


LISTA *removeLista(LISTA *l, CLI *c){
	if(l->inicio==NULL){
		printf("\nImpossível remover\n");
		system("pause");
		return l;	
	}else{
		CLI *aux = l->inicio;
		while( aux!=NULL ){
			if(aux->CPF==c->CPF){
				printf("\nElemento a ser removido encontrado!\n");
				break;
			}else
			aux = aux->prox;
		}
		if(l->inicio==l->fim){// 1 elemento
			l->inicio = NULL;
			l->fim = NULL;
		}else{
			if(aux == l->inicio){
				l->inicio = l->inicio->prox;
				l->inicio->ante = NULL;
				aux = NULL;
			}else{
				if(aux == l->fim){
					l->fim = l->fim->ante;
					l->fim->prox = NULL;
				}else{
					aux->ante->prox = aux->prox;
					aux->prox->ante = aux->ante;		
				}	
				aux->prox = NULL;
				aux->ante = NULL;
			}	
		}
		free(aux);
	}
	return l;
}

void consultaLista(LISTA *l){
	printf("\nDados em DISCO:\n");
	CLI *aux = l->inicio;
	printf("\n--------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("Nome:\t");
	while(aux!=NULL){
		printf("%s\t", aux->nome);
		aux = aux->prox;
	}
	printf("\n");
	aux = l->inicio;
	printf("Conta:\t");
	while(aux!=NULL){
		printf("%d\t", aux->conta);
		aux = aux->prox;
	}
	printf("\n");
	aux = l->inicio;
	printf("CPF:\t");
	while(aux!=NULL){
		printf("%d\t", aux->CPF);
		aux = aux->prox;
	}
	printf("\n");
	aux = l->inicio;
	printf("Saldo:\t");
	while(aux!=NULL){
		printf("%3.2f\t", aux->saldo);
		aux = aux->prox;
	}
	printf("\n");
	aux = l->inicio;
	printf("Limite:\t");
	while(aux!=NULL){
		printf("%3.2f\t", aux->limite);
		aux = aux->prox;
	}
	printf("\n");
	aux = l->inicio;
	printf("Válido:\t");
	while(aux!=NULL){
		printf("%d\t", aux->valido);
		aux = aux->prox;
	}
	printf("\n");
	aux = l->inicio;
	printf("Linha:\t");
	while(aux!=NULL){
		printf("%d\t", aux->linha);
		aux = aux->prox;
	}
	printf("\n--------------------------------------------------------------------------------------------------------------------------------------------\n");
	free(aux);
}


//Função que cria o ÚNICO DIRETÓRIO do programa.
//Inicialmente, como a profundidade é dois, então ele 4 nós iniciais
DISCO *criaDiretorio(){
	DISCO *d = (DISCO *) malloc (sizeof(DISCO));
	
	DIR *primeiro = (DIR *) malloc (sizeof(DIR));
	DIR *segundo = (DIR *) malloc (sizeof(DIR));
	DIR *terceiro = (DIR *) malloc (sizeof(DIR));
	DIR *quarto = (DIR *) malloc (sizeof(DIR));
	
	primeiro->apontaComeco = NULL;
	primeiro->profLocal = 2;
	primeiro->posi = 0;
	
	segundo->apontaComeco = NULL;
	segundo->profLocal = 2;
	segundo->posi = 1;
	
	terceiro->apontaComeco = NULL;
	terceiro->profLocal = 2;
	terceiro->posi = 2;
	
	quarto->apontaComeco = NULL;
	quarto->profLocal = 2;
	quarto->posi = 3;
	
	d->inicio= primeiro;
	d->fim = quarto;
	
	primeiro->ante = NULL;
	primeiro->prox = segundo;
	
	segundo->ante = primeiro;
	segundo->prox = terceiro;
	
	terceiro->ante = segundo;
	terceiro->prox = quarto;
	
	quarto->ante = terceiro;
	quarto->prox = NULL;
	
	return d;
}

DISCO *preencheTabela(DISCO *d, LISTA *l){
	
	if(l->inicio == NULL){
		printf("\n Nada a ser inserido na tabela \n");
		system("pause");
		return d;
	}else{
		CLI *aux1 = l->inicio;
		if(aux1->valido != 0){
			printf("\n aux1->valido %d\n",aux1->valido);
			system("pause");
			while(aux1!=NULL){
		
				REG *registro = (REG *)malloc(sizeof(REG));// é o nó que será inserido na tabela
				registro->id = aux1->CPF;
				registro->linha = aux1->linha;
				int x;
				int k;
				x = pow(2,proGlobal);// pra ver qual o tamanho do diretório
				k = registro->id%x;// função HASH para criar espalhamento
	
				DIR *dir = d->inicio;
				while(dir!=NULL){
					if(dir->posi == k){
						//printf("\n espalhamento:%d \n", k);
						//printf("\nAchei!\n");
						//system("pause");
						break;
					}else
					dir = dir->prox;
				}
		
			//aux é o bloco do tipo DIR que aponta para o bucket	

			if(dir->apontaComeco == NULL){
				dir->apontaComeco = registro;
				registro->prox = NULL;
				registro->ante = NULL;
				//printf("\n Se o DIR em questão não apontar para nada inicialmente \n");
				//system("pause");
			}else{
				REG *aux2 = dir->apontaComeco;//aux2 percorre os REGISTROS dentro dos buckets
				int soma = 0;
				//printf("\n Se o DIR em questão APONTAR para nada inicialmente \n");
				//system("pause");
				while(aux2!=NULL){
					soma++;
					aux2= aux2->prox;
					//printf("\nSoma:%d\n",soma);
					//system("pause");
				}
				if(soma==4){//não cabe
					//vai dar trampo
				//	printf("\nExpandindo Tabela Hash!\n");
				//	system("pause");
				}else{//quer dizer que tem 3, 2 ou 1 elemento
					aux2 = dir->apontaComeco;
					while(aux2->prox!=NULL){//encontrar o último registro do DIR específico
					aux2= aux2->prox;
					}
					aux2->prox = registro;
					registro->ante = aux2;
					registro->prox = NULL;
				//	printf("\n Adicionando elemento no fim do bucket \n");
				//	system("pause");
				}
			}
				aux1 = aux1->prox;
			}//while
		}else{
			printf("\n Não inserir este!\n");
		}
	}
	
	return d;
}

LISTA* leArquivo(LISTA *l, DISCO *d){
	char nome[50];
	int conta, CPF, valido, linha;
	float limite,saldo;
	printf("\nLendo DIGMOL-SQL...\n");
	while (!feof(arq)){    //percorre o arquivo todo
	   //Nome:Conta  CPF  Limite  Saldo  valido:Linha
	   
	CLI *novo = (CLI *) malloc (sizeof(CLI));
	   
	fflush(stdin);
	fscanf(arq, "%[^:]:%d:%d:%f:%f:%d:%d\n", nome,&conta,&CPF,&limite,&saldo,&valido,&linha);
    printf("\n nome :%s\n", nome);
    printf("\n conta:%d \n", conta);
    printf("\n CPF:%d \n", CPF);
    printf("\n limite:%f \n", limite);
    printf("\n saldo:%f \n", saldo);
	printf("\n valido:%d \n", valido);
    printf("\n linha:%d \n", linha);
    novo->conta = conta;
    novo->CPF = CPF;
	novo->limite = limite;
    novo->linha = linha;
	novo->saldo = saldo;
	novo->valido = valido;
	fflush(stdin);
    strcpy(novo->nome , nome);
	l = insereLista(l,novo);
	if(valido){
		clientes++;
	}
       
    }
    printf("\n clientes:%d \n", clientes);
    //chamar Aqui a função de preencher na HASH
    //preencheTabela(DISCO *d, LISTA *l,BUCKET *buc);
    system("pause");
	return l;
}

void salvaArquivo(LISTA *l){
	int ret;
	printf("\nSalvando no DIGMOL-SQL...\n");
	
	ret = remove("Banco.txt");//apaga o arquivo
	arq = fopen("Banco.txt","r+");//cria de novo
	
	CLI *aux = l->inicio;
	while(aux!=NULL){
		fprintf(arq, "%s:%d:%d:%f:%f:%d:%d\n", aux->nome,aux->conta,aux->CPF,aux->limite,aux->saldo,aux->valido,aux->linha);
		printf("%s:%d:%d:%f:%f:%d:%d\n", aux->nome,aux->conta,aux->CPF,aux->limite,aux->saldo,aux->valido,aux->linha);
		aux = aux->prox;	
	}
	printf("\nRetorno:%d \n", ret);
	system("pause");
}

DISCO* cadastrarCliente(DISCO *d, LISTA *l){//ARRUMAR
	char nome[50];
	int conta, CPF, valido;
	float limite,saldo;
	
	printf("\nDigite o nome:\n");
	fflush(stdin);
	scanf("%[^\n]", nome);
	printf("\nDigite o número da conta:\n");
	scanf("%d", &conta);
	printf("\n Digite o CPF:\n");
	scanf("%d", &CPF);
	printf("\nDigite o saldo:\n");
	scanf("%f", &saldo);
	printf("\nDigite o limite:\n");
	scanf("%f", &limite);
	
	CLI *novo = (CLI *) malloc (sizeof(CLI));
	
	if(novo == NULL){//verificação
		printf("\n Memória insuficiente.\n");
		exit(1);
	}
	
	novo->conta = conta;
    novo->CPF = CPF;
	novo->limite = limite;
    novo->linha = clientes+1;
	novo->saldo = saldo;
	novo->valido = 1;
	fflush(stdin);
    strcpy(novo->nome , nome);
	l = insereLista(l,novo);
	
	clientes++;
	
	//falta fazer altos bangs aqui
	
	REG *novo1 = (REG *)malloc(sizeof(REG));// é o nó que será inserido na tabela
	novo1->id = novo->CPF;
	novo1->linha = novo->linha;
	int x;
	int k;
	x = pow(2,proGlobal);// pra ver qual o tamanho do diretório
	k = novo1->id%x;// função HASH para criar espalhamento
	
	DIR *aux = d->inicio;
	while(aux!=NULL){
		if(aux->posi == k){
			//printf("\n espalhamento:%d \n", k);
			//printf("\nAchei!\n");
			//system("pause");
			break;
		}else
			aux = aux->prox;
	}
	//printf("\n aushduashdiashud \n");
	//system("pause");
	
	//Até aqui OK
	//aux é o bloco do tipo DIR que aponta para o bucket

	if(aux->apontaComeco == NULL){
		aux->apontaComeco = novo1;
		novo1->prox = NULL;
		novo1->ante = NULL;
		printf("\n Se o DIR em questão não apontar para nada inicialmente \n");
		system("pause");
	}else{
		REG *aux1 = aux->apontaComeco;//aux1 percorre os REGISTROS dentro dos buckets
		int soma = 0;
		printf("\n Se o DIR em questão APONTAR para nada inicialmente \n");
		system("pause");
		while(aux1!=NULL){
			soma++;
			aux1= aux1->prox;
			printf("\nSoma:%d\n",soma);
			system("pause");
		}
		if(soma==4){//não cabe
			//vai dar trampo
			printf("\nExpandindo Tabela Hash!\n");
			system("pause");
		}else{//quer dizer que tem 3, 2 ou 1 elemento
			aux1 = aux->apontaComeco;
			
			while(aux1->prox!=NULL){//encontrar o último registro do DIR específico
			aux1= aux1->prox;
			}
			aux1->prox = novo1;
			novo1->ante = aux1;
			novo1->prox = NULL;
			printf("\n Adicionando elemento no fim do bucket \n");
			system("pause");
		}
	}
	
	printf("\nFim da insere:\n");
	system("pause");
	return d;
}

DISCO* removeCliente(DISCO *l){//ARRUMAR
	//aqui também
	
	return l;
}

void consultaCliente(DISCO* l){
	int aux_cpf;
	printf("\nDigite o CPF para buscar:\n");
	scanf("%d", &aux_cpf);
}

void exibirHash(DISCO *tabela){//ARRUMAR
	DIR *aux1 = tabela->inicio;
	printf("\n \t ---Profundidade Global: %d --- \t\n", proGlobal);
	printf("\n--------------------------------------------------------------------------------------------------------------------------------------------\n");
	if(aux1==NULL){
		printf("\nNada ainda\n");
	}else{
		printf("\n Tem algo! \n");
		while(aux1!=NULL){
			REG *aux2 = aux1->apontaComeco;
			if(aux2==NULL){
				printf("\t Bucket: %d \n Profundidade Local: %d ",aux1->posi, aux1->profLocal);
			}
			else{
				printf("\t Bucket: %d \n Profundidade Local: %d ",aux1->posi, aux1->profLocal);
				while(aux2!=NULL){
					printf("\t");
					printf(">>>CPF:%d\t",aux2->id);
					printf(">>>RID:%d\t",aux2->linha);
					aux2 = aux2->prox;
				}
			}
			printf("\n");
			aux1 = aux1->prox;
		}
	}
	printf("\n--------------------------------------------------------------------------------------------------------------------------------------------\n");
}

void limpaDISCO(DISCO *d) {
    DIR *aux;
    while (d->inicio != NULL) {
        aux = d->inicio;
        d->inicio = d->inicio->prox;
        free(aux);
    }
}

void limpaLISTA(LISTA *l) {
    CLI *aux;
    while (l->inicio != NULL) {
        aux = l->inicio;
        l->inicio = l->inicio->prox;
        free(aux);
    }
}


float teste;

int main (){
	setlocale(LC_ALL, "Portuguese");
	int opc=0;
	
	LISTA *l = criaLista();
	
	DISCO *tabela = criaDiretorio();
	
	//BUCKET *bucketGenerico = criaBucket();//Acho que terei que remover isso
	
	/*
	DIR *aux;
	aux = tabela->inicio;
	while(aux!=NULL){
		printf("\nPosição Tabela: %d\n", aux->posi);
		aux = aux->prox;
	}
	system("pause");
	free(aux);
	*/
	
	arq = fopen("Banco.txt","r+");//abre o arquivo e irá sobrescrever
	if( arq == NULL){
		printf("\n Não foi possível abrir o arquivo.\n");
		system("pause");
		exit(1);
	}
	
	l = leArquivo(l, tabela);//OK
	
	tabela = preencheTabela(tabela,l);//Depois de ler, vamos preencher a Tabela com as informações do txt
	
	while(opc!=-1){
		system("cls");
		printf("\n BEM-VINDO AO DIGMOL-SQL! \n");
		
		exibirHash(tabela);
		consultaLista(l);
		
		printf("\n\t---MENU---\t\n");
		printf("\n1 para cadastrar cliente\n");
		printf("\n2 para remover cliente \n");
		printf("\n3 para consultar cliente\n");
		printf("\n4 para visualizar Tabela Hash \n");
		printf("\n5 para salvar em disco \n");
		printf("\n6 para visualizar LISTA auxiliar \n");
		printf("\n-1 para sair \n");
		scanf("%d", &opc);
	
		switch (opc){
			case 1:
				printf("\n cadastrar cliente\n");
				tabela = cadastrarCliente(tabela, l);
				system("pause");
				break;
			case 2:
				printf("\n remover cliente \n");
				
				system("pause");
				break;
			case 3:
				printf("\n consultar cliente\n");
				
				system("pause");
				break;
			case 4:
				printf("\n visualizar Tabela Hash \n");
				exibirHash(tabela);
				system("pause");
				break;
			case 5:
				printf("\n Salvando em disco \n");
				salvaArquivo(l);
				break;
			case 6:
				consultaLista(l);
				system("pause");
				break;
			case -1:
				printf("\nSaindo...\n");
				fclose (arq);
				limpaDISCO(tabela);
				limpaLISTA(l);
				exit(1);
				break;
			default :
				printf("\nEntrada inválida!\n");
				system("pause");
				break;
		}
	
	}
	
	return 0;
}//SGBD do Sucesso
