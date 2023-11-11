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
#include <mpfr.h>

#define __COMMON_IMPLEMENTATION__
#include "common.h"

#define __HELPER_IMPLEMENTATION__
#include "helper.h"

#ifdef MPFR
#endif

//--------------------------------------------------------------------------------------------

void mpfr_qtrt(mpfr_t rop, mpfr_t op, mpfr_rnd_t round);
void mpfr_ui_div_ui(mpfr_t rop, unsigned int op1, unsigned int op2, mpfr_rnd_t round);

void mpfr_sn(int n, mpfr_prec_t prec, mpfr_rnd_t round);
void mpfr_snx(int n, mpfr_rnd_t round);
void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round);

//--------------------------------------------------------------------------------------------

/*
ITER = 2 : 7
ITER = 3 : 170     > 16 * 7      = 112
ITER = 4 : 2470    < 16 * 170    = 2720
ITER = 5 : 35700   < 16 * 2470   = 39520
ITER = 6 : 514770  < 16 * 35700  = 571200
ITER = 7 : 8200000 ? 16 * 514770 = 8236320
*/

#define CONV 3.321928

#define PROG_BAR_LEN 25

#define PROG_BAR_DEC 5

mpfr_t sn, snx, an;
mpfr_t sn_p, snx_p, an_p;

uint64_t digits, prec;
unsigned char iter;

int main(void)
{
  PIHEX_INIT

  // setting up my global variables
  //----------------------------------------------------------------
  mpfr_inits2(prec, sn, snx, an, (mpfr_ptr)0);
  mpfr_inits2(prec, sn_p, snx_p, an_p, (mpfr_ptr)0);

  mpfr_t pi;
  mpfr_init2(pi, prec);

  // initialize the value of sn to s1 (sqrt(2) - 1)
  //----------------------------------
  mpfr_sqrt_ui(sn, 2, 0);
  mpfr_sub_ui(sn, sn, 1, 0);

  // initialize the value of an to a0 (1/3)
  //----------------------------------
  mpfr_ui_div_ui(an, 1, 3, 0);

  //----------------------------------

  for (int i = 1; i <= iter; ++i)
  {
    mpfr_snx(i, 0);

    mpfr_an(i, prec, 0);

    if (i != iter)
      mpfr_sn(i + 1, prec, 0);
  }

  printf("\n\n[INFO] main:\n");

  printf_debug("an: %.100Rf\n", an);

  mpfr_ui_div(pi, 1, an, 0);

  FILE *out_txt = open_file_or_panic(out_path_txt, "w");
  mpfr_fprintf(out_txt, "%.*Rf\n", digits, pi);
  fclose(out_txt);

  PIHEX_END

  mpfr_clear(pi);
  mpfr_clears(sn, snx, an, (mpfr_ptr)NULL);
  mpfr_clears(sn_p, snx_p, an_p, (mpfr_ptr)NULL);
  return EXIT_SUCCESS; // return 0
}

//--------------------------------------------------------------------------------------------

void mpfr_qtrt(mpfr_t rop, mpfr_t op, mpfr_rnd_t round)
{
  mpfr_rootn_ui(rop, op, 4, round);
}

void mpfr_ui_div_ui(mpfr_t rop, unsigned int op1, unsigned int op2, mpfr_rnd_t round)
{
  mpfr_set_ui(rop, op1, round);
  mpfr_div_ui(rop, rop, op2, round);
}

void mpfr_sn(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
  printf_debug("\n[INFO] S%i\n", n);

  assert(n <= iter);

  if (n < 1)
  {
    fprintf(stderr, "[ERROR] could not proced (n <= 1)\n");
    exit(EXIT_FAILURE);
  }

  char sn_name[10];
  sprintf(sn_name, "S%i", n);

  // progress_bar(sn_name, 0);

  mpfr_swap(sn, sn_p); // store the previous sn in the right varial and so frees the

  // progress_bar(sn_name, 5);

  // consts --------------

  mpfr_t u;
  mpfr_init2(u, prec);
  mpfr_sqr(u, snx, round);
  mpfr_add_ui(u, u, 1, round);
  mpfr_mul(u, u, snx, round);
  mpfr_mul_ui(u, u, 8, round);
  mpfr_qtrt(u, u, round);

  // progress_bar(sn_name, 40);

  mpfr_t t;
  mpfr_init2(t, prec);
  mpfr_add_ui(t, snx, 1, round);

  // progress_bar(sn_name, 65);

  //--------------

  mpfr_ui_sub(sn, 1, snx, round);
  mpfr_pow_ui(sn, sn, 4, round);

  // progress_bar(sn_name, 75);

  mpfr_t mul, tmp;
  mpfr_inits2(prec, mul, tmp, (mpfr_ptr)0);

  mpfr_add(mul, t, u, round);
  mpfr_sqr(mul, mul, round);
  mpfr_div(sn, sn, mul, round);

  // progress_bar(sn_name, 85);

  mpfr_sqr(tmp, u, round);
  mpfr_sqr(mul, t, round);
  mpfr_add(mul, mul, tmp, round);

  // progress_bar(sn_name, 90);

  mpfr_div(sn, sn, mul, round);

  mpfr_clears(mul, tmp, (mpfr_ptr)0);

  //--------------
  mpfr_clear(u);
  mpfr_clear(t);

  printf_debug("s%i:  %.60Rf\n", n, sn);

  // progress_bar(sn_name, 100);
  // printf("\n");

  printf_debug("End S%i\n", n);
}

