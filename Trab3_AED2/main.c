#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 10
#define ORDEM 10
#define TRUE 1
#define FALSE 0

//Aluno:LUCAS DOS ANJOS MORAES
//Matrícula:21453683

char urlInserir[]="valoresInserir.txt";
char urlRemover[]="valoresRemover.txt";

//Criação de estruturas
//Lista para leitura de arquivo
typedef struct tipoNoSimples{
    int chave;
    struct tipoNoSimples *anterior;
    struct tipoNoSimples *prox;
}tipoNoSimples;

typedef struct tipoLista{
    struct tipoNoSimples *inicio;
}tipoLista;

//B+
typedef struct tipoNo{
    int chaves[ORDEM+1];//Não é ordem-1 pq o vetor tem uma posição a mais para servir como contador
    struct tipoNo *filhos[ORDEM+1];
    struct tipoNo *prox;
}tipoNo;

typedef struct BPlus{
    struct tipoNo *raiz;
}BPlus;

//Criação de nós
tipoNoSimples* criaNoSimples(int valor){
    tipoNoSimples *novoNo=(tipoNoSimples*)malloc(sizeof(tipoNoSimples));
    novoNo->chave=valor;
    novoNo->prox=NULL;
    novoNo->anterior=NULL;
    return novoNo;
}

tipoLista* criarLista(){
    tipoLista *Lista;
    Lista=(tipoLista*)malloc(sizeof(tipoLista));
    Lista->inicio=NULL;
    return Lista;
}

BPlus* criaBPlus(){
    BPlus *Arvore;
    Arvore=(BPlus*)malloc(sizeof(BPlus));
    Arvore->raiz=NULL;
    return Arvore;
}

tipoNo* criaNo(int valor, int posicao){
    int contador=0;
    tipoNo *novoNo=(tipoNo*)malloc(sizeof(tipoNo));
    novoNo->chaves[posicao]=valor;
    novoNo->chaves[0]=1;
    while(contador<ORDEM+1){
        novoNo->filhos[contador]=NULL;
        contador++;
    }
    return novoNo;
}

//Funções de listas simples
int verificaVazia(tipoLista *Lista){
    if(Lista->inicio==NULL){
        return TRUE;
    }else{return FALSE;};
}

void insereNaLista(tipoLista *Lista,tipoNoSimples *No){
    tipoNoSimples *Auxiliar = Lista->inicio;
    if (verificaVazia(Lista)==TRUE){
        Lista->inicio = No;
    }else{
        while(Auxiliar->prox!=NULL){
            Auxiliar = Auxiliar->prox;
        }
        Auxiliar->prox = No;
        No->anterior = Auxiliar;
    }
}

void recebeDados(tipoLista *Lista, char url[], int tamLinha){
    char linha[tamLinha];
    FILE *arq;
    char *dados;
    int valores;
    int atoi();
    
    arq=fopen(url, "r");
    if(arq==NULL){
        printf("\nErro, não foi possível abrir o arquivo!\n");
    }else{
        while((fgets(linha, tamLinha, arq))!=NULL){
            dados=strtok(linha,";");
            valores=atoi(dados);
            insereNaLista(Lista, criaNoSimples(valores));
        }
    }
    fclose(arq);
}

//Funções da B+
tipoNo* BuscaNoReceptor(tipoNo* Raiz, int chave){
    int contador=0;
    tipoNo* aux=Raiz;
    if(Raiz==NULL){
        return NULL;
    }else{
        if(Raiz->chaves[0]<=(ORDEM) && Raiz->filhos[0]==NULL){
            return Raiz;
        }else{
            while(chave>aux->chaves[contador+1] && contador!=ORDEM){
                contador++;
            }
            return BuscaNoReceptor(aux->filhos[contador], chave);
        }
    }
}

int HaEspaco(tipoNo* No){
    if(No->chaves[0]<(ORDEM-1)){
        return TRUE;
    }else{return FALSE;}
}

