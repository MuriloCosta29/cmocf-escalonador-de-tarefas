# Escalonador de Tarefas Criticas de Voo

Implementacao 3 - Infraestrutura de Software (CESAR School).

## Arquivos .c utilizados

- **main.c**: unico arquivo-fonte do projeto. Contem a leitura e validacao
  do arquivo de entrada, a simulacao dos dois algoritmos de escalonamento
  (Rate Monotonic e Earliest Deadline First) e a geracao do relatorio de
  saida.

## Como compilar

```
make
```

Gera um unico executavel chamado `scheduler` no diretorio atual (`make
clean` remove o executavel, objetos e os arquivos `.out` gerados).

## Como executar

```
./scheduler rate <arquivo_de_entrada>
./scheduler edf  <arquivo_de_entrada>
```

O resultado e gravado no diretorio atual em `rate_cmocf.out` ou
`edf_cmocf.out` (o login usado, `cmocf`, esta definido na constante
`LOGIN` no topo de `main.c`). Nada e impresso em stdout durante a
execucao normal.

## Como testei

Usei o proprio exemplo do enunciado (`tests/voo.txt`, incluido neste
repositorio):

```
100
ATT 20 12 8
NAV 50 30 15
```

- `./scheduler rate tests/voo.txt` reproduz **exatamente**, linha a
  linha, o trace de exemplo dado no enunciado (incluindo a perda de
  deadline do NAV em t=30 e os contadores finais).
- `./scheduler edf tests/voo.txt` roda o mesmo cenario sob EDF.

Alem disso, testei manualmente casos de borda nao cobertos pelo exemplo
(tarefa ainda em execucao/preemptada no fim da simulacao, deadline
perdido enquanto preemptada, rajada igual ao periodo) e todos os casos
de entrada invalida pedidos no enunciado (numero incorreto de
argumentos, algoritmo desconhecido, arquivo inexistente, campo
faltando, valor nao numerico, valor nao positivo, violacao de
`C <= D <= P`) - todos terminam com mensagem coerente em stderr, codigo
de saida diferente de zero e sem criar arquivo `.out`.

O log dessas execucoes de teste (compilacao e rodadas manuais) esta em
`evidencias.log`, gerado com `script -a evidencias.log`.

## Sistema operacional

Desenvolvido e testado em macOS, compilado com `gcc` via `make`. O
codigo nao usa nenhuma extensao especifica de plataforma (apenas C11
padrao e a biblioteca padrao do C), portanto compila e roda sem
alteracoes em Linux tambem.
