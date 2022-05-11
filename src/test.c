#include <stdio.h>

#include <gmp.h>
#include <mpfr.h>

#define PREC 1024 * 1024 * 1000

int main(void)
{
  mpfr_t test;
  mpfr_init2(test, PREC);

  mpfr_set_ui(test, 1, 0);

  mpfr_div_ui(test, test, 3, 0);

  FILE *f = fopen("output/test.txt", "w");

  size_t nb_char = mpfr_out_str(f, 10, 0, test, 0);
  printf("\n\n\n%i\n%f\n", nb_char, ((double)(PREC)) / (double)((double)(nb_char)));

  mpfr_clear(test);
  fclose(f);
  return 0;
}