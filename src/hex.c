#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <gmp.h>
#include <mpfr.h>

// GO TO DIR:
//     cd C:/Users/Elève/Desktop/document/progra/pihex
// or  cd C:/Users/paul/Desktop/documents/programation/pihex

// COMPILE:
//   gcc -o bin/main src/hex.c -I include -L lib -lmingw32 -lgmp -lmpfr

// RUN :
//   ./bin/main

//--------------------------------------------------------------------------------------------

void mpfr_list_init2(const int prec, mpfr_t list[], size_t size);
void mpfr_list_clear(mpfr_t list[], size_t size);

void mpfr_qtrt(mpfr_t rop, mpfr_t op, mpfr_rnd_t round);
void mpfr_ui_div_ui(mpfr_t rop, unsigned int op1, unsigned int op2, mpfr_rnd_t round);

void mpfr_sn(int n, mpfr_prec_t prec, mpfr_rnd_t round);
void mpfr_snx(int n, mpfr_prec_t prec, mpfr_rnd_t round);
void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round);
int compare_files(FILE *file1, FILE *file2);

//--------------------------------------------------------------------------------------------

/*
ITER = 2 : 7
ITER = 3 : 170     > 16 * 7      = 112
ITER = 4 : 2470    < 16 * 170    = 2720
ITER = 5 : 35700   < 16 * 2470   = 39520
ITER = 6 : 514770  < 16 * 35700  = 571200
ITER = 7 : 8200000 ? 16 * 514770 = 8236320
*/

#define DIGITS 1000

#define CONV 6

#define PREC (((DIGITS) + 20) * (CONV)) >> 1

#define ITER 4

//--------------------------------------------------------------------------------------------

// mpfr_t buffer;

mpfr_t sn, snx, an;
mpfr_t sn_p, snx_p, an_p;

int main(void)
{
	// int print_prec = 1000;
	// if (print_prec != DIGITS)
	// {
	// 	fprintf(stderr, "need to change value into mpfr_fprintf, line: 81");
	// 	exit(EXIT_FAILURE); // exit(1)
	// }

	mpfr_inits2(PREC, sn, snx, an, (mpfr_ptr)0);
	mpfr_inits2(PREC, sn_p, snx_p, an_p, (mpfr_ptr)0);

	mpfr_t pi;
	mpfr_init2(pi, PREC);

	//----------------------------------

	// initialize th value of sn to s1 (sqrt(2) - 1)
	mpfr_sqrt_ui(sn, 2, 0);
	mpfr_sub_ui(sn, sn, 1, 0);

	//----------------------------------

	// initialize th value of an to a0 (1/3)
	mpfr_ui_div_ui(an, 1, 3, 0);

	//----------------------------------

	// printf("ok");

	for (int i = 1; i < ITER; ++i)
	{
		mpfr_snx(i, PREC, 0);

		mpfr_an(i, PREC, 0);

		mpfr_sn(i + 1, PREC, 0);
	}

	printf("\n\n[INFO] main:\n");

	mpfr_printf("an: %.500Rf\n", an);

	mpfr_ui_div(pi, 1, an, 0);

	// mpfr_printf("\nsn: %.60Rf\nsnx: %.60Rf", sn, snx);

	FILE *out = fopen("pi-out.txt", "w");

	if (out == NULL)
	{
		fprintf(stderr, "could not open output file: %i", errno);
		exit(EXIT_FAILURE);
	}

	mpfr_fprintf(out, "%.*RNf\n", DIGITS - 20, pi);

	fclose(out);

	FILE *correct_pi = fopen("pi copy.txt", "r");
	FILE *our_pi = fopen("pi-out.txt", "r");
	if (correct_pi == NULL || our_pi == NULL)
	{
		exit(EXIT_FAILURE);
	}

	int correctness = compare_files(out, correct_pi);

	printf("%i\n", correctness);

	fclose(correct_pi);
	fclose(our_pi);

	mpfr_printf("PI = %.500Rf\n", pi);

	// mpfr_printf("test: sn: %.1100Rf \nsnx: %.1100Rf \nan: %.1100Rf\n", sn, snx, an, sn_p, snx_p, an_p);

	printf("[INFO] End\n");

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
	printf("\n[INFO] SN\n");

	assert(n <= ITER);
	assert(n >= 1);

	printf("n: %i\n", n);

	if (n < 1)
	{
		fprintf(stderr, "[ERROR] could not proced (n <= 1)");
		exit(EXIT_FAILURE);
	}

	mpfr_swap(sn, sn_p); // store the previous sn in the right varial and so frees the

	// consts --------------

	mpfr_t u;
	mpfr_init2(u, prec);
	mpfr_sqr(u, snx, round);
	mpfr_add_ui(u, u, 1, round);
	mpfr_mul(u, u, snx, round);
	mpfr_mul_ui(u, u, 8, round);
	mpfr_qtrt(u, u, round);

	mpfr_t t;
	mpfr_init2(t, prec);
	mpfr_add_ui(t, snx, 1, round);

	// mpfr_printf("snx_p: %.60Rf\nu: %.60Rf\n", snx_p, u);

	//--------------

	mpfr_ui_sub(sn, 1, snx, round);
	mpfr_pow_ui(sn, sn, 4, round);

	mpfr_t mul, tmp;
	mpfr_inits2(prec, mul, tmp, (mpfr_ptr)0);

	mpfr_add(mul, t, u, round);
	mpfr_sqr(mul, mul, round);
	mpfr_div(sn, sn, mul, round);

	mpfr_sqr(tmp, u, round);
	mpfr_sqr(mul, t, round);
	mpfr_add(mul, mul, tmp, round);

	mpfr_div(sn, sn, mul, round);

	mpfr_clears(mul, tmp, (mpfr_ptr)0);

	//--------------
	mpfr_clear(u);
	mpfr_clear(t);

	mpfr_printf("s%i:  %.60Rf\n", n, sn);

	printf("[INFO] End SN\n");
}

