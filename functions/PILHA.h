#ifndef PILHA_H_INCLUDED
#define PILHA_H_INCLUDED

/* FUNÇÕES DE MANIPULAÇÃO DE PILHA

Pilha* CriaPilha ()     CRIA A PILHA

No* ins_ini (No* t, int a)  AUXILIAR DA INSERÇÃO

void push (Pilha* p, int v) INSERÇÃO

No* ret_ini (No* l)         AUXILIAR DA REMOÇÃO

int pop (Pilha* p)          REMOVE RETORNANDO O VALOR QUE FOI REMOVIDO

void libera (Pilha* p)      LIBERA A PILHA

void imprime (Pilha* p)     IMPRIMA A PILHA

int vaziaPilha(Pilha *p)	    VERIFICA SE A PILHA ESTÁ VAZIA, RETORNA 1

*/

typedef struct no
{
    int info;
    struct no *prox;
}No;

typedef struct pilha
{
   No *Topo;
}Pilha;

Pilha* CriaPilha (void)
{
     Pilha *p;
     p=(Pilha*)malloc(sizeof(Pilha));
     p->Topo = NULL;
   return p;
};

No* ins_ini (No* t, int a)
{
    No* aux = (No*) malloc(sizeof(No));
    aux->info = a;
    aux->prox = t;
    return aux;
};

void push (Pilha* p, int v)
{
    p->Topo = ins_ini(p->Topo,v);
};

No* ret_ini (No* l)
{
    No* p = l->prox;
    free(l);
    return p;
};

int pop (Pilha* p)
{
    int v;
    if (p==NULL)
    {
        printf("Pilha vazia.\n");
        exit(1); /* aborta programa */
    }
    v = p->Topo->info;
    p->Topo = ret_ini(p->Topo);
    return v;
};

Pilha * libera (Pilha* p)
{
    No *q = p->Topo;
    while (q!=NULL)
    {
        No* t = q->prox;
        free(q);
        q = t;
    }
    free(p);
    return(NULL);
};

void imprimePilha (Pilha* p)
{
    No* q;
    if(!p)
    {
        printf("\n\n\tPILHA VAZIA!!!!\n\n");
    }
    else
    {
        printf("\n\n\tImpressão da Pilha: ");
        for (q=p->Topo; q!=NULL; q=q->prox)
        {
            printf("% 2d",q->info);
        }
        printf("\n\n");
    }


};

int vaziaPilha(Pilha *p)
{
    if (p->Topo==NULL)
    {
        return 1; //pilha vazia
    }
    return 0;
}



#endif // PILHA_H_INCLUDED
