#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#include <gmp.h>
#include <mpfr.h>

// GO TO DIR:
//     cd C:/Users/Elève/Desktop/document/progra/pihex
// or  cd C:/Users/paul/Desktop/documents/programation/pihex

// COMPILE:
//   gcc -o bin/main src/hex.c -I include -L lib -lmingw32 -lgmp -lmpfr

// RUN :
//   ./bin/main

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

void mpfr_list_init2(const int prec, mpfr_t list[], size_t size);
void mpfr_list_clear(mpfr_t list[], size_t size);

void mpfr_qtrt(mpfr_t rop, mpfr_t op, mpfr_rnd_t round);
void mpfr_ui_div_ui(mpfr_t rop, unsigned int op1, unsigned int op2, mpfr_rnd_t round);

void mpfr_sn(int n, mpfr_prec_t prec, mpfr_rnd_t round);
void mpfr_snx(int n, mpfr_prec_t prec, mpfr_rnd_t round);
void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round);

int compare_files(FILE *file1, FILE *file2);
void yellow(void);
void reset(void);
void progress_bar(const char *name, int progress);

//--------------------------------------------------------------------------------------------

#define DEBUG 0

#if DEBUG == 1
#define mpfr_debug(format, ...)                              \
	yellow();                                                  \
	mpfr_printf("[BEBUG] " format __VA_OPT__(, ) __VA_ARGS__); \
	reset()
#else
#define mpfr_debug(format, ...) printf("")
#endif

/*
ITER = 2 : 7
ITER = 3 : 170     > 16 * 7      = 112
ITER = 4 : 2470    < 16 * 170    = 2720
ITER = 5 : 35700   < 16 * 2470   = 39520
ITER = 6 : 514770  < 16 * 35700  = 571200
ITER = 7 : 8200000 ? 16 * 514770 = 8236320
*/

#define DIGITS 2000000

#define CONV 6

#define PREC (((DIGITS) + 20) * (CONV)) >> 1

#define ITER 4

#define PROG_BAR_LEN 25

#define PROG_BAR_DEC 5

//--------------------------------------------------------------------------------------------

mpfr_t sn, snx, an;
mpfr_t sn_p, snx_p, an_p;

