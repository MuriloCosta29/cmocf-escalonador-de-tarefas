#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGIN "cmocf"
#define MAX_NAME 64
#define MAX_LINE 256

typedef enum { ALG_RATE, ALG_EDF } Algorithm;

typedef struct {
  char name[MAX_NAME];
  int period;
  int deadline;
  int burst;
  int file_index;

  int is_active;
  int remaining;
  int abs_deadline;

  int complete_count;
  int lost_count;
  int killed_count;
} Task;

Task *tasks = NULL;
int n_tasks = 0;
int cap_tasks = 0;

void die(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  free(tasks);
  exit(EXIT_FAILURE);
}

/*
Amortized doubling(amortização análitica).
*/
void add_task(Task t) {
  if (n_tasks == cap_tasks) {
    if (cap_tasks == 0) {
      cap_tasks = 8;
    } else {
      cap_tasks = cap_tasks * 2;
    }

    Task *tmp = realloc(tasks, (size_t)cap_tasks * sizeof(Task));
    if (!tmp)
      die("Erro: falha de alocacao de memoria.\n");
    tasks = tmp;
  }
  tasks[n_tasks++] = t;
}

int is_space_char(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' ||
         c == '\f';
}

char *trim(char *s) {
  while (is_space_char(*s))
    s++;
  if (*s == '\0')
    return s;
  char *end = s + strlen(s) - 1;
  while (end > s && is_space_char(*end)) {
    *end = '\0';
    end--;
  }
  return s;
}

int parse_positive_int(const char *s, int *out) {
  if (s == NULL || *s == '\0')
    return 0;
  for (const char *p = s; *p; p++) {
    if (*p < '0' || *p > '9')
      return 0;
  }
  errno = 0;
  char *endptr;
  long val = strtol(s, &endptr, 10);
  if (*endptr != '\0')
    return 0;
  if (errno == ERANGE || val > INT_MAX)
    return 0;
  if (val <= 0)
    return 0;
  *out = (int)val;
  return 1;
}

int parse_input_file(const char *path, int *total_time) {
  FILE *f = fopen(path, "r");
  if (!f)
    die("Erro: nao foi possivel abrir o arquivo '%s'.\n", path);

  char line[MAX_LINE];

  if (!fgets(line, sizeof line, f))
    die("Erro: arquivo de entrada vazio.\n");

  char *first = trim(line);
  if (!parse_positive_int(first, total_time))
    die("Erro: tempo total de simulacao invalido.\n");

  int line_no = 1;
  while (fgets(line, sizeof line, f)) {
    line_no++;
    char *s = trim(line);
    if (*s == '\0')
      continue;

    char *name = strtok(s, " \t");
    char *period_str = strtok(NULL, " \t");
    char *deadline_str = strtok(NULL, " \t");
    char *burst_str = strtok(NULL, " \t");
    char *extra = strtok(NULL, " \t");

    if (!name || !period_str || !deadline_str || !burst_str || extra)
      die("Erro: numero de campos invalido na linha %d.\n", line_no);

    if (strlen(name) >= MAX_NAME)
      die("Erro: nome de tarefa muito longo na linha %d.\n", line_no);

    Task t;
    memset(&t, 0, sizeof t);
    strcpy(t.name, name);

    if (!parse_positive_int(period_str, &t.period))
      die("Erro: periodo invalido na linha %d.\n", line_no);
    if (!parse_positive_int(deadline_str, &t.deadline))
      die("Erro: deadline invalido na linha %d.\n", line_no);
    if (!parse_positive_int(burst_str, &t.burst))
      die("Erro: rajada invalida na linha %d.\n", line_no);

    if (!(t.burst <= t.deadline && t.deadline <= t.period))
      die("Erro: tarefa na linha %d nao respeita C <= D <= P.\n", line_no);

    t.file_index = n_tasks;
    add_task(t);
  }

  fclose(f);

  if (n_tasks == 0)
    die("Erro: nenhuma tarefa encontrada no arquivo de entrada.\n");

  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 3)
    die("Uso: %s <rate|edf> <arquivo_de_entrada>\n", argv[0]);

  const char *alg_str = argv[1];
  if (strcmp(alg_str, "rate") != 0 && strcmp(alg_str, "edf") != 0)
    die("Erro: algoritmo desconhecido '%s'. Use 'rate' ou 'edf'.\n", alg_str);

  return EXIT_SUCCESS;
}
