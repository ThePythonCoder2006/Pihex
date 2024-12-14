#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include <gmp.h>

#define __COMMON_IMPLEMENTATION__
#include "common.h"

#define __HELPER_IMPLEMENTATION__
#include "helper.h"

#define __TIMES_IMPLEMENTATION__
#include "times.h"

#include "logging.h"

/*
colors:
Black  \033[0;30m
Red    \033[0;31m
Green  \033[0;32m
Yellow \033[0;33m
Blue   \033[0;34m
Purple \033[0;35m
Cyan   \033[0;36m
White  \033[0;37m
reset  \033[0m
*/

//--------------------------------------------------------------------------------------------

void mpf_sn(int n, hex_computation *hex);
void mpf_snx(int n, hex_computation *hex);
void mpf_an(int n, hex_computation *hex);

//--------------------------------------------------------------------------------------------

/*
ITER = 2 : 7
ITER = 3 : 170     > 16 * 7      = 112
ITER = 4 : 2470    < 16 * 170    = 2720
ITER = 5 : 35700   < 16 * 2470   = 39520
ITER = 6 : 514770  < 16 * 35700  = 571200
ITER = 7 : 8200000 ? 16 * 514770 = 8236320
*/

int main(int argc, char **argv)
{
  (void)argc, (void)argv;

  hex_computation main = {0};

  init_hex(&main);

  //----------------------------------

  for (int i = 1; i <= main.iter; ++i)
  {
    mpf_snx(i, &main);

    mpf_an(i, &main);

    if (i != main.iter)
      mpf_sn(i + 1, &main);
  }

  printf("\n\n[INFO] main:\n");

  printf_debug("an: %.100Ff\n", main.an);

  mpf_ui_div(main.pi, 1, main.an);

  pi_write_out(main.out_path_txt, &main);

  end_log(&main);

  printf_debug("PI = %.100Rf\n", main.pi);

  printf("[INFO] End\n");

  clear_hex(&main);

  return EXIT_SUCCESS; // return 0
}

//--------------------------------------------------------------------------------------------

// expects "progress" to be an already existing integer in scope, which would start at zero at the start of the function
#define PB(name, timer, times)                       \
  *times = (float)timer_get_time(timer);             \
  print_progress_bar(name, table[progress], *times); \
  ++times;                                           \
  ++progress;

#define TIMES_LEN 25

void mpf_sn(int n, hex_computation *hex)
{
  printf_debug("\n[INFO] S%i\n", n);

  assert(n <= hex->iter);

  if (n < 1)
  {
    fprintf(stderr, "[ERROR] could not proced (n <= 1)");
    exit(EXIT_FAILURE);
  }

  char name[10];
  sprintf(name, "S%i", n);
  float times[TIMES_LEN] = {0.0};
  float *times_ptr = times;
  size_t progress = 0;
  float *table = sn_times;
  timer exec_time = {0};
  timer_start(&exec_time);

  mpf_swap(hex->sn, hex->sn_p); // store the previous sn in the right varial and so frees the
  PB(name, &exec_time, times_ptr);

  // consts --------------

  __mpf_mul(hex->u, hex->snx, hex->snx);
  __mpf_add_ui(hex->u, hex->u, 1);
  __mpf_mul(hex->u, hex->u, hex->snx);
  __mpf_mul_ui(hex->u, hex->u, 8);
  __mpf_qtrt(hex->u, hex->u);

  __mpf_init(hex->t);
  __mpf_add_ui(hex->t, hex->snx, 1);

  //--------------

  __mpf_ui_sub(hex->sn, 1, hex->snx);
  __mpf_pow_ui(hex->sn, hex->sn, 4);

  mpf_t mul;
  __mpf_init(mul);

  __mpf_add(mul, hex->t, hex->u);
  __mpf_mul(mul, mul, mul);
  __mpf_div(hex->sn, hex->sn, mul);

  __mpf_mul(hex->tmp, hex->u, hex->u);
  __mpf_mul(mul, hex->t, hex->t);
  __mpf_add(mul, mul, hex->tmp);

  __mpf_div(hex->sn, hex->sn, mul);
  __mpf_clear(mul);

  printf_debug("s%i:  %.60Ff\n", n, hex->sn);

  printf("\n");

  char out_path[BUFF_SIZE] = {0};
  snprintf(out_path, BUFF_SIZE, "./src/times/sn/%s-%s-times.bin", hex->out_base_path, name);
  PB_write_out(out_path, times, progress);

  printf_debug("[INFO] End S%i\n", n);
}