int main(void)
{
	// variables to store the date and time components
	int hours, minutes, seconds, day, month, year;

	time_t now;
	time(&now);
	struct tm *local = localtime(&now);

	hours = local->tm_hour;				// get hours since midnight (0-23)
	minutes = local->tm_min;			// get minutes passed after the hour (0-59)
	seconds = local->tm_sec;			// get seconds passed after a minute (0-59)
	day = local->tm_mday;					// get day of month (1 to 31)
	month = local->tm_mon + 1;		// get month of year (0 to 11)
	year = local->tm_year + 1900; // get year since 1900

	char out_path[1024];
	sprintf(out_path, "output/pihex-out-%02d-%02d-%d-%02d-%02d-%02d-iter-%i-digits-%i.txt", day, month, year, hours, minutes, seconds, ITER, DIGITS);

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

	for (int i = 1; i < ITER; ++i)
	{
		mpfr_snx(i, PREC, 0);

		mpfr_an(i, PREC, 0);

		mpfr_sn(i + 1, PREC, 0);
	}

	printf("\n\n[INFO] main:\n");

	mpfr_debug("an: %.100Rf\n", an);

	mpfr_ui_div(pi, 1, an, 0);

	FILE *out = fopen(out_path, "ab+");

	if (out == NULL)
	{
		fprintf(stderr, "could not open output file: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	mpfr_fprintf(out, "%.*RNf\n", DIGITS - 20, pi);

	fclose(out);

	FILE *correct_pi = fopen("pi copy.txt", "r");
	FILE *our_pi = fopen(out_path, "r");
	if (correct_pi == NULL || our_pi == NULL)
	{
		fprintf(stderr, "could not open one of pi file file: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int correctness = compare_files(our_pi, correct_pi);

	printf("[INFO] %i decimals of pi are correct in the calculations\n", correctness);

	fclose(correct_pi);
	fclose(our_pi);

	mpfr_debug("PI = %.100Rf\n", pi);

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
	printf("\n[INFO] S%i\n", n);

	assert(n <= ITER);
	assert(n >= 1);

	if (n < 1)
	{
		fprintf(stderr, "[ERROR] could not proced (n <= 1)");
		exit(EXIT_FAILURE);
	}

	progress_bar("SN", 0);

	mpfr_swap(sn, sn_p); // store the previous sn in the right varial and so frees the

	progress_bar("SN", 5);

	// consts --------------

	mpfr_t u;
	mpfr_init2(u, prec);
	mpfr_sqr(u, snx, round);
	mpfr_add_ui(u, u, 1, round);
	mpfr_mul(u, u, snx, round);
	mpfr_mul_ui(u, u, 8, round);
	mpfr_qtrt(u, u, round);

	progress_bar("SN", 40);

	mpfr_t t;
	mpfr_init2(t, prec);
	mpfr_add_ui(t, snx, 1, round);

	progress_bar("SN", 65);

	//--------------

	mpfr_ui_sub(sn, 1, snx, round);
	mpfr_pow_ui(sn, sn, 4, round);

	progress_bar("SN", 75);

	mpfr_t mul, tmp;
	mpfr_inits2(prec, mul, tmp, (mpfr_ptr)0);

	mpfr_add(mul, t, u, round);
	mpfr_sqr(mul, mul, round);
	mpfr_div(sn, sn, mul, round);

	progress_bar("SN", 85);

	mpfr_sqr(tmp, u, round);
	mpfr_sqr(mul, t, round);
	mpfr_add(mul, mul, tmp, round);

	progress_bar("SN", 90);

	mpfr_div(sn, sn, mul, round);

	mpfr_clears(mul, tmp, (mpfr_ptr)0);

	//--------------
	mpfr_clear(u);
	mpfr_clear(t);

	mpfr_debug("s%i:  %.60Rf\n", n, sn);

	progress_bar("SN", 100);
	printf("\n");

	printf("[INFO] End S%i\n", n);
}

void mpfr_snx(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
	printf("\n[INFO] S%iX\n", n);

	assert(n <= ITER);
	assert(n >= -20);

	progress_bar("SNX", 0);

	mpfr_swap(snx, snx_p);

	progress_bar("SNX", 5);

	mpfr_pow_ui(snx, sn, 4, round);
	mpfr_ui_sub(snx, 1, snx, round);

	progress_bar("SNX", 50);

	mpfr_qtrt(snx, snx, round);
	mpfr_debug("s%ix: %.60Rf\n", n, snx);

	progress_bar("SNX", 100);

	printf("[INFO] End S%iX\n", n);
}

void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
	printf("\n[INFO] A%i\n", n);
	assert(n <= ITER);

	if (n <= 0)
	{
		fprintf(stderr, "[ERROR] could not proced (n <= 0)");
		exit(EXIT_FAILURE); // exit(1)
	}

	progress_bar("AN", 0);

	mpfr_swap(an, an_p);

	progress_bar("AN", 5);

	mpfr_t t, m1, m2;
	mpfr_inits2(prec, t, m1, m2, (mpfr_ptr)0);

	progress_bar("AN", 10);

	mpfr_add_ui(t, snx, 1, round);

	progress_bar("AN", 12);

	mpfr_debug("s%ix: %.60Rf\n", n, snx);

	mpfr_add_ui(m1, sn, 1, round);
	mpfr_div(m1, m1, t, round);
	mpfr_pow_ui(m1, m1, 4, round);

	progress_bar("AN", 20);

	mpfr_pow_si(m2, t, -4, round);

	progress_bar("AN", 25);

	mpfr_debug("a%i: %.60Rf\nt : %.60Rf\nm1 : %.60Rf\nm2 : %.60Rf\n", n - 1, an_p, t, m1, m2);

	mpfr_t tmp;
	mpfr_init2(tmp, prec);

	progress_bar("AN", 30);

	mpfr_mul_si(an, m1, -4, round);
	mpfr_mul_si(tmp, m2, -12, round);
	mpfr_add(an, tmp, an, round);
	mpfr_add_ui(an, an, 1, round);
	// mpfr_debug("PLUS2: %.60Rg\n", an_c[n]);

	progress_bar("AN", 50);

	mpfr_ui_pow_ui(tmp, 4, (2 * n) - 1, round);
	mpfr_div_ui(tmp, tmp, 3, round);
	// mpfr_debug("PLUS1: %.60Rg\n", tmp);

	progress_bar("AN", 65);

	mpfr_mul(an, an, tmp, round);
	mpfr_neg(an, an, round);
	// mpfr_debug("PLUS: %.60Rf\n", an_c[n]);

	progress_bar("AN", 80);

	mpfr_mul_ui(tmp, m1, 16, round);
	mpfr_mul(tmp, tmp, an_p, round);
	// mpfr_debug("start: %.60Rf\n", start);

	progress_bar("AN", 95);

	mpfr_sub(an, tmp, an, round);
	// mpfr_debug("an: %.60Rf\n", an_c[n]);

	mpfr_clear(tmp);
	mpfr_clears(t, m1, m2, (mpfr_ptr)0);
	mpfr_debug("a%i: %.60Rf\n", n, an);

	progress_bar("AN", 100);

	printf("[INFO] End A%i\n", n);
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

void yellow(void)
{
	printf("\033[0;33m");
}

void reset(void)
{
	printf("\033[0m");
}

void progress_bar(const char *name, int progress)
{
	assert(progress <= 100);

	char spaces[PROG_BAR_DEC + 5];
	unsigned char nb_spaces = PROG_BAR_DEC - strlen(name);

	printf("\r%s", name);
	for (unsigned char i = 0; i < nb_spaces; ++i)
		printf(" ");
	printf("|");
	for (unsigned char i = 0; i < progress / (100 / PROG_BAR_LEN); ++i)
		printf("#");
	for (unsigned char i = progress / (100 / PROG_BAR_LEN); i < PROG_BAR_LEN; ++i)
		printf("-");
	printf("|  %i%%", progress);
	fflush(stdout);
}