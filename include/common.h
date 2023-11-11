#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "helper.h"

#ifndef __COMMON__
#define __COMMON__

FILE *open_file_or_panic(const char *path, const char *mode);
int compare_files(FILE *file1, FILE *file2);
void progress_bar(const char *name, int progress);

void yellow(void);
void reset(void);

extern timer start_timer;

#ifdef DEBUG
#ifndef mpfr_version
#define printf_debug(format, ...)                             \
  do                                                          \
  {                                                           \
    yellow();                                                 \
    gmp_printf("[DEBUG] " format __VA_OPT__(, ) __VA_ARGS__); \
    reset();                                                  \
  } while (0)
#else
#define printf_debug(format, ...)                              \
  do                                                           \
  {                                                            \
    yellow();                                                  \
    mpfr_printf("[DEBUG] " format __VA_OPT__(, ) __VA_ARGS__); \
    reset();                                                   \
  } while (0)
#endif
#else
#define printf_debug(format, ...)
#endif

#ifdef mpfr_version
#define OUT_FILE_BASE_NAME "pihex-out-mpfr"
#else
#define OUT_FILE_BASE_NAME "pihex-out-gmp"
#endif

#define BUFF_SIZE (1024)
#define EXT_SIZE (8)

#define PIHEX_INIT                                                            \
  /* getting iter and prec variables                                          \
  ----------------------------------------------------------------*/          \
  printf("Enter the number of digits you want to calculate : ");              \
  scanf("%" PRIu64 "%*c", &digits);                                           \
                                                                              \
  double prec_temp, tmp;                                                      \
  prec_temp = log(digits);                                                    \
                                                                              \
  tmp = log(16);                                                              \
                                                                              \
  prec_temp /= tmp;                                                           \
                                                                              \
  iter = (uint8_t)(prec_temp) + 1;                                            \
                                                                              \
  prec_temp = (uint64_t)digits;                                               \
  prec_temp *= CONV;                                                          \
  prec = (uint64_t)round(prec_temp) + 200;                                    \
                                                                              \
  assert(iter >= 1);                                                          \
                                                                              \
  /* I/O                                                                      \
  ----------------------------------------------------------------*/          \
  printf("do you want to start calculating pi with an accuraty of %" PRIu64   \
         " digits, using %" PRIu64                                            \
         " bits per number and do %i iteration of the formula [Y/N] :\n",     \
         digits, prec, iter);                                                 \
  fflush(stdout);                                                             \
  char ans;                                                                   \
  scanf("%c", &ans);                                                          \
  fflush(stdin);                                                              \
                                                                              \
  if (ans != 'y' && ans != 'Y')                                               \
  {                                                                           \
    printf("terminating the program.");                                       \
    exit(0);                                                                  \
  }                                                                           \
                                                                              \
  /* getting time                                                             \
  ----------------------------------------------------------------*/          \
  timer_start(&start_timer);                                                  \
  time_t time_tmp = 0;                                                        \
  time(&time_tmp);                                                            \
  struct tm *local = localtime(&time_tmp);                                    \
                                                                              \
  /* variables to store the date and time components*/                        \
  int hours, minutes, seconds, day, month, year;                              \
                                                                              \
  hours = local->tm_hour;       /* get hours since midnight (0-23)*/          \
  minutes = local->tm_min;      /* get minutes passed after the hour (0-59)*/ \
  seconds = local->tm_sec;      /* get seconds passed after a minute (0-59)*/ \
  day = local->tm_mday;         /* get day of month (1 to 31)*/               \
  month = local->tm_mon + 1;    /* get month of year (0 to 11)*/              \
  year = local->tm_year + 1900; /* get year since 1900*/                      \
                                                                              \
  char out_path_no_ext[BUFF_SIZE + EXT_SIZE] = {0};                           \
  snprintf(out_path_no_ext, BUFF_SIZE,                                        \
           "output/" OUT_FILE_BASE_NAME                                       \
           "-%02d-%02d-%d-%02d-%02d-%02d-iter-%i-digits-%" PRIu64,            \
           day, month, year, hours, minutes, seconds, iter, digits);          \
                                                                              \
  char out_path_log[BUFF_SIZE + EXT_SIZE] = {0};                              \
  strcpy(out_path_log, out_path_no_ext);                                      \
  strcat(out_path_log, ".log");                                               \
                                                                              \
  char *out_path_txt = out_path_no_ext;                                       \
  strcat(out_path_txt, ".txt");

#define PIHEX_END                                                                             \
                                                                                              \
  char correct_pi_path[] = "pi copy.txt";                                                     \
  FILE *correct_pi = open_file_or_panic(correct_pi_path, "r");                                \
  FILE *our_pi = open_file_or_panic(out_path_txt, "r");                                       \
                                                                                              \
  uint64_t correctness = compare_files(our_pi, correct_pi);                                   \
                                                                                              \
  fclose(correct_pi);                                                                         \
  fclose(our_pi);                                                                             \
                                                                                              \
  printf("[INFO] %" PRIu64 " decimals of pi are correct in the calculations\n", correctness); \
                                                                                              \
  /* time calculations*/                                                                      \
  double time_taken = timer_stop(&start_timer);                                               \
  printf("[INFO] it took %.3f seconds\n", time_taken);                                        \
                                                                                              \
  FILE *out_log = open_file_or_panic(out_path_log, "w");                                      \
                                                                                              \
  fprintf(out_log, "Calculation of %" PRIu64 " digits of pi\n"                                \
                   "\n"                                                                       \
                   "Time taken : %lfs\n"                                                      \
                   "\n"                                                                       \
                   "%" PRIu64 " digits of pi coincinded with \"%s\"\n",                       \
          digits, time_taken, correctness, correct_pi_path);                                  \
                                                                                              \
  fclose(out_log);                                                                            \
                                                                                              \
  printf_debug("PI = %.100Rf\n", pi);                                                         \
                                                                                              \
  printf("[INFO] End\n");

#endif //__COMMON__

#ifdef __COMMON_IMPLEMENTATION__

timer start_timer;

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

  uint32_t ex_t_h, ex_t_m;
  double ex_t_s;

  double t = timer_get_time(&start_timer);

  ex_t_s = fmod(t, 60);
  uint32_t dif_int = (uint32_t)(t - ex_t_s);
  ex_t_m = dif_int % 60;
  ex_t_h = dif_int / 3600;

  printf("|  %i%% %dh%dm%.3fs", progress, ex_t_h, ex_t_m, ex_t_s);
  fflush(stdout);
#else
  (void)name, (void)progress;
#endif
  return;
}

#endif