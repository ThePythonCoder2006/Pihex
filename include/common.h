#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "gmp.h"
#include "helper.h"

#ifndef __COMMON__
#define __COMMON__

#define BUFF_SIZE (1024)
#define EXT_SIZE (8)
#define CONV 3.321928

typedef struct hex_computation_s
{
  mpf_t sn, snx, an, sn_p, snx_p, an_p;
  mpf_t pi, u, t, m1, m2, tmp;
  uint64_t digits;
  timer start_timer;
  char out_base_path[BUFF_SIZE + EXT_SIZE];
  char out_path_log[BUFF_SIZE + EXT_SIZE];
  char out_path_txt[BUFF_SIZE + EXT_SIZE];
  uint8_t iter;
} hex_computation;

FILE *open_file_or_panic(const char *path, const char *mode);
int compare_files(FILE *file1, FILE *file2);
void progress_bar(const char *name, int progress, timer *start_timer);

void yellow(void);
void reset(void);

uint64_t get_num_digits(void);
uint8_t compute_num_iter(hex_computation *hex);
uint64_t compute_prec(hex_computation *hex);
void confirmation_form(hex_computation *hex);
uint8_t get_file_paths(hex_computation *hex);
void pi_write_out(char *out_path, hex_computation *hex);
void init_hex(hex_computation *hex);
void clear_hex(hex_computation *hex);

void mpf_qtrt(mpf_t rop, mpf_t op);
void mpf_ui_div_ui(mpf_t rop, unsigned int op1, unsigned int op2);
void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2, hex_computation *hex);
void mpf_mul_d(mpf_t rop, mpf_t op1, double op2, hex_computation *hex);

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

#define OUT_FILE_BASE_NAME "pihex-out"

#endif //__COMMON__

#ifdef __COMMON_IMPLEMENTATION__

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

void print_progress_bar(const char *name, float progress, float time)
{
#ifndef DEBUG
  assert(progress <= 1);

  unsigned char nb_spaces = PROG_BAR_DEC - strlen(name);

  printf("\r%s", name);
  for (unsigned char i = 0; i < nb_spaces; ++i)
    printf(" ");
  printf("[");
  for (unsigned char i = 0; i < progress * PROG_BAR_LEN; ++i)
    printf("#");
  for (unsigned char i = progress * PROG_BAR_LEN; i < PROG_BAR_LEN; ++i)
    printf("-");

  float end_ETA_time = 0;
  if (progress != 0)
    end_ETA_time = time / progress;

  // printf("%f\n", end_ETA_time);

  uint32_t end_h, end_m;
  double end_s;

  end_s = fmod(end_ETA_time, 60);
  uint32_t quotient = (uint32_t)(end_ETA_time - end_s);
  end_m = quotient % 60;
  end_h = quotient / 3600;

  uint32_t ex_t_h, ex_t_m;
  double ex_t_s;

  ex_t_s = fmod(time, 60);
  uint32_t dif_int = (uint32_t)(time - ex_t_s);
  ex_t_m = dif_int % 60;
  ex_t_h = dif_int / 3600;

  printf("]  %i%% %dh%dm%.1fs/%dh%dm%.1fs (ETA)", (int)(100U * progress),
         ex_t_h, ex_t_m, ex_t_s,
         end_h, end_m, end_s);
  fflush(stdout);
#else
  (void)name, (void)progress;
#endif
  return;
}

uint64_t get_num_digits(void)
{
  uint64_t digits;
  printf("Enter the number of digits you want to calculate : ");
  scanf("%" PRIu64 "%*c", &digits);
  return digits;
}

uint8_t compute_num_iter(hex_computation *hex)
{
  uint8_t iter = (uint8_t)(log(hex->digits) / log(16)) + 1;
  assert(iter >= 1);
  return iter;
}

uint64_t compute_prec(hex_computation *hex)
{
  return (uint64_t)round(hex->digits * CONV) + 200;
}

void confirmation_form(hex_computation *hex)
{
  printf("do you want to start calculating pi with an accuraty of %" PRIu64
         " digits, using %" PRIu64
         " bits per number and do %i iteration of the formula [Y/N] :",
         hex->digits, (uint64_t)mpf_get_default_prec(), hex->iter);
  fflush(stdout);
  char ans;
  scanf("%c", &ans);
  fflush(stdin);

  if (ans != 'y' && ans != 'Y')
  {
    printf("terminating the program.");
    exit(0);
  }
  return;
}