void mpfr_snx(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
	printf("\n[INFO] SNX\n");

	assert(n <= ITER);
	assert(n >= -20);

	mpfr_swap(snx, snx_p);

	printf("n: %i\n", n);
	// mpfr_sn(snx, n, prec, round);
	mpfr_pow_ui(snx, sn, 4, round);
	mpfr_ui_sub(snx, 1, snx, round);
	mpfr_qtrt(snx, snx, round);
	mpfr_printf("s%ix: %.60Rf\n", n, snx);
	printf("[INFO] End SNX\n");
}

void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
	printf("\n[INFO] AN\n");
	assert(n <= ITER);
	printf("n: %i\n", n);

	if (n <= 0)
	{
		fprintf(stderr, "[ERROR] could not proced (n <= 0)");
		exit(EXIT_FAILURE); // exit(1)
	}

	mpfr_swap(an, an_p);

	mpfr_t t, m1, m2;
	mpfr_inits2(prec, t, m1, m2, (mpfr_ptr)0);

	mpfr_add_ui(t, snx, 1, round);

	mpfr_printf("s%ix: %.60Rf\n", n, snx);

	mpfr_add_ui(m1, sn, 1, round);
	mpfr_div(m1, m1, t, round);
	mpfr_pow_ui(m1, m1, 4, round);

	mpfr_pow_si(m2, t, -4, round);

	mpfr_printf("a%i: %.60Rf\nt : %.60Rf\nm1 : %.60Rf\nm2 : %.60Rf\n", n - 1, an_p, t, m1, m2);

	mpfr_t tmp;
	mpfr_init2(tmp, prec);

	mpfr_mul_si(an, m1, -4, round);
	mpfr_mul_si(tmp, m2, -12, round);
	mpfr_add(an, tmp, an, round);
	mpfr_add_ui(an, an, 1, round);
	////mpfr_printf("PLUS2: %.60Rg\n", an_c[n]);

	mpfr_ui_pow_ui(tmp, 4, (2 * n) - 1, round);
	mpfr_div_ui(tmp, tmp, 3, round);
	// mpfr_printf("PLUS1: %.60Rg\n", tmp);

	mpfr_mul(an, an, tmp, round);
	mpfr_neg(an, an, round);
	// mpfr_printf("PLUS: %.60Rf\n", an_c[n]);

	mpfr_mul_ui(tmp, m1, 16, round);
	mpfr_mul(tmp, tmp, an_p, round);
	// mpfr_printf("start: %.60Rf\n", start);

	mpfr_sub(an, tmp, an, round);
	// mpfr_printf("an: %.60Rf\n", an_c[n]);

	mpfr_clear(tmp);
	mpfr_clears(t, m1, m2, (mpfr_ptr)0);
	mpfr_printf("a%i: %.60Rf\n", n, an);
	printf("[INFO] End AN\n");
}

int compare_files(FILE *file1, FILE *file2)
{
	int counter = 0;
	while (1)
	{
		char c1 = fgetc(file1);
		char c2 = fgetc(file2);
		printf("c1: %c \nc2: %c\n", c1, c2);

		if (feof(file1) || feof(file2))
		{
			break;
		}
		if (c1 != c2)
		{
			break;
		}
		++counter;
	}
	return counter;
}