#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef __COMMON__
#define __COMMON__

FILE *open_file_or_panic(const char *path, const char *mode);
int compare_files(FILE *file1, FILE *file2);
void progress_bar(const char *name, int progress);

void yellow(void);
void reset(void);

extern time_t start;

#endif //__COMMON__

#ifdef __COMMON_IMPLEMENTATION__

time_t start;

FILE *open_file_or_panic(const char *path, const char *mode)
{
  FILE *f = fopen(path, mode);
  if (f == NULL)
  {
    fprintf(stderr, "could not open file '%s' because : %s\n", path, strerror(errno));
    exit(1);
  }
  return f;
}

int compare_files(FILE *file1, FILE *file2)
{
  int counter = 0;
  char c1, c2;
  while (1)
  {
    c1 = fgetc(file1);
    c2 = fgetc(file2);
    // printf("c1: %c \nc2: %c\n", c1, c2);

    if (feof(file1) || feof(file2))
      break;
    if (c1 != c2)
      break;

    ++counter;
  }
  return counter;
}

void yellow(void)
{
  printf("\033[0;33m");
  return;
}

void reset(void)
{
  printf("\033[0m");
  return;
}

#define PROG_BAR_LEN 25
#define PROG_BAR_DEC 5

void progress_bar(const char *name, int progress)
{
#ifndef DEBUG
  assert(progress <= 100);

  unsigned char nb_spaces = PROG_BAR_DEC - strlen(name);

  printf("\r%s", name);
  for (unsigned char i = 0; i < nb_spaces; ++i)
    printf(" ");
  printf("|");
  for (unsigned char i = 0; i < progress / (100 / PROG_BAR_LEN); ++i)
    printf("#");
  for (unsigned char i = progress / (100 / PROG_BAR_LEN); i < PROG_BAR_LEN; ++i)
    printf("-");

  int ex_t_h, ex_t_m, ex_t_s;

  time_t t;
  time(&t);

  int dif = (double)(t - start);
  ex_t_s = dif % 60;
  ex_t_m = (dif - ex_t_s) % 60;
  ex_t_h = (dif - ex_t_s) / 3600;

  printf("|  %i%% %dh%dm%ds", progress, ex_t_h, ex_t_m, ex_t_s);
  fflush(stdout);
#endif
  return;
}

#endif