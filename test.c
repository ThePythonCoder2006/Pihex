#include <stdio.h>

#include <gmp.h>
#include <mpfr.h>

int main(void)
{
  printf("%i", sizeof(mpfr_t));
  return 0;
}