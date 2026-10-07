/* ==========================================================================
   main.c
   Menu do Sistema de Controle de Participacao em Eventos Estudantis

   Estrutura de Dados I - DS130 - TADS / UFPR
   Autor: Jeanluca Vergopolan - GRR20230995

   Compilacao:
     gcc -Wall -Wextra -pedantic -std=c11 -o sistema \
         main.c lista.c eventos.c participantes.c
   Execucao:
     ./sistema
   Verificacao de memoria:
     valgrind --leak-check=full ./sistema
   ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "participantes.h"

/* --------------------------------------------------------------------------
   Funcao: ler_linha  (auxiliar interna)
   O que faz: le uma linha inteira do teclado e remove o '\n' do final.
   Por que existe: scanf("%s") para no primeiro espaco, o que quebraria
              nomes compostos. Alem disso, misturar scanf e fgets costuma
              deixar lixo no buffer. Lendo tudo com fgets o comportamento
              fica previsivel.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void ler_linha(char *destino, int tamanho)
{
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    destino[strcspn(destino, "\n")] = '\0';
}

/* --------------------------------------------------------------------------
   Funcao: ler_inteiro  (auxiliar interna)
   O que faz: le uma linha e tenta converter para inteiro.
   Retorno: 1 se converteu, 0 se o usuario digitou algo invalido.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static int ler_inteiro(const char *rotulo, int *destino)
{
    char buffer[64];
    printf("%s", rotulo);
    ler_linha(buffer, sizeof(buffer));
    return (sscanf(buffer, "%d", destino) == 1);
}

/* --------------------------------------------------------------------------
   Funcao: mostrar_menu
   O que faz: imprime as opcoes do sistema.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void mostrar_menu(void)
{
    printf("\n==========================================================\n");
    printf("  SISTEMA DE CONTROLE DE PARTICIPACAO EM EVENTOS\n");
    printf("  UFPR - Setor de Extensao\n");
    printf("==========================================================\n");
    printf("  1 - Cadastrar evento\n");
    printf("  2 - Listar eventos\n");
    printf("  3 - Inscrever participante\n");
    printf("  4 - Remover participante de um evento\n");
    printf("  5 - Listar participantes de um evento\n");
    printf("  6 - Relatorio de participacao individual (por RA)\n");
    printf("  7 - Emitir lista de presenca de um evento\n");
    printf("  0 - Sair\n");
    printf("==========================================================\n");
    printf("  Opcao: ");
}

/* --------------------------------------------------------------------------
   Funcao: op_cadastrar_evento
   O que faz: le os dados de um evento e chama ev_cadastrar, tratando os
              tres retornos possiveis.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void op_cadastrar_evento(ListaEventos *le)
{
    int  codigo, r;
    char nome[TAM_NOME_EV];
    char data[TAM_DATA];

    printf("\n--- Cadastro de evento ---\n");
    if (!ler_inteiro("  Codigo (numero inteiro): ", &codigo)) {
        printf("  Codigo invalido.\n");
        return;
    }
    printf("  Nome do evento: ");
    ler_linha(nome, sizeof(nome));
    if (nome[0] == '\0') {
        printf("  Nome nao pode ser vazio.\n");
        return;
    }
    printf("  Data (dd/mm/aaaa): ");
    ler_linha(data, sizeof(data));

    r = ev_cadastrar(le, codigo, nome, data);
    if (r == 1)       printf("  Evento cadastrado com sucesso.\n");
    else if (r == -1) printf("  ERRO: ja existe um evento com o codigo %d.\n", codigo);
    else              printf("  ERRO: memoria insuficiente.\n");
}

/* --------------------------------------------------------------------------
   Funcao: op_inscrever
   O que faz: le evento, RA e nome, e chama pa_inscrever. A mensagem de
              duplicidade sai daqui.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void op_inscrever(ListaEventos *le)
{
    int  codigo, r;
    char ra[TAM_RA];
    char nome[TAM_NOME];

    printf("\n--- Inscricao de participante ---\n");
    if (!ler_inteiro("  Codigo do evento: ", &codigo)) {
        printf("  Codigo invalido.\n");
        return;
    }
    printf("  RA: ");
    ler_linha(ra, sizeof(ra));
    if (ra[0] == '\0') {
        printf("  RA nao pode ser vazio.\n");
        return;
    }
    printf("  Nome: ");
    ler_linha(nome, sizeof(nome));
    if (nome[0] == '\0') {
        printf("  Nome nao pode ser vazio.\n");
        return;
    }

    r = pa_inscrever(le, codigo, ra, nome);
    switch (r) {
        case PA_OK:
            printf("  Inscricao realizada com sucesso.\n"); break;
        case PA_EVENTO_INEXISTENTE:
            printf("  ERRO: nao existe evento com o codigo %d.\n", codigo); break;
        case PA_JA_INSCRITO:
            printf("  ERRO: o RA %s ja esta inscrito neste evento.\n", ra); break;
        default:
            printf("  ERRO: memoria insuficiente.\n"); break;
    }
}

/* --------------------------------------------------------------------------
   Funcao: op_remover
   O que faz: le evento e RA e chama pa_remover.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void op_remover(ListaEventos *le)
{
    int  codigo, r;
    char ra[TAM_RA];

    printf("\n--- Remocao de participante ---\n");
    if (!ler_inteiro("  Codigo do evento: ", &codigo)) {
        printf("  Codigo invalido.\n");
        return;
    }
    printf("  RA: ");
    ler_linha(ra, sizeof(ra));

    r = pa_remover(le, codigo, ra);
    if (r == PA_EVENTO_INEXISTENTE)
        printf("  ERRO: nao existe evento com o codigo %d.\n", codigo);
    else if (r == PA_NAO_INSCRITO)
        printf("  ERRO: o RA %s nao esta inscrito neste evento.\n", ra);
}

/* --------------------------------------------------------------------------
   Funcao: op_por_codigo
   O que faz: pede um codigo de evento e chama a funcao de relatorio
              recebida por parametro. Evita repetir o mesmo bloco de
              leitura tres vezes no menu.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void op_por_codigo(const ListaEventos *le,
                          int (*acao)(const ListaEventos *, int))
{
    int codigo;
    if (!ler_inteiro("  Codigo do evento: ", &codigo)) {
        printf("  Codigo invalido.\n");
        return;
    }
    if (acao(le, codigo) == PA_EVENTO_INEXISTENTE) {
        printf("  ERRO: nao existe evento com o codigo %d.\n", codigo);
    }
}

/* --------------------------------------------------------------------------
   Funcao: op_relatorio_ra
   O que faz: pede um RA e emite o relatorio individual.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void op_relatorio_ra(const ListaEventos *le)
{
    char ra[TAM_RA];
    printf("\n--- Relatorio individual ---\n");
    printf("  RA: ");
    ler_linha(ra, sizeof(ra));
    if (ra[0] == '\0') {
        printf("  RA nao pode ser vazio.\n");
        return;
    }
    pa_relatorio_ra(le, ra);
}

/* --------------------------------------------------------------------------
   Funcao: carregar_exemplo
   O que faz: popula o sistema com alguns dados, so para facilitar o teste
              e a demonstracao durante a apresentacao.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
static void carregar_exemplo(ListaEventos *le)
{
    ev_cadastrar(le, 101, "Palestra: Carreira em TI",        "20/10/2026");
    ev_cadastrar(le, 102, "Minicurso de Git e GitHub",       "22/10/2026");
    ev_cadastrar(le, 103, "Oficina de Estruturas de Dados",  "25/10/2026");

    pa_inscrever(le, 101, "20231001", "Ana Souza");
    pa_inscrever(le, 101, "20231002", "Bruno Lima");
    pa_inscrever(le, 101, "20231003", "Carla Mendes");
    pa_inscrever(le, 102, "20231002", "Bruno Lima");
    pa_inscrever(le, 102, "20231004", "Diego Alves");
    pa_inscrever(le, 103, "20231001", "Ana Souza");
}

/* --------------------------------------------------------------------------
   Funcao: main
   O que faz: inicializa a lista de eventos, roda o laco do menu e, ao
              sair, destroi TODA a memoria alocada.
   Autor: Jeanluca Vergopolan - GRR20230995
   -------------------------------------------------------------------------- */
int main(void)
{
    ListaEventos le;
    char buffer[32];
    int  opcao = -1;

    ev_inicializar(&le);
    carregar_exemplo(&le);   /* comente esta linha para iniciar vazio */

    while (opcao != 0) {
        mostrar_menu();
        ler_linha(buffer, sizeof(buffer));
        if (sscanf(buffer, "%d", &opcao) != 1) {
            printf("  Opcao invalida.\n");
            continue;
        }

        switch (opcao) {
            case 1: op_cadastrar_evento(&le); break;
            case 2: ev_listar(&le);           break;
            case 3: op_inscrever(&le);        break;
            case 4: op_remover(&le);          break;
            case 5: printf("\n--- Participantes por evento ---\n");
                    op_por_codigo(&le, pa_listar_evento);  break;
            case 6: op_relatorio_ra(&le);     break;
            case 7: printf("\n--- Lista de presenca ---\n");
                    op_por_codigo(&le, pa_lista_presenca); break;
            case 0: break;
            default: printf("  Opcao invalida.\n"); break;
        }
    }

    ev_destruir(&le);   /* um free para cada malloc, inclusive os sentinelas */
    printf("\n  Memoria liberada. Sistema encerrado.\n\n");
    return 0;
}
