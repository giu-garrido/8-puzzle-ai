#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

#define N 3
#define EMBARALHAMENTOS 100
#define MAX_MOVIMENTOS 1000

// Estrutura para representar um estado do puzzle
typedef struct Estado {
    int tabuleiro[N][N];
    int vazio_x;
    int vazio_y;
    struct Estado* pai;
    char movimento;
    int profundidade;
} Estado;

// Estrutura para nó da lista encadeada
typedef struct No {
    Estado* estado;
    struct No* proximo;
} No;

// Estrutura para fila
typedef struct {
    No* inicio;
    No* fim;
    int tamanho;
} Fila;

// Estrutura para pilha
typedef struct {
    No* topo;
    int tamanho;
} Pilha;

// Estrutura para armazenar estados visitados
typedef struct {
    Estado** estados;
    int quantidade;
    int capacidade;
} EstadosVisitados;

// Funções auxiliares básicas
void imprimir_tabuleiro(int tabuleiro[N][N]) {
    printf("\n+---+---+---+\n");
    for (int i = 0; i < N; i++) {
        printf("|");
        for (int j = 0; j < N; j++) {
            if (tabuleiro[i][j] == 0) {
                printf("   |");
            } else {
                printf(" %d |", tabuleiro[i][j]);
            }
        }
        printf("\n+---+---+---+\n");
    }
}

void copiar_tabuleiro(int origem[N][N], int destino[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            destino[i][j] = origem[i][j];
        }
    }
}

// Funções da Fila
Fila* criar_fila() {
    Fila* f = (Fila*)malloc(sizeof(Fila));
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
    return f;
}

void enfileirar(Fila* f, Estado* estado) {
    No* novo = (No*)malloc(sizeof(No));
    novo->estado = estado;
    novo->proximo = NULL;

    if (f->inicio == NULL) {
        f->inicio = novo;
    } else {
        f->fim->proximo = novo;
    }
    f->fim = novo;
    f->tamanho++;
}

Estado* desenfileirar(Fila* f) {
    if (f->inicio == NULL) return NULL;

    No* no = f->inicio;
    Estado* estado = no->estado;
    f->inicio = no->proximo;
    f->tamanho--;

    if (f->inicio == NULL) f->fim = NULL;
    free(no);
    return estado;
}

void destruir_fila(Fila* f) {
    while (f->inicio != NULL) {
        desenfileirar(f);
    }
    free(f);
}

int fila_vazia(Fila* f) {
    return f->inicio == NULL;
}

// Funções da Pilha
Pilha* criar_pilha() {
    Pilha* p = (Pilha*)malloc(sizeof(Pilha));
    p->topo = NULL;
    p->tamanho = 0;
    return p;
}

void empilhar(Pilha* p, Estado* estado) {
    No* novo = (No*)malloc(sizeof(No));
    novo->estado = estado;
    novo->proximo = p->topo;
    p->topo = novo;
    p->tamanho++;
}

Estado* desempilhar(Pilha* p) {
    if (p->topo == NULL) return NULL;

    No* no = p->topo;
    Estado* estado = no->estado;
    p->topo = no->proximo;
    p->tamanho--;

    free(no);
    return estado;
}

void destruir_pilha(Pilha* p) {
    while (p->topo != NULL) {
        desempilhar(p);
    }
    free(p);
}

int pilha_vazia(Pilha* p) {
    return p->topo == NULL;
}

// Funções para gerenciar estados visitados
EstadosVisitados* criar_estados_visitados(int capacidade_inicial) {
    EstadosVisitados* ev = (EstadosVisitados*)malloc(sizeof(EstadosVisitados));
    ev->estados = (Estado**)malloc(sizeof(Estado*) * capacidade_inicial);
    ev->quantidade = 0;
    ev->capacidade = capacidade_inicial;
    return ev;
}

void destruir_estados_visitados(EstadosVisitados* ev) {
    for (int i = 0; i < ev->quantidade; i++) {
        free(ev->estados[i]);
    }
    free(ev->estados);
    free(ev);
}

void adicionar_estado_visitado(EstadosVisitados* ev, Estado* estado) {
    if (ev->quantidade >= ev->capacidade) {
        int nova_capacidade = ev->capacidade * 2;

        Estado** novos_estados = (Estado**)malloc(sizeof(Estado*) * nova_capacidade);

        for (int i = 0; i < ev->quantidade; i++) {
            novos_estados[i] = ev->estados[i];
        }

        free(ev->estados);

        ev->estados = novos_estados;
        ev->capacidade = nova_capacidade;
    }

    ev->estados[ev->quantidade++] = estado;
}


