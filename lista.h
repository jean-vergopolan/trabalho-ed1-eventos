/* ==========================================================================
   lista.h
   TAD Lista Linear Simplesmente Encadeada COM NO CABECALHO (sentinela)
   Armazena os participantes inscritos em um evento.

   Sistema de Controle de Participacao em Eventos Estudantis
   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995

   ---------------------------------------------------------------------------
   POR QUE LISTA COM NO CABECALHO?
   As operacoes mais frequentes do sistema sao inscrever e remover
   participante. Numa lista encadeada tradicional, remover o PRIMEIRO
   elemento exige um caminho de codigo separado, porque ele altera o
   proprio ponteiro de inicio da lista. Com o no cabecalho sempre existe
   um no anterior (o proprio sentinela), entao inserir e remover seguem
   um unico caminho de codigo, sem nenhum "if lista vazia".
   O custo e um unico no extra por evento, de tamanho fixo.

   INVARIANTE DA ESTRUTURA:
     - 'cabecalho' NUNCA e NULL depois de lp_inicializar
     - o primeiro participante real e sempre cabecalho->proximo
     - lista logicamente vazia  ->  cabecalho->proximo == NULL
     - 'tamanho' conta apenas os participantes reais (o sentinela NAO conta)
   ========================================================================== */

#ifndef LISTA_H
#define LISTA_H

#define TAM_RA    16
#define TAM_NOME  61

/* Informacao util de um participante */
typedef struct {
    char ra[TAM_RA];
    char nome[TAM_NOME];
} Participante;

/* No da lista encadeada de participantes */
typedef struct NoP {
    Participante dados;
    struct NoP *proximo;
} NoP;

/* Cabecalho da lista: guarda o no sentinela e a quantidade de reais */
typedef struct {
    NoP *cabecalho;
    int  tamanho;
} ListaParticipantes;

/* --- Criacao e destruicao ---------------------------------------------- */
int  lp_inicializar(ListaParticipantes *l);
void lp_destruir(ListaParticipantes *l);

/* --- Consultas ---------------------------------------------------------- */
int   lp_esta_vazia(const ListaParticipantes *l);
int   lp_tamanho(const ListaParticipantes *l);
NoP  *lp_buscar_ra(const ListaParticipantes *l, const char *ra);
NoP  *lp_primeiro(const ListaParticipantes *l);

/* --- Insercao e remocao ------------------------------------------------- */
int lp_inserir_fim(ListaParticipantes *l, const char *ra, const char *nome);
int lp_remover_ra(ListaParticipantes *l, const char *ra, Participante *saida);

#endif /* LISTA_H */