void mpf_snx(int n, hex_computation *hex)
{
  printf_debug("\n[INFO] S%iX\n", n);

  assert(n <= hex->iter);
  assert(n >= 0);

  char name[10];
  sprintf(name, "S%iX", n);
  float times[100] = {0.0};
  float *times_ptr = times;
  size_t progress = 0;
  float *table = snx_times;
  timer exec_time;
  timer_start(&exec_time);

  __mpf_swap(hex->snx, hex->snx_p);

  __mpf_pow_ui(hex->snx, hex->sn, 4);
  __mpf_ui_sub(hex->snx, 1, hex->snx);

  __mpf_qtrt(hex->snx, hex->snx);

  printf_debug("s%ix: %.60Ff\n", n, hex->snx);

  printf("\n");

  char out_path[BUFF_SIZE] = {0};
  snprintf(out_path, BUFF_SIZE, "./src/times/snx/%s-%s-times.bin", hex->out_base_path, name);
  PB_write_out(out_path, times, progress);

  printf_debug("End S%iX\n", n);
}

void mpf_an(int n, hex_computation *hex)
{
  printf_debug("A%i\n", n);
  assert(n <= hex->iter);

  if (n <= 0)
  {
    fprintf(stderr, "[ERROR] could not proced (n <= 0)");
    exit(EXIT_FAILURE); // exit(1)
  }

  char name[10];
  sprintf(name, "A%i", n);
  float times[100] = {0.0};
  float *times_ptr = times;
  size_t progress = 0;
  float *table = an_times;
  timer exec_time;
  timer_start(&exec_time);

  __mpf_swap(hex->an, hex->an_p);

  __mpf_add_ui(hex->t, hex->snx, 1);

  printf_debug("s%ix: %.60Ff\n", n, hex->snx);

  __mpf_add_ui(hex->m1, hex->sn, 1);
  __mpf_div(hex->m1, hex->m1, hex->t);
  __mpf_pow_ui(hex->m1, hex->m1, 4);

  __mpf_pow_ui(hex->m2, hex->t, 4);
  __mpf_ui_div(hex->m2, 1, hex->m2);

  printf_debug("a%i: %.60Ff\nt : %.60Ff\nm1 : %.60Ff\nm2 : %.60Ff\n", n - 1, hex->an_p, hex->t, hex->m1, hex->m2);

  PB(name, &exec_time, times_ptr);

  __mpf_mul_si(hex->an, hex->m1, -4, hex);
  __mpf_mul_si(hex->tmp, hex->m2, -12, hex);
  __mpf_add(hex->an, hex->tmp, hex->an);
  __mpf_add_ui(hex->an, hex->an, 1);

  printf_debug("PLUS2: %.60Ff\n", hex->an);

  mpz_t pow;
  __mpz_init(pow);
  __mpz_ui_pow_ui(pow, 4, (2 * n) - 1);
  __mpf_set_z(hex->tmp, pow);
  __mpf_div_ui(hex->tmp, hex->tmp, 3);
  printf_debug("PLUS1: %Ff\n", hex->tmp);

  __mpz_clear(pow);

  __mpf_mul(hex->an, hex->an, hex->tmp);
  __mpf_neg(hex->an, hex->an);

  printf_debug("PLUS: %.60Ff\n", hex->an);

  __mpf_mul_ui(hex->tmp, hex->m1, 16);
  __mpf_mul(hex->tmp, hex->tmp, hex->an_p);

  printf_debug("start: %.60Ff\n", hex->tmp);

  __mpf_sub(hex->an, hex->tmp, hex->an);

  printf_debug("a%i: %.60Ff\n", n, hex->an);

  printf("\n");

  char out_path[BUFF_SIZE] = {0};
  snprintf(out_path, BUFF_SIZE, "./src/times/an/%s-%s-times.bin", hex->out_base_path, name);
  PB_write_out(out_path, times, progress);

  printf_debug("End A%i\n", n);
}