int estado_visitado(EstadosVisitados* ev, Estado* estado) {
    for (int i = 0; i < ev->quantidade; i++) {
        int igual = 1;
        for (int j = 0; j < N && igual; j++) {
            for (int k = 0; k < N && igual; k++) {
                if (estado->tabuleiro[j][k] != ev->estados[i]->tabuleiro[j][k]) {
                    igual = 0;
                }
            }
        }
        if (igual) return 1;
    }
    return 0;
}

// Funções do jogo
void trocar_posicao(int *pos1, int *pos2) {
    int aux = *pos1;
    *pos1 = *pos2;
    *pos2 = aux;
}

void embaralhar_tabuleiro(int tabuleiro[N][N], int *vazio_x, int *vazio_y) {
    // Inicializar com o estado objetivo
    int valor = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == N-1 && j == N-1) {
                tabuleiro[i][j] = 0;
                *vazio_x = i;
                *vazio_y = j;
            } else {
                tabuleiro[i][j] = valor++;
            }
        }
    }


    // Realizar movimentos aleatórios
    for (int i = 0; i < EMBARALHAMENTOS; i++) {
        int direcao = rand() % 4;
        int novo_x = *vazio_x;
        int novo_y = *vazio_y;

        switch (direcao) {
            case 0: if (novo_x > 0) novo_x--; break;
            case 1: if (novo_x < N-1) novo_x++; break;
            case 2: if (novo_y > 0) novo_y--; break;
            case 3: if (novo_y < N-1) novo_y++; break;
        }

        if (novo_x != *vazio_x || novo_y != *vazio_y) {
            trocar_posicao(&tabuleiro[*vazio_x][*vazio_y],
                          &tabuleiro[novo_x][novo_y]);
            *vazio_x = novo_x;
            *vazio_y = novo_y;
        }
    }
}

int eh_objetivo(int tabuleiro[N][N]) {
    int valor = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i == N-1 && j == N-1) {
                if (tabuleiro[i][j] != 0) return 0;
            } else {
                if (tabuleiro[i][j] != valor++) return 0;
            }
        }
    }
    return 1;
}

int mover(int tabuleiro[N][N], char direcao, int *vazio_x, int *vazio_y) {
    int novo_x = *vazio_x;
    int novo_y = *vazio_y;

    switch (direcao) {
        case 'W': case 'w': if (*vazio_x > 0) novo_x--; break;
        case 'S': case 's': if (*vazio_x < N-1) novo_x++; break;
        case 'A': case 'a': if (*vazio_y > 0) novo_y--; break;
        case 'D': case 'd': if (*vazio_y < N-1) novo_y++; break;
        default: return 0;
    }

    if (novo_x != *vazio_x || novo_y != *vazio_y) {
        trocar_posicao(&tabuleiro[*vazio_x][*vazio_y],
                      &tabuleiro[novo_x][novo_y]);
        *vazio_x = novo_x;
        *vazio_y = novo_y;
        return 1;
    }

    return 0;
}

// Funções da IA
char* obter_nome_movimento(char movimento) {
    switch (movimento) {
        case 'W': return "CIMA";
        case 'S': return "BAIXO";
        case 'A': return "ESQUERDA";
        case 'D': return "DIREITA";
        default: return "INICIAL";
    }
}

Estado* criar_estado(int tabuleiro[N][N], int profundidade, char movimento, Estado* pai) {
    Estado* estado = (Estado*)malloc(sizeof(Estado));
    copiar_tabuleiro(tabuleiro, estado->tabuleiro);
    estado->profundidade = profundidade;
    estado->movimento = movimento;
    estado->pai = pai;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (tabuleiro[i][j] == 0) {
                estado->vazio_x = i;
                estado->vazio_y = j;
                break;
            }
        }
    }

    return estado;
}

