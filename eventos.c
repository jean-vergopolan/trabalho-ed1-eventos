/* ==========================================================================
   eventos.c
   Implementacao do TAD Lista de Eventos

   Sistema de Controle de Participacao em Eventos Estudantis
   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995
   ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eventos.h"

/* --------------------------------------------------------------------------
   Funcao: ev_inicializar
   O que faz: deixa a lista de eventos vazia.
   Detalhe: aqui NAO existe sentinela, entao lista vazia significa
            inicio == NULL. E diferente da lista de participantes de
            proposito, para a gente comparar as duas estrategias.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
void ev_inicializar(ListaEventos *le)
{
    le->inicio  = NULL;
    le->tamanho = 0;
}

/* --------------------------------------------------------------------------
   Funcao: ev_esta_vazia / ev_tamanho / ev_primeiro
   O que fazem: consultas simples de estado, todas O(1).
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int ev_esta_vazia(const ListaEventos *le)
{
    return (le->inicio == NULL);
}

int ev_tamanho(const ListaEventos *le)
{
    return le->tamanho;
}

NoE *ev_primeiro(const ListaEventos *le)
{
    return le->inicio;
}

/* --------------------------------------------------------------------------
   Funcao: ev_buscar
   O que faz: procura um evento pelo codigo e devolve o ponteiro do no,
              ou NULL se nao existir.
   Complexidade: O(n), busca sequencial.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
NoE *ev_buscar(const ListaEventos *le, int codigo)
{
    NoE *atual = le->inicio;

    while (atual != NULL) {
        if (atual->codigo == codigo) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

/* --------------------------------------------------------------------------
   Funcao: ev_cadastrar
   O que faz: cadastra um evento novo no FIM da lista, mantendo a ordem de
              cadastro, e ja inicializa a lista de participantes dele.
   Regra de negocio: o codigo do evento e unico. Se ja existir, recusa.
   Complexidade: O(n) por causa da verificacao de duplicidade.
   Retorno:  1 sucesso
             0 falha de memoria
            -1 ja existe evento com esse codigo
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int ev_cadastrar(ListaEventos *le, int codigo, const char *nome, const char *data)
{
    NoE *novo, *atual;

    if (ev_buscar(le, codigo) != NULL) {
        return -1;                       /* codigo duplicado */
    }

    novo = (NoE *) malloc(sizeof(NoE));
    if (novo == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        return 0;
    }

    novo->codigo = codigo;
    strncpy(novo->nome, nome, TAM_NOME_EV - 1);
    novo->nome[TAM_NOME_EV - 1] = '\0';
    strncpy(novo->data, data, TAM_DATA - 1);
    novo->data[TAM_DATA - 1] = '\0';
    novo->proximo = NULL;

    /* cada evento nasce com a sua propria lista de participantes */
    if (!lp_inicializar(&novo->inscritos)) {
        free(novo);
        return 0;
    }

    if (le->inicio == NULL) {            /* lista vazia: novo e o unico */
        le->inicio = novo;
    } else {
        atual = le->inicio;
        while (atual->proximo != NULL) { /* percorre ate o ultimo */
            atual = atual->proximo;
        }
        atual->proximo = novo;
    }

    le->tamanho++;
    return 1;
}

/* --------------------------------------------------------------------------
   Funcao: ev_listar
   O que faz: imprime todos os eventos cadastrados com a quantidade de
              inscritos de cada um.
   Complexidade: O(n).
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
void ev_listar(const ListaEventos *le)
{
    NoE *atual = le->inicio;

    if (ev_esta_vazia(le)) {
        printf("\nNenhum evento cadastrado.\n");
        return;
    }

    printf("\n==========================================================\n");
    printf("  EVENTOS CADASTRADOS (%d)\n", le->tamanho);
    printf("==========================================================\n");
    printf("  %-8s %-32s %-12s %s\n", "CODIGO", "NOME", "DATA", "INSCRITOS");
    printf("  ----------------------------------------------------------\n");

    while (atual != NULL) {
        printf("  %-8d %-32.32s %-12s %d\n",
               atual->codigo, atual->nome, atual->data,
               lp_tamanho(&atual->inscritos));
        atual = atual->proximo;
    }
    printf("==========================================================\n");
}

/* --------------------------------------------------------------------------
   Funcao: ev_destruir
   O que faz: libera toda a memoria do sistema.
   Detalhe importante: para CADA evento e preciso destruir primeiro a lista
              de participantes dele (que tem os seus proprios mallocs,
              inclusive o do sentinela) e so entao liberar o no do evento.
              E tambem e preciso guardar o ponteiro do proximo evento ANTES
              de liberar o atual.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
void ev_destruir(ListaEventos *le)
{
    NoE *atual = le->inicio;
    NoE *proximo;

    while (atual != NULL) {
        proximo = atual->proximo;        /* guarda ANTES de liberar */
        lp_destruir(&atual->inscritos);  /* libera a lista interna  */
        free(atual);
        atual = proximo;
    }

    le->inicio  = NULL;
    le->tamanho = 0;
}
