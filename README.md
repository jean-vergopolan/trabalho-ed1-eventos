# Sistema de Controle de Participação em Eventos Estudantis

Trabalho Prático I da disciplina **Estruturas de Dados I (DS130)**
Tecnologia em Análise e Desenvolvimento de Sistemas · UFPR
Prof. Helcio Soares Padilha Junior

**Autoria:** Jeanluca Vergopolan - GRR20230995
Trabalho individual. Todas as funções estão identificadas com comentário de autoria no próprio código.

---

## O que o sistema faz

Aplicação em C, de linha de comando, que gerencia os eventos do setor de extensão e os estudantes inscritos em cada um.

Funcionalidades implementadas:

| # | Funcionalidade | Onde está |
|---|---|---|
| 1 | Cadastrar evento (código, nome, data) | `ev_cadastrar` em `eventos.c` |
| 2 | Listar eventos cadastrados | `ev_listar` em `eventos.c` |
| 3 | Inscrever participante (RA, nome, evento) | `pa_inscrever` em `participantes.c` |
| 4 | Impedir inscrição duplicada | `pa_inscrever` em `participantes.c` |
| 5 | Listar participantes por evento | `pa_listar_evento` em `participantes.c` |
| 6 | Remover participante de um evento | `pa_remover` em `participantes.c` |
| 7 | Relatório de participação individual por RA | `pa_relatorio_ra` em `participantes.c` |
| 8 | Emitir lista de presença de um evento | `pa_lista_presenca` em `participantes.c` |

---

## Como compilar e executar

```bash
gcc -Wall -Wextra -pedantic -std=c11 -o sistema \
    main.c lista.c eventos.c participantes.c

./sistema
```

Verificação de memória:

```bash
valgrind --leak-check=full ./sistema
```

ou, se o Valgrind não estiver disponível:

```bash
gcc -Wall -Wextra -pedantic -std=c11 -g -fsanitize=address \
    -o sistema_asan main.c lista.c eventos.c participantes.c
./sistema_asan
```

O programa inicia com alguns eventos e inscrições de exemplo, para facilitar o teste. Para começar com o sistema vazio, comente a chamada de `carregar_exemplo` no `main`.

---

## Organização dos arquivos

```
lista.h / lista.c            TAD Lista de Participantes
                             (encadeada COM NÓ CABEÇALHO)

eventos.h / eventos.c        TAD Lista de Eventos
                             (encadeada SIMPLES)
                             Cada evento carrega a sua própria
                             ListaParticipantes por valor.

participantes.h / .c         Regras de negócio: inscrição, remoção
                             e os três relatórios. Não implementa
                             estrutura nenhuma, só usa a interface
                             pública dos dois TADs acima.

main.c                       Menu e leitura de entrada.
```

A separação segue o princípio de TAD visto na primeira aula: o `.h` é a **interface** (diz o quê), o `.c` é a **implementação** (diz como). Funções auxiliares internas (`criar_no`, `ler_linha`, `ler_inteiro`) estão marcadas como `static` justamente para não vazarem para fora do módulo. Todos os cabeçalhos têm guarda de inclusão.

---

## Escolha das estruturas e justificativa

O trabalho pede duas coleções diferentes, e cada uma recebeu a estrutura que o seu padrão de uso justifica.

### Participantes de um evento: lista encadeada com nó cabeçalho

As operações dominantes sobre participantes são **inscrever** e **remover**. Numa lista encadeada tradicional, remover o primeiro elemento exige um caminho de código separado, porque ele altera o próprio ponteiro `inicio` da lista. O mesmo vale para inserir na posição 0.

Com o nó cabeçalho, **sempre existe um nó anterior**, que no pior caso é o próprio sentinela. Isso elimina todos os `if (lista vazia)` e todos os casos especiais de primeiro elemento. Em `lp_remover_ra`, o ponteiro `anterior` simplesmente começa no cabeçalho, e remover o primeiro inscrito usa exatamente o mesmo código que remover qualquer outro.

O custo é um único nó extra por evento, de tamanho fixo, independente de a lista ter 3 ou 3000 inscritos. A complexidade assintótica não muda. O ganho é simplicidade e menos pontos onde errar.

### Eventos: lista encadeada simples

A quantidade de eventos do setor de extensão é imprevisível, então uma lista estática obrigaria a escolher um `MAX` arbitrário, com risco de estouro ou desperdício de memória.

Sobre eventos o sistema faz basicamente duas coisas: cadastrar (uma vez) e buscar por código (o tempo todo, em praticamente toda operação). Não existe remoção frequente de eventos nem necessidade de percorrer de trás para frente, então o ponteiro extra de uma lista duplamente encadeada seria custo sem benefício. A lista encadeada simples é o menor recurso que resolve o problema.

---

## Complexidade das operações

| Operação | Custo | Por quê |
|---|---|---|
| `lp_esta_vazia`, `lp_tamanho` | O(1) | só consultam um campo |
| `lp_buscar_ra` | O(p) | busca sequencial entre os inscritos |
| `lp_inserir_fim` | O(p) | percorre até o último nó |
| `lp_remover_ra` | O(p) | percorre até achar o anterior |
| `ev_buscar` | O(e) | busca sequencial entre os eventos |
| `ev_cadastrar` | O(e) | verifica duplicidade e insere no fim |
| `pa_inscrever` | O(e + p) | busca o evento, depois checa o RA |
| `pa_relatorio_ra` | O(e · p) | para cada evento, busca o RA na lista dele |

Sendo **e** o número de eventos e **p** o número de inscritos no evento em questão.

---

## Invariantes das estruturas

**Lista de participantes (com cabeçalho)**

- `cabecalho` nunca é `NULL` depois de `lp_inicializar`
- o primeiro participante real é sempre `cabecalho->proximo`
- lista logicamente vazia significa `cabecalho->proximo == NULL`
- o campo `tamanho` conta apenas os participantes reais, o sentinela não entra

**Lista de eventos (simples)**

- lista vazia significa `inicio == NULL`
- o campo `proximo` do último evento vale `NULL`
- não existem dois eventos com o mesmo código

---

## Cuidados de memória

- Todo retorno de `malloc` é verificado antes de qualquer uso do ponteiro
- Em toda remoção, os ponteiros são religados **antes** da chamada de `free`, nunca depois
- `lp_destruir` começa o laço no próprio `cabecalho`, porque o sentinela também foi alocado com `malloc` e também precisa do seu `free`
- `ev_destruir` destrói a lista de participantes de cada evento antes de liberar o nó do evento
- Em todos os laços de destruição, o endereço do próximo nó é guardado numa variável local antes de liberar o atual

Verificado com AddressSanitizer e LeakSanitizer: **zero vazamentos e zero erros de memória**. Compila sem nenhum aviso com `-Wall -Wextra -pedantic -std=c11`.
