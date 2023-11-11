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

// GO TO DIR:
//     cd C:/Users/Elève/Desktop/document/progra/pihex/base
// or  cd C:/Users/paul/Desktop/documents/programation/pihex/base

// COMPILE:
//   gcc -o bin/hex-gmp src/hex-gmp-6.2.1.c -I include -L lib -lgmp -lmpfr

// RUN :
//   ./bin/hex-gmp

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

void mpf_qtrt(mpf_t rop, mpf_t op);
void mpf_ui_div_ui(mpf_t rop, unsigned int op1, unsigned int op2);

void mpf_sn(int n, mp_bitcnt_t prec);
void mpf_snx(int n);
void mpf_an(int n, mp_bitcnt_t prec);

void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2);
void mpf_mul_d(mpf_t rop, mpf_t op1, double op2);

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

mpf_t sn, snx, an;
mpf_t sn_p, snx_p, an_p;

uint64_t digits, prec;
uint8_t iter;

int main(void)
{
  PIHEX_INIT

  // setting up my global variables
  //----------------------------------------------------------------
  mpf_init2(sn, prec);
  mpf_init2(snx, prec);
  mpf_init2(an, prec);
  mpf_init2(sn_p, prec);
  mpf_init2(snx_p, prec);
  mpf_init2(an_p, prec);

  mpf_t pi;
  mpf_init2(pi, prec);

  // initialize the value of sn to s1 (sqrt(2) - 1)
  //----------------------------------
  mpf_sqrt_ui(sn, 2);
  mpf_sub_ui(sn, sn, 1);

  // initialize the value of an to a0 (1/3)
  //----------------------------------
  mpf_ui_div_ui(an, 1, 3);

  //----------------------------------

  for (int i = 1; i <= iter; ++i)
  {
    mpf_snx(i);

    mpf_an(i, prec);

    if (i != iter)
      mpf_sn(i + 1, prec);
  }

  printf("\n\n[INFO] main:\n");

  printf_debug("an: %.100Ff\n", an);

  mpf_ui_div(pi, 1, an);

  FILE *out = open_file_or_panic(out_path_txt, "ab+");
  gmp_fprintf(out, "%.*Ff\n", digits, pi);

  fclose(out);

  PIHEX_END

  mpf_clear(pi);
  mpf_clears(sn, snx, an, (mpf_ptr)NULL);
  mpf_clears(sn_p, snx_p, an_p, (mpf_ptr)NULL);
  return EXIT_SUCCESS; // return 0
}

//--------------------------------------------------------------------------------------------

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

void mpf_sn(int n, mp_bitcnt_t prec)
{
  printf_debug("\n[INFO] S%i\n", n);

  assert(n <= iter);

  if (n < 1)
  {
    fprintf(stderr, "[ERROR] could not proced (n <= 1)");
    exit(EXIT_FAILURE);
  }

  char sn_name[10];
  sprintf(sn_name, "S%i", n);

  progress_bar(sn_name, 0);

  mpf_swap(sn, sn_p); // store the previous sn in the right varial and so frees the

  progress_bar(sn_name, 4);

  // consts --------------

  mpf_t u;
  mpf_init2(u, prec);
  progress_bar(sn_name, 8);
  mpf_mul(u, snx, snx);
  progress_bar(sn_name, 12);
  mpf_add_ui(u, u, 1);
  progress_bar(sn_name, 16);
  mpf_mul(u, u, snx);
  progress_bar(sn_name, 20);
  mpf_mul_ui(u, u, 8);
  progress_bar(sn_name, 24);
  mpf_qtrt(u, u);
  progress_bar(sn_name, 28);

  mpf_t t;
  progress_bar(sn_name, 32);
  mpf_init2(t, prec);
  progress_bar(sn_name, 36);
  mpf_add_ui(t, snx, 1);
  progress_bar(sn_name, 40);

  //--------------

  mpf_ui_sub(sn, 1, snx);
  progress_bar(sn_name, 44);
  mpf_pow_ui(sn, sn, 4);

  progress_bar(sn_name, 48);

  mpf_t mul, tmp;
  progress_bar(sn_name, 52);
  mpf_init2(mul, prec);
  progress_bar(sn_name, 56);
  mpf_init2(tmp, prec);

  progress_bar(sn_name, 60);
  mpf_add(mul, t, u);
  progress_bar(sn_name, 64);
  mpf_mul(mul, mul, mul);
  progress_bar(sn_name, 68);
  mpf_div(sn, sn, mul);

  progress_bar(sn_name, 72);

  mpf_mul(tmp, u, u);
  progress_bar(sn_name, 76);
  mpf_mul(mul, t, t);
  progress_bar(sn_name, 80);
  mpf_add(mul, mul, tmp);

  progress_bar(sn_name, 84);

  mpf_div(sn, sn, mul);

  progress_bar(sn_name, 88);
  mpf_clears(mul, tmp, (mpf_ptr)0);

  progress_bar(sn_name, 92);
  //--------------
  mpf_clear(u);
  mpf_clear(t);

  progress_bar(sn_name, 96);
  printf_debug("s%i:  %.60Ff\n", n, sn);

  progress_bar(sn_name, 100);
  printf("\n");

  printf_debug("[INFO] End S%i\n", n);
}