uint8_t get_file_paths(hex_computation *hex)
{
  time_t time_tmp = 0;
  time(&time_tmp);
  struct tm *local = localtime(&time_tmp);

  /* variables to store the date and time components*/
  int hours, minutes, day, month, year;

  hours = local->tm_hour;       // get hours since midnight (0-23)
  minutes = local->tm_min;      // get minutes passed after the hour (0-59)
  day = local->tm_mday;         // get day of month (1 to 31)
  month = local->tm_mon + 1;    // get month of year (0 to 11)
  year = local->tm_year + 1900; // get year since 1900

  snprintf(hex->out_base_path, BUFF_SIZE,
           OUT_FILE_BASE_NAME
           "-%02d-%02d-%d-%02d-%02d-iter-%i-digits-%" PRIu64,
           day, month, year, hours, minutes, hex->iter, hex->digits);

  snprintf(hex->out_path_log, BUFF_SIZE, "./output/%s.log", hex->out_base_path);
  snprintf(hex->out_path_txt, BUFF_SIZE, "./output/%s.txt", hex->out_base_path);
  return 0;
}

void end_log(hex_computation *hex)
{
  double time_taken = timer_stop(&hex->start_timer);
  printf("[INFO] the computation took %.3f seconds\n", time_taken);

  FILE *out_log = open_file_or_panic(hex->out_path_log, "w");

  // TODO: rework the correctness princple to function again, as I do not have the old `pi copy.txt` which served as a reference

  /* char correct_pi_path[] = "pi copy.txt";
   *   FILE *correct_pi = open_file_or_panic(correct_pi_path, "r");
   *   FILE *our_pi = open_file_or_panic(out_path_txt, "r");
   *
   *   uint64_t correctness = compare_files(our_pi, correct_pi);
   *
   *   fclose(correct_pi);
   *   fclose(our_pi);
   *
   *   printf("[INFO] %" PRIu64 " decimals of pi are correct in the calculations\n", correctness);
   */
  int64_t correctness = -1;
  char correct_pi_path[] = "";
  fprintf(out_log, "Calculation of %" PRIu64 " digits of pi\n"
                   "\n"
                   "Time taken : %lfs\n"
                   "\n"
                   "%" PRId64 " digits of pi coincinded with \"%s\"\n",
          hex->digits, time_taken, correctness, correct_pi_path);

  fclose(out_log);
}

void pi_write_out(char *out_path, hex_computation *hex)
{
  FILE *out = open_file_or_panic(out_path, "ab+");
  if (out == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open file %s: %s\n", out_path, strerror(errno));
    return;
  }
  gmp_fprintf(out, "%.*Ff\n", hex->digits, hex->pi);

  fclose(out);
  printf("[INFO] pi was successfully written to %s\n", out_path);
  return;
}

void init_hex(hex_computation *hex)
{
  hex->digits = get_num_digits();
  hex->iter = compute_num_iter(hex);
  mpf_set_default_prec(compute_prec(hex));
  confirmation_form(hex);

  get_file_paths(hex);

  timer_start(&hex->start_timer);

  // setting up global variables
  //----------------------------------------------------------------
  mpf_inits(hex->sn, hex->snx, hex->an, (mpf_ptr)NULL);
  mpf_inits(hex->sn_p, hex->snx_p, hex->an_p, hex->pi, (mpf_ptr)NULL);
  mpf_inits(hex->u, hex->t, hex->m1, hex->m2, hex->tmp, (mpf_ptr)NULL);

  // initialize the values of sn and an
  //----------------------------------
  mpf_sqrt_ui(hex->sn, 2);
  mpf_sub_ui(hex->sn, hex->sn, 1);

  mpf_ui_div_ui(hex->an, 1, 3);
  return;
}

void clear_hex(hex_computation *hex)
{
  mpf_clear(hex->pi);
  mpf_clears(hex->sn, hex->snx, hex->an, (mpf_ptr)NULL);
  mpf_clears(hex->sn_p, hex->snx_p, hex->an_p, (mpf_ptr)NULL);
  mpf_clears(hex->u, hex->t, hex->m1, hex->m2, hex->tmp, (mpf_ptr)NULL);
  return;
}

void PB_write_out(char *fname, float *times, size_t len)
{
  FILE *f = fopen(fname, "wb");
  if (f == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open file %s: %s", fname, strerror(errno));
    return;
  }
  fwrite(&len, sizeof(len), 1, f);
  fwrite(times, sizeof(times[0]), len, f);

  return;
}

void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2, hex_computation *hex)
{
  mpf_set_si(hex->tmp, op2);
  printf_debug("op1 = %.60Ff\n op2 = %.60Ff\n", op1, tmp);
  mpf_mul(rop, hex->tmp, op1);
}

void mpf_mul_d(mpf_t rop, mpf_t op1, double op2, hex_computation *hex)
{
  mpf_set_d(hex->tmp, op2);
  mpf_mul(rop, op1, hex->tmp);
}

void mpf_qtrt(mpf_t rop, mpf_t op)
{
  mpf_sqrt(rop, op);
  mpf_sqrt(rop, rop);
}

void mpf_ui_div_ui(mpf_t rop, unsigned int op1, unsigned int op2)
{
  mpf_set_ui(rop, op1);
  mpf_div_ui(rop, rop, op2);
}

#endif