Estado* mover_vazio(Estado* atual, char direcao) {
    int novo_x = atual->vazio_x;
    int novo_y = atual->vazio_y;

    switch (direcao) {
        case 'W': if (novo_x <= 0) return NULL; novo_x--; break;
        case 'S': if (novo_x >= N - 1) return NULL; novo_x++; break;
        case 'A': if (novo_y <= 0) return NULL; novo_y--; break;
        case 'D': if (novo_y >= N - 1) return NULL; novo_y++; break;
        default: return NULL;
    }

    Estado* novo_estado = criar_estado(atual->tabuleiro, atual->profundidade + 1, direcao, atual);
    novo_estado->tabuleiro[atual->vazio_x][atual->vazio_y] =
        novo_estado->tabuleiro[novo_x][novo_y];
    novo_estado->tabuleiro[novo_x][novo_y] = 0;
    novo_estado->vazio_x = novo_x;
    novo_estado->vazio_y = novo_y;

    return novo_estado;
}

void imprimir_solucao(Estado* estado_final) {
    if (estado_final == NULL) return;

    int num_movimentos = 0;
    Estado* atual = estado_final;
    while (atual->pai != NULL) {
        num_movimentos++;
        atual = atual->pai;
    }

    Estado** caminho = (Estado**)malloc(sizeof(Estado*) * num_movimentos);
    atual = estado_final;
    for (int i = num_movimentos - 1; i >= 0; i--) {
        caminho[i] = atual;
        atual = atual->pai;
    }

    printf("\n=================================");
    printf("\n        SOLUÇÃO ENCONTRADA       ");
    printf("\n=================================");
    printf("\nNúmero de movimentos: %d\n", num_movimentos);

    printf("\nEstado Inicial:");
    imprimir_tabuleiro(atual);

    for (int i = 0; i < num_movimentos; i++) { //da pra mudar aqui p ler os char e ir movendo
        printf("\nPasso %d: Mover %s", i + 1, obter_nome_movimento(caminho[i]->movimento));
        imprimir_tabuleiro(caminho[i]->tabuleiro);
        printf("Profundidade: %d\n", caminho[i]->profundidade);
    }

    printf("\n=================================\n");
    free(caminho);
}

void busca_largura(int tabuleiro_inicial[N][N]) {
    Fila* fila = criar_fila();
    EstadosVisitados* visitados = criar_estados_visitados(1000);

    Estado* estado_inicial = criar_estado(tabuleiro_inicial, 0, '-', NULL);
    enfileirar(fila, estado_inicial);
    adicionar_estado_visitado(visitados, estado_inicial);

    printf("\nIniciando Busca em Largura...\n");

    while (!fila_vazia(fila)) {
        Estado* atual = desenfileirar(fila);

        if (eh_objetivo(atual->tabuleiro)) {
            printf("\nSolução encontrada!");
            printf("\nNós explorados: %d", visitados->quantidade);
            imprimir_solucao(atual);
            destruir_fila(fila);
            destruir_estados_visitados(visitados);
            return;
        }

        char movimentos[] = {'W', 'S', 'A', 'D'};
        for (int i = 0; i < 4; i++) {
            Estado* proximo = mover_vazio(atual, movimentos[i]);
            if (proximo != NULL && !estado_visitado(visitados, proximo)) {
                enfileirar(fila, proximo);
                adicionar_estado_visitado(visitados, proximo);
            } else if (proximo != NULL) {
                free(proximo);
            }
        }
    }

    printf("\nNão foi possível encontrar uma solução!");
    destruir_fila(fila);
    destruir_estados_visitados(visitados);
}

void busca_profundidade_limitada(int tabuleiro_inicial[N][N], int limite_profundidade, EstadosVisitados* visitados) {
    Pilha* pilha = criar_pilha();

    Estado* estado_inicial = criar_estado(tabuleiro_inicial, 0, '-', NULL);
    empilhar(pilha, estado_inicial);
    adicionar_estado_visitado(visitados, estado_inicial);

    while (!pilha_vazia(pilha)) {
        Estado* atual = desempilhar(pilha);

        if (eh_objetivo(atual->tabuleiro)) {
            printf("\nSolução encontrada na profundidade %d!", limite_profundidade);
            printf("\nNós explorados: %d", visitados->quantidade);
            imprimir_solucao(atual);
            destruir_pilha(pilha);
            return;
        }

        if (atual->profundidade < limite_profundidade) {
            char movimentos[] = {'W', 'S', 'A', 'D'};
            for (int i = 0; i < 4; i++) {
                Estado* proximo = mover_vazio(atual, movimentos[i]);
                if (proximo != NULL && !estado_visitado(visitados, proximo)) {
                    empilhar(pilha, proximo);
                    adicionar_estado_visitado(visitados, proximo);
                } else if (proximo != NULL) {
                    free(proximo);
                }
            }
        }
    }

    destruir_pilha(pilha);
}

