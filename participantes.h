/* ==========================================================================
   participantes.h
   Regras de negocio de inscricao, remocao e relatorios.

   Este modulo NAO reimplementa lista nenhuma. Ele usa a interface publica
   dos TADs lista.h e eventos.h. E a camada que conhece as REGRAS do
   problema (nao pode haver inscricao duplicada, relatorio por RA percorre
   todos os eventos, e assim por diante).

   Sistema de Controle de Participacao em Eventos Estudantis
   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995
   ========================================================================== */

#ifndef PARTICIPANTES_H
#define PARTICIPANTES_H

#include "eventos.h"

/* Codigos de retorno das operacoes de inscricao e remocao */
#define PA_OK                 1
#define PA_SEM_MEMORIA        0
#define PA_EVENTO_INEXISTENTE -1
#define PA_JA_INSCRITO        -2
#define PA_NAO_INSCRITO       -3

int  pa_inscrever(ListaEventos *le, int codigo, const char *ra, const char *nome);
int  pa_remover(ListaEventos *le, int codigo, const char *ra);
int  pa_listar_evento(const ListaEventos *le, int codigo);
int  pa_lista_presenca(const ListaEventos *le, int codigo);
int  pa_relatorio_ra(const ListaEventos *le, const char *ra);

#endif /* PARTICIPANTES_H */