void mpf_snx(int n)
{
  printf_debug("\n[INFO] S%iX\n", n);

  assert(n <= iter);
  assert(n >= 0);

  char snx_name[10];
  sprintf(snx_name, "S%iX", n);

  progress_bar(snx_name, 0);

  mpf_swap(snx, snx_p);

  progress_bar(snx_name, 5);

  mpf_pow_ui(snx, sn, 4);
  mpf_ui_sub(snx, 1, snx);

  progress_bar(snx_name, 50);

  mpf_qtrt(snx, snx);
  printf_debug("s%ix: %.60Ff\n", n, snx);

  progress_bar(snx_name, 100);
  printf("\n");

  printf_debug("End S%iX\n", n);
}

void mpf_an(int n, mp_bitcnt_t prec)
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

  progress_bar(an_name, 0);

  mpf_swap(an, an_p);

  progress_bar(an_name, 5);

  mpf_t t, m1, m2;
  mpf_init2(t, prec);
  mpf_init2(m1, prec);
  mpf_init2(m2, prec);

  progress_bar(an_name, 10);

  mpf_add_ui(t, snx, 1);

  progress_bar(an_name, 12);

  printf_debug("s%ix: %.60Ff\n", n, snx);

  mpf_add_ui(m1, sn, 1);
  mpf_div(m1, m1, t);
  mpf_pow_ui(m1, m1, 4);

  progress_bar(an_name, 20);

  mpf_pow_ui(m2, t, 4);
  mpf_ui_div(m2, 1, m2);

  progress_bar(an_name, 25);

  printf_debug("a%i: %.60Ff\nt : %.60Ff\nm1 : %.60Ff\nm2 : %.60Ff\n", n - 1, an_p, t, m1, m2);

  mpf_t tmp;
  mpf_init2(tmp, prec);

  progress_bar(an_name, 30);

  mpf_mul_si(an, m1, -4);
  mpf_mul_si(tmp, m2, -12);
  mpf_add(an, tmp, an);
  mpf_add_ui(an, an, 1);
  printf_debug("PLUS2: %.60Ff\n", an);

  progress_bar(an_name, 50);

  mpz_t pow;
  mpz_init(pow);
  mpz_ui_pow_ui(pow, 4, (2 * n) - 1);
  mpf_set_z(tmp, pow);
  mpf_div_ui(tmp, tmp, 3);
  printf_debug("PLUS1: %Ff\n", tmp);

  mpz_clear(pow);

  progress_bar(an_name, 65);

  mpf_mul(an, an, tmp);
  mpf_neg(an, an);
  printf_debug("PLUS: %.60Ff\n", an);

  progress_bar(an_name, 80);

  mpf_mul_ui(tmp, m1, 16);
  mpf_mul(tmp, tmp, an_p);
  printf_debug("start: %.60Ff\n", tmp);

  progress_bar(an_name, 95);

  mpf_sub(an, tmp, an);
  // printf_debug("an: %.60Ff\n", an_c[n]);

  mpf_clear(tmp);
  mpf_clears(t, m1, m2, (mpf_ptr)0);
  printf_debug("a%i: %.60Ff\n", n, an);

  progress_bar(an_name, 100);
  printf("\n");

  printf_debug("End A%i\n", n);
}

void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2)
{
  mpf_t tmp;
  mpf_init(tmp);
  mpf_set_si(tmp, op2);
  printf_debug("op1 = %.60Ff\n op2 = %.60Ff\n", op1, tmp);
  mpf_mul(rop, tmp, op1);
  mpf_clear(tmp);
}

void mpf_mul_d(mpf_t rop, mpf_t op1, double op2)
{
  mpf_t tmp;
  mpf_init(tmp);
  mpf_set_d(tmp, op2);
  mpf_mul(rop, op1, tmp);
  mpf_clear(tmp);
}
