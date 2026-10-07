/* ==========================================================================
   eventos.h
   TAD Lista de Eventos (lista linear simplesmente encadeada)
   Cada evento carrega, dentro de si, a sua propria lista de participantes.

   Sistema de Controle de Participacao em Eventos Estudantis
   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995

   ---------------------------------------------------------------------------
   POR QUE LISTA SIMPLESMENTE ENCADEADA AQUI?
   A quantidade de eventos do setor de extensao e imprevisivel, entao uma
   lista estatica obrigaria a chutar um MAX e correr risco de overflow ou
   de desperdicio. Sobre os eventos o sistema so faz duas coisas: cadastrar
   (uma vez) e buscar por codigo (sempre). Nao ha remocao frequente nem
   necessidade de percorrer de tras para frente, entao o ponteiro extra de
   uma lista dupla seria custo sem beneficio. Encadeada simples e o menor
   recurso que resolve o problema.

   INVARIANTE:
     - lista vazia  ->  inicio == NULL
     - o campo proximo do ultimo evento vale NULL
     - nao existem dois eventos com o mesmo codigo
   ========================================================================== */

#ifndef EVENTOS_H
#define EVENTOS_H

#include "lista.h"

#define TAM_NOME_EV 81
#define TAM_DATA    11   /* dd/mm/aaaa + '\0' */

/* No da lista de eventos */
typedef struct NoE {
    int   codigo;
    char  nome[TAM_NOME_EV];
    char  data[TAM_DATA];
    ListaParticipantes inscritos;   /* lista encadeada propria deste evento */
    struct NoE *proximo;
} NoE;

/* Cabecalho da lista de eventos */
typedef struct {
    NoE *inicio;
    int  tamanho;
} ListaEventos;

/* --- Criacao e destruicao ---------------------------------------------- */
void ev_inicializar(ListaEventos *le);
void ev_destruir(ListaEventos *le);

/* --- Consultas ---------------------------------------------------------- */
int   ev_esta_vazia(const ListaEventos *le);
int   ev_tamanho(const ListaEventos *le);
NoE  *ev_buscar(const ListaEventos *le, int codigo);
NoE  *ev_primeiro(const ListaEventos *le);

/* --- Operacoes ---------------------------------------------------------- */
int  ev_cadastrar(ListaEventos *le, int codigo, const char *nome, const char *data);
void ev_listar(const ListaEventos *le);

#endif /* EVENTOS_H */