tipoNo* InsereOrdenado(tipoNo* no, int chave){
    int contador=1;
    int auxiliar;
    while(chave>no->chaves[contador] && contador<=no->chaves[0]){
        contador++;
    }
    if((contador)==no->chaves[0]){
        no->chaves[contador+1]=chave;
    }else{
        auxiliar=(no->chaves[0]+1);
        while(auxiliar>contador){
            no->chaves[auxiliar]=no->chaves[auxiliar-1];
            auxiliar--;
        }
        no->chaves[contador]=chave;
    }
    no->chaves[0]++;
    return no;
}

int PegaMenor(int qtdNo, int qtdRaiz){
    if(qtdNo<=qtdRaiz){
        return qtdNo;
    }else{
        return qtdRaiz;
    }
}

int SaoIguais(tipoNo* No, tipoNo* Raiz){
    int qtdNo,qtdRaiz,limite;
    int qtdChavesIguais=0;
    qtdNo=No->chaves[0];
    qtdRaiz=Raiz->chaves[0];
    limite=PegaMenor(qtdNo, qtdRaiz);
    while(No->chaves[qtdChavesIguais]==Raiz->chaves[qtdChavesIguais] && qtdChavesIguais<=limite){
        qtdChavesIguais++;
    }
    if(qtdChavesIguais==limite){
        return TRUE;
    }else{return FALSE;}
}

tipoNo* transfereChaves(tipoNo* NoQuebra, int posicao){
    int contador=1;
    int indice=posicao;
    tipoNo* novoNo=criaNo(NoQuebra->chaves[indice],contador);
    if(NoQuebra->chaves[0]>3){
        while(indice<=NoQuebra->chaves[0]){
            indice++;
            contador++;
            novoNo->chaves[contador]=NoQuebra->chaves[indice];
        }
    }
    novoNo->chaves[0]=contador;
    NoQuebra->chaves[0]=2;
    if(NoQuebra->prox!=NULL){
        novoNo->prox=NoQuebra->prox;
    }
    NoQuebra->prox=novoNo;
    return NoQuebra;
}

tipoNo* Quebra(tipoNo* NoQuebra){
    int contador=3;
    int limite=NoQuebra->chaves[0];
    tipoNo* auxiliar=criaNo(0,0);
    auxiliar->prox=NoQuebra;
    NoQuebra=transfereChaves(NoQuebra, contador);
    limite=limite-2;
    while(contador<=limite){
        NoQuebra=NoQuebra->prox;
        NoQuebra=transfereChaves(NoQuebra, contador);
        limite=limite-2;
    }
    return auxiliar->prox;
}

tipoNo* NovaRaiz(tipoNo* Raiz){
    int contador=2;
    tipoNo* aux=Raiz->prox;
    tipoNo* novaRaiz=criaNo(Raiz->chaves[Raiz->chaves[0]],1);//Passando o último elemento do vetor na cria No
    novaRaiz->filhos[0]=Raiz;
    while(aux->prox!=NULL){
        Raiz=Raiz->prox;
        aux=Raiz->prox;
        novaRaiz->chaves[contador]=Raiz->chaves[Raiz->chaves[0]];
        novaRaiz->filhos[contador-1]=Raiz;
        contador++;
    }
    return novaRaiz;
}

tipoNo* Split(tipoNo* NoQuebra, tipoNo* raiz, int chave){
    tipoNo* noPai;
    tipoNo* aux;
    if(SaoIguais(NoQuebra, raiz)==TRUE){
        NoQuebra=Quebra(NoQuebra);
        raiz=NovaRaiz(raiz);
        return raiz;
    }else{//2 tipos
        //noPai=BuscaPai(NoQuebra, raiz);
        aux=noPai->filhos[0];
        NoQuebra=InsereOrdenado(NoQuebra, chave);
        NoQuebra=Quebra(NoQuebra);
        if(SaoIguais(noPai,raiz)==TRUE){
            //            noPai=atualizaPai(NoQuebra, noPai);//atualiza com o valor inserido
            //            noPai=eliminaHerdeiros(noPai);//tira os filhos e retorna o no pai sem os filhos
            //            noPai=Quebra(noPai);
            //            noPai=atualizaFilhos(noPai, aux);//coloca os filhos e retorna o no pai que é cabeça
            //            return NovaRaiz(noPai);
        }else{
            //aí fudeu
            //por enquanto a linha de return(246) pode ser a unica que fica no if acima
            //mas resta tratar esse else pq eu tava fazendo b+ sem o pai cheio
        }
    }
    return raiz;
}

