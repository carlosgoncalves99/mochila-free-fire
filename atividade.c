/* mochila.c
 * Comparação de estruturas: Vetor (sequencial) vs Lista Encadeada
 * Requisitos atendidos:
 *  - Structs: Item e No (para lista)
 *  - Duas implementações paralelas (vetor e lista)
 *  - Inserir, Remover (por nome), Listar, Buscar (sequencial) nas duas
 *  - Ordenar (Bubble Sort) no vetor
 *  - Busca binária (apenas no vetor) após ordenar
 *  - Contadores de comparações (sequencial e binária)
 *  - Menu claro, nomes descritivos e comentários
 *
 * Obs: Mantive comparação de nomes "case-insensitive" para usabilidade.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>   /* opcional: medir tempo */

#define MAX_ITENS 100

/* ==========================
   MODELOS DE DADOS
   ========================== */
typedef struct {
    char nome[30];
    char tipo[20];
    int  quantidade;
} Item;

typedef struct No {
    Item dados;
    struct No* proximo;
} No;

/* ==========================
   CONTADORES DE COMPARAÇÕES
   ========================== */
long comparacoesSequencialVetor = 0;
long comparacoesBinariaVetor    = 0;
long comparacoesSequencialLista = 0;

/* ==========================
   HELPERS
   ========================== */

