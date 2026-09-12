#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOGIN "cmocf"
#define MAX_NAME 64

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

int main(void) { return EXIT_SUCCESS; }
