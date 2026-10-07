/* ==========================================================================
   participantes.c
   Implementacao das regras de inscricao, remocao e relatorios.

   Sistema de Controle de Participacao em Eventos Estudantis
   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995
   ========================================================================== */

#include <stdio.h>
#include <string.h>
#include "participantes.h"

/* --------------------------------------------------------------------------
   Funcao: pa_inscrever
   O que faz: inscreve um participante num evento, IMPEDINDO duplicidade.
   Como impede a duplicidade: antes de inserir, chama lp_buscar_ra na lista
              daquele evento. Se o RA ja estiver la, recusa a operacao.
              Repare que a checagem e por evento: o mesmo RA PODE estar
              inscrito em varios eventos diferentes, so nao duas vezes no
              mesmo evento.
   Complexidade: O(e + p), sendo e o numero de eventos (busca do evento)
              e p o numero de inscritos naquele evento (busca do RA e
              insercao no fim).
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int pa_inscrever(ListaEventos *le, int codigo, const char *ra, const char *nome)
{
    NoE *ev = ev_buscar(le, codigo);

    if (ev == NULL) {
        return PA_EVENTO_INEXISTENTE;
    }
    if (lp_buscar_ra(&ev->inscritos, ra) != NULL) {
        return PA_JA_INSCRITO;            /* regra de negocio do trabalho */
    }
    if (!lp_inserir_fim(&ev->inscritos, ra, nome)) {
        return PA_SEM_MEMORIA;
    }
    return PA_OK;
}

/* --------------------------------------------------------------------------
   Funcao: pa_remover
   O que faz: remove a inscricao de um participante em um evento.
   Complexidade: O(e + p).
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int pa_remover(ListaEventos *le, int codigo, const char *ra)
{
    NoE *ev = ev_buscar(le, codigo);
    Participante removido;

    if (ev == NULL) {
        return PA_EVENTO_INEXISTENTE;
    }
    if (!lp_remover_ra(&ev->inscritos, ra, &removido)) {
        return PA_NAO_INSCRITO;
    }
    printf("\n  Inscricao removida: %s (RA %s)\n", removido.nome, removido.ra);
    return PA_OK;
}

/* --------------------------------------------------------------------------
   Funcao: pa_listar_evento
   O que faz: imprime todos os participantes inscritos em um evento, na
              ordem de inscricao.
   Detalhe: percorre a partir de lp_primeiro, que ja devolve o primeiro
              participante REAL, pulando o sentinela.
   Complexidade: O(e + p).
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int pa_listar_evento(const ListaEventos *le, int codigo)
{
    NoE *ev = ev_buscar(le, codigo);
    NoP *atual;
    int  i = 1;

    if (ev == NULL) {
        return PA_EVENTO_INEXISTENTE;
    }

    printf("\n==========================================================\n");
    printf("  PARTICIPANTES DO EVENTO %d - %s\n", ev->codigo, ev->nome);
    printf("  Data: %s\n", ev->data);
    printf("==========================================================\n");

    if (lp_esta_vazia(&ev->inscritos)) {
        printf("  Nenhum participante inscrito.\n");
        printf("==========================================================\n");
        return PA_OK;
    }

    printf("  %-4s %-14s %s\n", "#", "RA", "NOME");
    printf("  ----------------------------------------------------------\n");

    atual = lp_primeiro(&ev->inscritos);
    while (atual != NULL) {
        printf("  %-4d %-14s %s\n", i, atual->dados.ra, atual->dados.nome);
        atual = atual->proximo;
        i++;
    }

    printf("  ----------------------------------------------------------\n");
    printf("  Total: %d inscrito(s)\n", lp_tamanho(&ev->inscritos));
    printf("==========================================================\n");
    return PA_OK;
}

/* --------------------------------------------------------------------------
   Funcao: pa_lista_presenca
   O que faz: emite a lista de presenca de um evento, com uma coluna de
              assinatura, no formato que seria impresso em papel.
   Diferenca para pa_listar_evento: aqui o foco e o documento fisico, por
              isso o cabecalho formal e a linha de assinatura.
   Complexidade: O(e + p).
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int pa_lista_presenca(const ListaEventos *le, int codigo)
{
    NoE *ev = ev_buscar(le, codigo);
    NoP *atual;
    int  i = 1;

    if (ev == NULL) {
        return PA_EVENTO_INEXISTENTE;
    }

    printf("\n");
    printf("  UNIVERSIDADE FEDERAL DO PARANA\n");
    printf("  Setor de Educacao Profissional e Tecnologica\n");
    printf("  LISTA DE PRESENCA\n\n");
    printf("  Evento: %s (codigo %d)\n", ev->nome, ev->codigo);
    printf("  Data:   %s\n\n", ev->data);

    if (lp_esta_vazia(&ev->inscritos)) {
        printf("  Nenhum participante inscrito neste evento.\n\n");
        return PA_OK;
    }

    printf("  %-4s %-14s %-30s %s\n", "#", "RA", "NOME", "ASSINATURA");
    printf("  ---------------------------------------------------------------------\n");

    atual = lp_primeiro(&ev->inscritos);
    while (atual != NULL) {
        printf("  %-4d %-14s %-30.30s ____________________\n",
               i, atual->dados.ra, atual->dados.nome);
        atual = atual->proximo;
        i++;
    }

    printf("\n  Total de inscritos: %d\n", lp_tamanho(&ev->inscritos));
    printf("  _________________________________\n");
    printf("  Assinatura do responsavel\n\n");
    return PA_OK;
}

/* --------------------------------------------------------------------------
   Funcao: pa_relatorio_ra
   O que faz: emite o relatorio de participacao individual, mostrando TODOS
              os eventos em que aquele RA esta inscrito.
   Como funciona: percorre a lista de EVENTOS e, para cada evento, faz uma
              busca pelo RA na lista de participantes daquele evento. E uma
              busca aninhada: a lista externa de eventos por fora, a lista
              encadeada de participantes por dentro.
   Complexidade: O(e * p) no pior caso.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int pa_relatorio_ra(const ListaEventos *le, const char *ra)
{
    NoE *ev = ev_primeiro(le);
    NoP *achado;
    int  total = 0;
    char nome[TAM_NOME];

    nome[0] = '\0';

    printf("\n==========================================================\n");
    printf("  RELATORIO DE PARTICIPACAO INDIVIDUAL\n");
    printf("  RA consultado: %s\n", ra);
    printf("==========================================================\n");

    while (ev != NULL) {
        achado = lp_buscar_ra(&ev->inscritos, ra);
        if (achado != NULL) {
            if (nome[0] == '\0') {
                strncpy(nome, achado->dados.nome, TAM_NOME - 1);
                nome[TAM_NOME - 1] = '\0';
            }
            printf("  [%d] %-32.32s  %s\n", ev->codigo, ev->nome, ev->data);
            total++;
        }
        ev = ev->proximo;
    }

    if (total == 0) {
        printf("  Este RA nao esta inscrito em nenhum evento.\n");
    } else {
        printf("  ----------------------------------------------------------\n");
        printf("  Participante: %s\n", nome);
        printf("  Total de eventos: %d\n", total);
    }
    printf("==========================================================\n");
    return PA_OK;
}