BPlus* InsereNaBPlus(BPlus* BPlus, int chave){
    //tratar inserção de números ja existentes
    tipoNo* noDisponivel=BuscaNoReceptor(BPlus->raiz, chave);
    if(noDisponivel==NULL){
        BPlus->raiz=criaNo(chave, 1);
        return BPlus;
    }
    if(HaEspaco(noDisponivel)==TRUE){
        noDisponivel=InsereOrdenado(noDisponivel, chave);
        return BPlus;
    }
    noDisponivel=Split(noDisponivel, BPlus->raiz, chave);
    return InsereNaBPlus(BPlus, chave);
}

void imprimeArvore(BPlus* BPlus){
    tipoNo* auxiliar=BPlus->raiz;
    int chave, qtdChaves,contador=1;
    
    while(contador<=(auxiliar->chaves[0])){
        chave=auxiliar->chaves[contador];
        printf("chave %d tem valor %d\n",contador, chave);
        contador++;
    }
    qtdChaves=auxiliar->chaves[0];
    printf("Quantidade de chaves do vetor igual a:%d\n",qtdChaves);
}


/*___________________________________________________
 
 
                      MEUS CÓDIGOS
 
 ____________________________________________________
 */

tipoNo* buscaNoVetor(tipoNo* raiz, int chave, int contador)
{
    if (raiz == NULL)
    {
        return NULL;
    }
    
    if (chave == raiz->chaves[contador])
    {
        return raiz;
    }
    
    if (contador == raiz->chaves[0] && raiz->filhos[0] == NULL)
    {
        return NULL;
    }
    
    else
    {
        if (chave < raiz->chaves[contador])
        {
            return buscaNoVetor(raiz->filhos[contador-1], chave, 1);
        }
        
        contador++;
        if (contador > raiz->chaves[0])
            return buscaNoVetor(raiz->filhos[9], chave, 1);
        
        return buscaNoVetor(raiz, chave, contador);
    }
}

void removedoVetor(int chaves[], int chave, int contador)
{
    while (contador < chaves[0])
    {
        chaves[contador] = chaves[contador+1];
        contador++;
    }
    chaves[0]--;
}


tipoNo* casoUm(tipoNo* raiz, int chave)
{
    if (raiz->filhos[0] == NULL)
    {
        int contador = 1;
        
        while (contador <= raiz->chaves[0])
        {
            if (chave == raiz->chaves[contador])
            {
                removedoVetor(raiz->chaves, chave, contador);
            }
            else
                contador++;
        }
    }
    return raiz;
}

tipoNo* buscaNo(tipoNo*raiz, int valorZero, int contador)
{
    if (valorZero < raiz->chaves[contador])
    {
        if (valorZero == raiz->filhos[contador-1]->chaves[1])
        {
            return raiz->filhos[contador-1];
        }
        return buscaNo(raiz->filhos[contador-1], valorZero, 1);
    }
    else
    {
        contador++;
        return buscaNo(raiz, valorZero, contador);
    }
    
}

tipoNo* concatenaDuasPaginas(tipoNo* raiz)
{
    //tem q encontrar o pai
}

tipoNo* casoDois(tipoNo* raiz, int chave)
{
    
    
    return raiz;
}



BPlus* Remove(BPlus* BPlus, int chave)
{
    tipoNo* raiz = buscaNoVetor(BPlus->raiz, chave, 1);
    
    if (BPlus->raiz == NULL)
    {
        printf("A árvore está vazia!");
        return NULL;
    }
    
    
    return BPlus;
}







int main(void){
    //int opcao;
    tipoLista *valoresInserir=criarLista();
    tipoLista *valoresRemover=criarLista();
    BPlus *arvoreBPlus=criaBPlus();
    
    recebeDados(valoresInserir, urlInserir, MAX);
    recebeDados(valoresRemover, urlRemover, MAX);
    
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 50);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 100);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 40);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 15);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 34);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 142);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 1594);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 23);
    arvoreBPlus=InsereNaBPlus(arvoreBPlus, 91);
    imprimeArvore(arvoreBPlus);
    
    return 0;
}