void busca_profundidade_iterativa(int tabuleiro_inicial[N][N]) {
    EstadosVisitados* visitados = criar_estados_visitados(1000);
    printf("\nIniciando Busca em Profundidade Iterativa...\n");

    for (int profundidade = 0; profundidade <= 31; profundidade++) {
        printf("\nTentando profundidade %d...\n", profundidade);
        visitados->quantidade = 0;  // Reseta os estados visitados
        busca_profundidade_limitada(tabuleiro_inicial, profundidade, visitados);

        // Verifica se encontrou a solução
        if (visitados->quantidade > 0 &&
            eh_objetivo(visitados->estados[visitados->quantidade-1]->tabuleiro)) {
            destruir_estados_visitados(visitados);
            return;
        }
    }

    printf("\nNão foi possível encontrar uma solução!");
    destruir_estados_visitados(visitados);
}

// Modo de jogo manual
void modo_jogo() {
    int tabuleiro[N][N];
    int vazio_x, vazio_y;
    int movimentos = 0;
    char movimento;

    printf("\n=================================");
    printf("\n         PUZZLE-8 - JOGO         ");
    printf("\n=================================\n");

    embaralhar_tabuleiro(tabuleiro, &vazio_x, &vazio_y);

    while (!eh_objetivo(tabuleiro)) {
        printf("\nMovimentos realizados: %d", movimentos);
        imprimir_tabuleiro(tabuleiro);
        printf("\nUse WASD para mover (ou Q para sair): ");

        scanf(" %c", &movimento);
        if (movimento == 'Q' || movimento == 'q') break;

        if (mover(tabuleiro, movimento, &vazio_x, &vazio_y)) {
            movimentos++;
        } else {
            printf("\nMovimento inválido!");
        }

        if (eh_objetivo(tabuleiro)) {
            printf("\nParabéns! Você venceu em %d movimentos!", movimentos);
            imprimir_tabuleiro(tabuleiro);
            break;
        }
    }
}

// Modo de resolução por IA
void modo_ia(int tabuleiro_inicial[N][N]) {
    printf("\n=================================");
    printf("\n    RESOLUÇÃO POR INTELIGÊNCIA   ");
    printf("\n          ARTIFICIAL             ");
    printf("\n=================================\n");

    printf("\nEscolha o algoritmo de busca:");
    printf("\n1. Busca em Largura (BFS)");
    printf("\n2. Busca em Profundidade Iterativa (IDS)");
    printf("\nOpção: ");

    int escolha;
    scanf("%d", &escolha);

    switch(escolha) {
        case 1:
            busca_largura(tabuleiro_inicial);
            break;
        case 2:
            busca_profundidade_iterativa(tabuleiro_inicial);
            break;
        default:
            printf("\nOpção inválida!");
    }
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    int opcao;
    do {
        printf("\n=================================");
        printf("\n       PUZZLE-8 MENU PRINCIPAL   ");
        printf("\n=================================");
        printf("\n1. Jogar manualmente");
        printf("\n2. Ver IA resolver");
        printf("\n3. Sair");
        printf("\nEscolha uma opção: ");
        scanf("%d", &opcao);

        /*while (scanf("%d", &opcao) != 1 || opcao < 1 || opcao > 3) {
            printf("Entrada invalida! Escolha uma opcao entre 1 e 3:\n");
            while(getchar() != '\n'); // Limpar o buffer
        }*/

        switch(opcao) {
            case 1:
                modo_jogo();
                break;
            case 2: {
                int tabuleiro_inicial[N][N];
                int vazio_x, vazio_y;
                embaralhar_tabuleiro(tabuleiro_inicial, &vazio_x, &vazio_y);
                printf("\nEstado inicial do puzzle:");
                imprimir_tabuleiro(tabuleiro_inicial);
                modo_ia(tabuleiro_inicial);
                break;
            }
            case 3:
                printf("\nObrigado por jogar!\n");
                break;
            default:
                printf("\nOpção inválida!\n");
        }
    } while (opcao != 3);

    return 0;
}