void mpfr_snx(int n, mpfr_rnd_t round)
{
  printf_debug("S%iX\n", n);

  assert(n <= iter);
  assert(n >= 0);

  char snx_name[10];
  sprintf(snx_name, "S%iX", n);

  // progress_bar(snx_name, 0);

  mpfr_swap(snx, snx_p);

  // progress_bar(snx_name, 5);

  mpfr_pow_ui(snx, sn, 4, round);
  mpfr_ui_sub(snx, 1, snx, round);

  // progress_bar(snx_name, 50);

  mpfr_qtrt(snx, snx, round);
  printf_debug("s%ix: %.60Rf\n", n, snx);

  // progress_bar(snx_name, 100);
  // printf("\n");

  printf_debug("End S%iX\n", n);
}

void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
  printf_debug("A%i\n", n);
  assert(n <= iter);

  if (n <= 0)
  {
    fprintf(stderr, "[ERROR] could not proced (n <= 0)");
    exit(EXIT_FAILURE); // exit(1)
  }

  char an_name[10];
  sprintf(an_name, "A%i", n);

  // progress_bar(an_name, 0);

  mpfr_swap(an, an_p);

  // progress_bar(an_name, 5);

  mpfr_t t, m1, m2;
  mpfr_inits2(prec, t, m1, m2, (mpfr_ptr)0);

  // progress_bar(an_name, 10);

  mpfr_add_ui(t, snx, 1, round);

  // progress_bar(an_name, 12);

  printf_debug("s%ix: %.60Rf\n", n, snx);

  mpfr_add_ui(m1, sn, 1, round);
  mpfr_div(m1, m1, t, round);
  mpfr_pow_ui(m1, m1, 4, round);

  // progress_bar(an_name, 20);

  mpfr_pow_si(m2, t, -4, round);

  // progress_bar(an_name, 25);

  printf_debug("a%i: %.60Rf\nt : %.60Rf\nm1 : %.60Rf\nm2 : %.60Rf\n", n - 1, an_p, t, m1, m2);

  mpfr_t tmp;
  mpfr_init2(tmp, prec);

  // progress_bar(an_name, 30);

  mpfr_mul_si(an, m1, -4, round);
  mpfr_mul_si(tmp, m2, -12, round);
  mpfr_add(an, tmp, an, round);
  mpfr_add_ui(an, an, 1, round);
  // printf_debug("PLUS2: %.60Rg\n", an_c[n]);

  // progress_bar(an_name, 50);

  mpfr_ui_pow_ui(tmp, 4, (2 * n) - 1, round);
  mpfr_div_ui(tmp, tmp, 3, round);
  // printf_debug("PLUS1: %.60Rg\n", tmp);

  // progress_bar(an_name, 65);

  mpfr_mul(an, an, tmp, round);
  mpfr_neg(an, an, round);
  // printf_debug("PLUS: %.60Rf\n", an_c[n]);

  // progress_bar(an_name, 80);

  mpfr_mul_ui(tmp, m1, 16, round);
  mpfr_mul(tmp, tmp, an_p, round);
  // printf_debug("start: %.60Rf\n", start);

  // progress_bar(an_name, 95);

  mpfr_sub(an, tmp, an, round);
  // printf_debug("an: %.60Rf\n", an_c[n]);

  mpfr_clear(tmp);
  mpfr_clears(t, m1, m2, (mpfr_ptr)0);
  printf_debug("a%i: %.60Rf\n", n, an);

  // progress_bar(an_name, 100);
  // printf("\n");

  printf_debug("End A%i\n", n);
}