/* strcasecmp compatível (case-insensitive) */
int stricmp_pt(const char *a, const char *b) {
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        ++a; ++b;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* Leitura segura de linha (remove '\n') */
void lerLinha(char *dest, int tam) {
    if (fgets(dest, tam, stdin)) {
        size_t n = strlen(dest);
        if (n && dest[n-1] == '\n') dest[n-1] = '\0';
    }
}

/* ==========================
   MOCHILA COM VETOR
   ========================== */
typedef struct {
    Item itens[MAX_ITENS];
    int  tamanho;
    int  ordenado; /* 0 = não, 1 = sim (por nome) */
} MochilaVetor;

void iniciarMochilaVetor(MochilaVetor *m) {
    m->tamanho = 0;
    m->ordenado = 0;
}

int inserirItemVetor(MochilaVetor *m, Item it) {
    if (m->tamanho >= MAX_ITENS) return 0;
    m->itens[m->tamanho++] = it;
    m->ordenado = 0; /* inserir pode quebrar ordenação */
    return 1;
}

int removerItemVetor(MochilaVetor *m, const char *nome) {
    for (int i = 0; i < m->tamanho; ++i) {
        if (stricmp_pt(m->itens[i].nome, nome) == 0) {
            /* desloca à esquerda */
            for (int j = i; j < m->tamanho - 1; ++j) {
                m->itens[j] = m->itens[j+1];
            }
            m->tamanho--;
            /* remoção mantém ordenação relativa; conservamos flag */
            return 1;
        }
    }
    return 0;
}

void listarVetor(const MochilaVetor *m) {
    if (m->tamanho == 0) {
        printf("[VETOR] Mochila vazia.\n");
        return;
    }
    printf("[VETOR] Itens (%d):\n", m->tamanho);
    for (int i = 0; i < m->tamanho; ++i) {
        printf(" - %s | tipo: %s | qtd: %d\n",
               m->itens[i].nome, m->itens[i].tipo, m->itens[i].quantidade);
    }
    printf("Estado ordenado: %s\n", m->ordenado ? "SIM" : "NÃO");
}

/* Busca sequencial no vetor (conta comparações) */
int buscarSequencialVetor(const MochilaVetor *m, const char *nome, int *pos) {
    comparacoesSequencialVetor = 0;
    for (int i = 0; i < m->tamanho; ++i) {
        comparacoesSequencialVetor++;
        if (stricmp_pt(m->itens[i].nome, nome) == 0) {
            if (pos) *pos = i;
            return 1;
        }
    }
    return 0;
}

/* Ordenação (Bubble Sort) por nome */
void ordenarVetor(MochilaVetor *m) {
    if (m->tamanho <= 1) { m->ordenado = 1; return; }
    for (int i = 0; i < m->tamanho - 1; ++i) {
        int trocou = 0;
        for (int j = 0; j < m->tamanho - 1 - i; ++j) {
            if (stricmp_pt(m->itens[j].nome, m->itens[j+1].nome) > 0) {
                Item aux = m->itens[j];
                m->itens[j] = m->itens[j+1];
                m->itens[j+1] = aux;
                trocou = 1;
            }
        }
        if (!trocou) break; /* melhor caso O(n) */
    }
    m->ordenado = 1;
}

/* Busca binária por nome no vetor ordenado (conta comparações) */
int buscarBinariaVetor(const MochilaVetor *m, const char *nome, int *pos) {
    comparacoesBinariaVetor = 0;
    if (!m->ordenado) {
        printf("[AVISO] Vetor não está ordenado. Ordene antes da busca binária.\n");
        return 0;
    }
    int ini = 0, fim = m->tamanho - 1;
    while (ini <= fim) {
        int meio = ini + (fim - ini) / 2;
        comparacoesBinariaVetor++;
        int cmp = stricmp_pt(m->itens[meio].nome, nome);
        if (cmp == 0) { if (pos) *pos = meio; return 1; }
        else if (cmp < 0) ini = meio + 1;
        else fim = meio - 1;
    }
    return 0;
}

/* ==========================
   MOCHILA COM LISTA ENCADEADA
   ========================== */
typedef struct {
    No *inicio;
    int tamanho;
} MochilaLista;

void iniciarMochilaLista(MochilaLista *m) {
    m->inicio = NULL;
    m->tamanho = 0;
}

int inserirItemLista(MochilaLista *m, Item it) {
    No *novo = (No*)malloc(sizeof(No));
    if (!novo) return 0;
    novo->dados = it;
    novo->proximo = m->inicio; /* insere no início (O(1)) */
    m->inicio = novo;
    m->tamanho++;
    return 1;
}

int removerItemLista(MochilaLista *m, const char *nome) {
    No *ant = NULL, *p = m->inicio;
    while (p) {
        if (stricmp_pt(p->dados.nome, nome) == 0) {
            if (ant) ant->proximo = p->proximo;
            else m->inicio = p->proximo;
            free(p);
            m->tamanho--;
            return 1;
        }
        ant = p;
        p = p->proximo;
    }
    return 0;
}

void listarLista(const MochilaLista *m) {
    if (!m->inicio) {
        printf("[LISTA] Mochila vazia.\n");
        return;
    }
    printf("[LISTA] Itens (%d):\n", m->tamanho);
    for (No *p = m->inicio; p; p = p->proximo) {
        printf(" - %s | tipo: %s | qtd: %d\n",
               p->dados.nome, p->dados.tipo, p->dados.quantidade);
    }
}

/* Busca sequencial na lista (conta comparações) */
int buscarSequencialLista(const MochilaLista *m, const char *nome, Item *encontrado) {
    comparacoesSequencialLista = 0;
    for (No *p = m->inicio; p; p = p->proximo) {
        comparacoesSequencialLista++;
        if (stricmp_pt(p->dados.nome, nome) == 0) {
            if (encontrado) *encontrado = p->dados;
            return 1;
        }
    }
    return 0;
}

/* Libera memória da lista */
void destruirLista(MochilaLista *m) {
    No *p = m->inicio;
    while (p) {
        No *prox = p->proximo;
        free(p);
        p = prox;
    }
    m->inicio = NULL;
    m->tamanho = 0;
}

/* ==========================
   MENUS / IU
   ========================== */

void menuVetor(MochilaVetor *v) {
    int op;
    char buf[64];
    do {
        printf("\n=== MOCHILA (VETOR) ===\n");
        printf("1. Inserir item\n");
        printf("2. Remover item por nome\n");
        printf("3. Listar itens\n");
        printf("4. Buscar (sequencial)\n");
        printf("5. Ordenar por nome (Bubble Sort)\n");
        printf("6. Buscar (binaria) [requer ordenado]\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) { while(getchar()!='\n'); op=-1; }
        while(getchar()!='\n'); /* limpa buffer */

        if (op == 1) {
            Item it;
            printf("Nome: "); lerLinha(it.nome, sizeof(it.nome));
            printf("Tipo: "); lerLinha(it.tipo, sizeof(it.tipo));
            printf("Quantidade: "); scanf("%d", &it.quantidade); while(getchar()!='\n');
            if (inserirItemVetor(v, it)) puts("[OK] Inserido.");
            else puts("[ERRO] Mochila cheia.");
        } else if (op == 2) {
            printf("Nome a remover: "); lerLinha(buf, sizeof(buf));
            if (removerItemVetor(v, buf)) puts("[OK] Removido.");
            else puts("[! ] Nao encontrado.");
        } else if (op == 3) {
            listarVetor(v);
        } else if (op == 4) {
            printf("Nome a buscar (sequencial): "); lerLinha(buf, sizeof(buf));
            int pos;
            clock_t t0 = clock();
            int ok = buscarSequencialVetor(v, buf, &pos);
            clock_t t1 = clock();
            if (ok) printf("[ACHOU] Posicao %d | comparacoes: %ld | tempo: %.3f ms\n",
                           pos, comparacoesSequencialVetor, 1000.0*(t1-t0)/CLOCKS_PER_SEC);
            else     printf("[NAO ACHOU] | comparacoes: %ld | tempo: %.3f ms\n",
                           comparacoesSequencialVetor, 1000.0*(t1-t0)/CLOCKS_PER_SEC);
        } else if (op == 5) {
            clock_t t0 = clock();
            ordenarVetor(v);
            clock_t t1 = clock();
            printf("[OK] Ordenado por nome. Tempo: %.3f ms\n", 1000.0*(t1-t0)/CLOCKS_PER_SEC);
        } else if (op == 6) {
            printf("Nome a buscar (binaria): "); lerLinha(buf, sizeof(buf));
            int pos;
            clock_t t0 = clock();
            int ok = buscarBinariaVetor(v, buf, &pos);
            clock_t t1 = clock();
            if (ok) printf("[ACHOU] Posicao %d | comparacoes(binaria): %ld | tempo: %.3f ms\n",
                           pos, comparacoesBinariaVetor, 1000.0*(t1-t0)/CLOCKS_PER_SEC);
            else     printf("[NAO ACHOU] | comparacoes(binaria): %ld | tempo: %.3f ms\n",
                           comparacoesBinariaVetor, 1000.0*(t1-t0)/CLOCKS_PER_SEC);
        }
    } while (op != 0);
}

void menuLista(MochilaLista *l) {
    int op;
    char buf[64];
    do {
        printf("\n=== MOCHILA (LISTA ENCADEADA) ===\n");
        printf("1. Inserir item\n");
        printf("2. Remover item por nome\n");
        printf("3. Listar itens\n");
        printf("4. Buscar (sequencial)\n");
        printf("0. Voltar\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) { while(getchar()!='\n'); op=-1; }
        while(getchar()!='\n');

        if (op == 1) {
            Item it;
            printf("Nome: "); lerLinha(it.nome, sizeof(it.nome));
            printf("Tipo: "); lerLinha(it.tipo, sizeof(it.tipo));
            printf("Quantidade: "); scanf("%d", &it.quantidade); while(getchar()!='\n');
            if (inserirItemLista(l, it)) puts("[OK] Inserido.");
            else puts("[ERRO] Falha ao inserir.");
        } else if (op == 2) {
            printf("Nome a remover: "); lerLinha(buf, sizeof(buf));
            if (removerItemLista(l, buf)) puts("[OK] Removido.");
            else puts("[! ] Nao encontrado.");
        } else if (op == 3) {
            listarLista(l);
        } else if (op == 4) {
            printf("Nome a buscar (sequencial): "); lerLinha(buf, sizeof(buf));
            Item achado;
            clock_t t0 = clock();
            int ok = buscarSequencialLista(l, buf, &achado);
            clock_t t1 = clock();
            if (ok) {
                printf("[ACHOU] %s | tipo: %s | qtd: %d | comparacoes: %ld | tempo: %.3f ms\n",
                       achado.nome, achado.tipo, achado.quantidade,
                       comparacoesSequencialLista, 1000.0*(t1-t0)/CLOCKS_PER_SEC);
            } else {
                printf("[NAO ACHOU] | comparacoes: %ld | tempo: %.3f ms\n",
                       comparacoesSequencialLista, 1000.0*(t1-t0)/CLOCKS_PER_SEC);
            }
        }
    } while (op != 0);
}

/* ==========================
   MAIN
   ========================== */
int main(void) {
    MochilaVetor mv;
    MochilaLista ml;
    iniciarMochilaVetor(&mv);
    iniciarMochilaLista(&ml);

    /* Exemplos iniciais (opcional) para testar rápido */
    Item exemplos[] = {
        {"Pocao", "Consumivel", 3},
        {"Espada", "Arma", 1},
        {"Escudo", "Armadura", 1},
        {"Antidoto", "Consumivel", 5},
        {"Flecha", "Municao", 20}
    };
    for (int i = 0; i < 5; ++i) inserirItemVetor(&mv, exemplos[i]);
    for (int i = 0; i < 5; ++i) inserirItemLista(&ml, exemplos[i]);

    int op;
    do {
        printf("\n=============================\n");
        printf(" SISTEMA DE MOCHILA (JOGO) \n");
        printf("=============================\n");
        printf("1. Usar Mochila (Vetor)\n");
        printf("2. Usar Mochila (Lista Encadeada)\n");
        printf("9. Comparar contadores (ultimo teste)\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) { while(getchar()!='\n'); op=-1; }
        while(getchar()!='\n');

        if (op == 1) menuVetor(&mv);
        else if (op == 2) menuLista(&ml);
        else if (op == 9) {
            printf("\n--- COMPARACAO (ULTIMAS BUSCAS) ---\n");
            printf("Vetor:  Sequencial = %ld comparacoes | Binaria = %ld comparacoes\n",
                   comparacoesSequencialVetor, comparacoesBinariaVetor);
            printf("Lista:  Sequencial = %ld comparacoes\n",
                   comparacoesSequencialLista);
            puts("(Dica: ordene o vetor e compare as buscas no mesmo item.)");
        }
    } while (op != 0);

    destruirLista(&ml);
    puts("Encerrado. Ate mais!");
    return 0;
}
