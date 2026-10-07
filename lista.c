/* ==========================================================================
   lista.c
   Implementacao do TAD Lista de Participantes (encadeada com no cabecalho)

   Sistema de Controle de Participacao em Eventos Estudantis
   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995
   ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

/* --------------------------------------------------------------------------
   Funcao: criar_no  (auxiliar interna)
   O que faz: aloca um no novo ja preenchido com RA e nome, com o campo
              proximo apontando para NULL.
   Por que e static: e detalhe de implementacao, nao faz parte da interface
              do TAD, entao nao deve ser visivel fora deste arquivo.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static NoP *criar_no(const char *ra, const char *nome)
{
    NoP *novo = (NoP *) malloc(sizeof(NoP));
    if (novo == NULL) {               /* malloc pode falhar: sempre checar */
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        return NULL;
    }

    /* strncpy com limite evita estouro do vetor de char */
    strncpy(novo->dados.ra, ra, TAM_RA - 1);
    novo->dados.ra[TAM_RA - 1] = '\0';
    strncpy(novo->dados.nome, nome, TAM_NOME - 1);
    novo->dados.nome[TAM_NOME - 1] = '\0';

    novo->proximo = NULL;
    return novo;
}

/* --------------------------------------------------------------------------
   Funcao: lp_inicializar
   O que faz: cria o no SENTINELA e deixa a lista logicamente vazia.
              A partir daqui 'cabecalho' nunca mais sera NULL.
   Retorno: 1 em caso de sucesso, 0 se faltar memoria.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int lp_inicializar(ListaParticipantes *l)
{
    l->cabecalho = (NoP *) malloc(sizeof(NoP));
    if (l->cabecalho == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente ao criar o sentinela.\n");
        l->tamanho = 0;
        return 0;
    }

    /* O dado do sentinela NUNCA e lido. Zeramos so por higiene. */
    l->cabecalho->dados.ra[0]   = '\0';
    l->cabecalho->dados.nome[0] = '\0';
    l->cabecalho->proximo = NULL;   /* logicamente vazia */
    l->tamanho = 0;
    return 1;
}

/* --------------------------------------------------------------------------
   Funcao: lp_esta_vazia
   O que faz: diz se a lista nao tem nenhum participante real.
   Detalhe: o sentinela existe sempre, por isso o teste e feito no
            cabecalho->proximo e NAO no cabecalho.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int lp_esta_vazia(const ListaParticipantes *l)
{
    return (l->cabecalho->proximo == NULL);
}

/* --------------------------------------------------------------------------
   Funcao: lp_tamanho
   O que faz: devolve a quantidade de participantes REAIS.
              O no sentinela nunca entra nessa contagem.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int lp_tamanho(const ListaParticipantes *l)
{
    return l->tamanho;
}

/* --------------------------------------------------------------------------
   Funcao: lp_primeiro
   O que faz: devolve o primeiro participante REAL da lista, ou NULL se
              estiver vazia. Serve para os outros modulos percorrerem a
              lista sem precisar saber que existe um sentinela.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
NoP *lp_primeiro(const ListaParticipantes *l)
{
    return l->cabecalho->proximo;
}

/* --------------------------------------------------------------------------
   Funcao: lp_buscar_ra
   O que faz: procura um participante pelo RA e devolve o ponteiro do no,
              ou NULL se nao existir.
   Complexidade: O(n), busca sequencial.
   Detalhe: como esta funcao LE DADOS, o percurso comeca em
            cabecalho->proximo, pulando o sentinela.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
NoP *lp_buscar_ra(const ListaParticipantes *l, const char *ra)
{
    NoP *atual = l->cabecalho->proximo;

    while (atual != NULL) {
        if (strcmp(atual->dados.ra, ra) == 0) {
            return atual;             /* encontrou */
        }
        atual = atual->proximo;       /* anda uma casa */
    }
    return NULL;                      /* RA nao inscrito nesta lista */
}

/* --------------------------------------------------------------------------
   Funcao: lp_inserir_fim
   O que faz: insere um participante no FIM da lista, preservando a ordem
              de inscricao (que e o que a lista de presenca precisa).
   Complexidade: O(n), porque percorre ate o ultimo no.
   Detalhe: como esta funcao precisa RELIGAR PONTEIROS, o percurso comeca
            no proprio cabecalho. Assim, inserir na lista vazia usa
            exatamente o mesmo codigo de inserir numa lista cheia.
   Retorno: 1 sucesso, 0 falha de memoria.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int lp_inserir_fim(ListaParticipantes *l, const char *ra, const char *nome)
{
    NoP *novo, *atual;

    novo = criar_no(ra, nome);
    if (novo == NULL) return 0;

    atual = l->cabecalho;                 /* comeca no SENTINELA */
    while (atual->proximo != NULL) {      /* anda ate o ultimo no */
        atual = atual->proximo;
    }
    atual->proximo = novo;                /* o ultimo passa a apontar p/ novo */

    l->tamanho++;
    return 1;
}

/* --------------------------------------------------------------------------
   Funcao: lp_remover_ra
   O que faz: remove o participante de RA informado e devolve, por
              parametro de saida, os dados do removido.
   Complexidade: O(n).
   Detalhe importante: o ponteiro 'anterior' comeca no cabecalho. Por isso
              remover o PRIMEIRO participante nao precisa de nenhum
              tratamento especial: o anterior dele e o proprio sentinela.
   Ordem das operacoes: primeiro religa o ponteiro, SO DEPOIS chama free.
              Ler um campo de memoria ja liberada e erro grave.
   Retorno: 1 se removeu, 0 se o RA nao estava na lista.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int lp_remover_ra(ListaParticipantes *l, const char *ra, Participante *saida)
{
    NoP *anterior = l->cabecalho;         /* comeca no SENTINELA */
    NoP *removido;

    /* Para no no cujo SUCESSOR tem o RA procurado */
    while (anterior->proximo != NULL &&
           strcmp(anterior->proximo->dados.ra, ra) != 0) {
        anterior = anterior->proximo;
    }

    if (anterior->proximo == NULL) {
        return 0;                         /* percorreu tudo e nao achou */
    }

    removido = anterior->proximo;

    if (saida != NULL) {
        *saida = removido->dados;         /* guarda antes de liberar */
    }

    anterior->proximo = removido->proximo; /* 1) religa, "pulando" o no   */
    free(removido);                        /* 2) so agora devolve a memoria */

    l->tamanho--;
    return 1;
}

/* --------------------------------------------------------------------------
   Funcao: lp_destruir
   O que faz: libera TODOS os nos da lista, INCLUSIVE o sentinela.
   Detalhe: o laco comeca em l->cabecalho (e nao em cabecalho->proximo),
            porque o sentinela tambem foi alocado com malloc e tambem
            precisa do seu free. Esquecer isso e um vazamento silencioso.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
void lp_destruir(ListaParticipantes *l)
{
    NoP *atual = l->cabecalho;            /* comeca NO SENTINELA */
    NoP *proximo;

    while (atual != NULL) {
        proximo = atual->proximo;         /* guarda ANTES de liberar */
        free(atual);
        atual = proximo;
    }

    l->cabecalho = NULL;
    l->tamanho = 0;
}
