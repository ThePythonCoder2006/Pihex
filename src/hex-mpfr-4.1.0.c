#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>

#include <gmp.h>
#include <mpfr.h>

// GO TO DIR:
//     cd C:/Users/Elève/Desktop/document/progra/pihex/base
// or  cd C:/Users/paul/Desktop/documents/programation/pihex/base

// COMPILE:
//   gcc -o bin/hex-mpfr src/hex-mpfr-4.1.0.c -I include -L lib -lgmp -lmpfr

// RUN :
//   ./bin/hex-mpfr

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
void mpfr_snx(int n, mpfr_rnd_t round);
void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round);

int compare_files(FILE *file1, FILE *file2);
void progress_bar(const char *name, int progress);
FILE *open_file_or_panic(const char *path, const char *mode);

// colors ----------------------
void yellow(void);
void reset(void);

//--------------------------------------------------------------------------------------------

#define DEBUG 0

#if DEBUG == 1
#define mpfr_debug(format, ...)                              \
	yellow();                                                  \
	mpfr_printf("[DEBUG] " format __VA_OPT__(, ) __VA_ARGS__); \
	reset()
#else
#define mpfr_debug(format, ...)
#endif

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

time_t start;

int main(void)
{
	// getting iter and prec variables
	//----------------------------------------------------------------
	printf("Enter the number of digits you want to calculate : ");
	fflush(stdout);
	scanf("%" PRIu64, &digits);
	fflush(stdin);

	mpfr_t prec_temp, tmp;
	mpfr_init2(prec_temp, 1024);
	mpfr_log_ui(prec_temp, digits, 0);

	mpfr_init2(tmp, 1024);
	mpfr_log_ui(tmp, 16, 0);

	mpfr_div(prec_temp, prec_temp, tmp, 0);

	mpfr_clear(tmp);

	mpfr_printf("%Rf\n", prec_temp);

	iter = mpfr_get_ui(prec_temp, MPFR_RNDU) + 1;

	mpfr_set_ui(prec_temp, digits, 0);
	mpfr_mul_d(prec_temp, prec_temp, CONV, 0);
	prec = mpfr_get_ui(prec_temp, MPFR_RNDU) + 200;

	assert(iter >= 1);

	mpfr_clear(prec_temp);

	// I/O
	//----------------------------------------------------------------
	printf("do you want to start calculating pi with an accuraty of %llu digits, using %" PRIu64 " bits per number and do %i iteration of the formula [Y/N] :\n", digits, prec, iter);
	fflush(stdout);
	char ans;
	scanf("%c", &ans);
	fflush(stdin);

	if (ans != 'y' && ans != 'Y')
	{
		printf("terminating the program.");
		exit(0);
	}

	// getting time
	//----------------------------------------------------------------
	time(&start);
	struct tm *local = localtime(&start);

	// variables to store the date and time components
	int hours, minutes, seconds, day, month, year;

	hours = local->tm_hour;				// get hours since midnight (0-23)
	minutes = local->tm_min;			// get minutes passed after the hour (0-59)
	seconds = local->tm_sec;			// get seconds passed after a minute (0-59)
	day = local->tm_mday;					// get day of month (1 to 31)
	month = local->tm_mon + 1;		// get month of year (0 to 11)
	year = local->tm_year + 1900; // get year since 1900

	char out_path[1024];
	sprintf(out_path, "output/pihex-out-%02d-%02d-%d-%02d-%02d-%02d-iter-%i-digits-%" PRIu64 ".txt", day, month, year, hours, minutes, seconds, iter, digits);

	// setting up my global variables
	//----------------------------------------------------------------
	mpfr_inits2(prec, sn, snx, an, (mpfr_ptr)0);
	mpfr_inits2(prec, sn_p, snx_p, an_p, (mpfr_ptr)0);

	mpfr_t pi;
	mpfr_init2(pi, prec);

	// initialize th value of sn to s1 (sqrt(2) - 1)
	//----------------------------------
	mpfr_sqrt_ui(sn, 2, 0);
	mpfr_sub_ui(sn, sn, 1, 0);

	// initialize th value of an to a0 (1/3)
	//----------------------------------
	mpfr_ui_div_ui(an, 1, 3, 0);

	//----------------------------------

	for (int i = 1; i < iter; ++i)
	{
		mpfr_snx(i, 0);

		mpfr_an(i, prec, 0);

		if (i != iter - 1)
			mpfr_sn(i + 1, prec, 0);
	}

	printf("\n\n[INFO] main:\n");

	mpfr_debug("an: %.100Rf\n", an);

	mpfr_ui_div(pi, 1, an, 0);

	FILE *out = open_file_or_panic(out_path, "ab+");
	mpfr_fprintf(out, "%.*RNf\n", digits, pi);

	fclose(out);

	FILE *correct_pi = open_file_or_panic("pi copy.txt", "r");
	FILE *our_pi = open_file_or_panic(out_path, "r");

	int correctness = compare_files(our_pi, correct_pi);

	printf("[INFO] %i decimals of pi are correct in the calculations\n", correctness);

	time_t end;
	time(&end);

	double time_taken = (double)(end - start);
	printf("[INFO] it took %.2f seconds\n", time_taken);

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
	mpfr_debug("\n[INFO] S%i\n", n);

	assert(n <= iter);
	assert(n >= 1);

	if (n < 1)
	{
		fprintf(stderr, "[ERROR] could not proced (n <= 1)");
		exit(EXIT_FAILURE);
	}

	char sn_name[10];
	sprintf(sn_name, "S%i", n);

	progress_bar(sn_name, 0);

	mpfr_swap(sn, sn_p); // store the previous sn in the right varial and so frees the

	progress_bar(sn_name, 5);

	// consts --------------

	mpfr_t u;
	mpfr_init2(u, prec);
	mpfr_sqr(u, snx, round);
	mpfr_add_ui(u, u, 1, round);
	mpfr_mul(u, u, snx, round);
	mpfr_mul_ui(u, u, 8, round);
	mpfr_qtrt(u, u, round);

	progress_bar(sn_name, 40);

	mpfr_t t;
	mpfr_init2(t, prec);
	mpfr_add_ui(t, snx, 1, round);

	progress_bar(sn_name, 65);

	//--------------

	mpfr_ui_sub(sn, 1, snx, round);
	mpfr_pow_ui(sn, sn, 4, round);

	progress_bar(sn_name, 75);

	mpfr_t mul, tmp;
	mpfr_inits2(prec, mul, tmp, (mpfr_ptr)0);

	mpfr_add(mul, t, u, round);
	mpfr_sqr(mul, mul, round);
	mpfr_div(sn, sn, mul, round);

	progress_bar(sn_name, 85);

	mpfr_sqr(tmp, u, round);
	mpfr_sqr(mul, t, round);
	mpfr_add(mul, mul, tmp, round);

	progress_bar(sn_name, 90);

	mpfr_div(sn, sn, mul, round);

	mpfr_clears(mul, tmp, (mpfr_ptr)0);

	//--------------
	mpfr_clear(u);
	mpfr_clear(t);

	mpfr_debug("s%i:  %.60Rf\n", n, sn);

	progress_bar(sn_name, 100);
	printf("\n");

	mpfr_debug("End S%i\n", n);
}

void mpfr_snx(int n, mpfr_rnd_t round)
{
	mpfr_debug("S%iX\n", n);

	assert(n <= iter);
	assert(n >= -20);

	char snx_name[10];
	sprintf(snx_name, "S%iX", n);

	progress_bar(snx_name, 0);

	mpfr_swap(snx, snx_p);

	progress_bar(snx_name, 5);

	mpfr_pow_ui(snx, sn, 4, round);
	mpfr_ui_sub(snx, 1, snx, round);

	progress_bar(snx_name, 50);

	mpfr_qtrt(snx, snx, round);
	mpfr_debug("s%ix: %.60Rf\n", n, snx);

	progress_bar(snx_name, 100);
	printf("\n");

	mpfr_debug("End S%iX\n", n);
}

void mpfr_an(int n, mpfr_prec_t prec, mpfr_rnd_t round)
{
	mpfr_debug("A%i\n", n);
	assert(n <= iter);

	if (n <= 0)
	{
		fprintf(stderr, "[ERROR] could not proced (n <= 0)");
		exit(EXIT_FAILURE); // exit(1)
	}

	char an_name[10];
	sprintf(an_name, "A%i", n);

	progress_bar(an_name, 0);

	mpfr_swap(an, an_p);

	progress_bar(an_name, 5);

	mpfr_t t, m1, m2;
	mpfr_inits2(prec, t, m1, m2, (mpfr_ptr)0);

	progress_bar(an_name, 10);

	mpfr_add_ui(t, snx, 1, round);

	progress_bar(an_name, 12);

	mpfr_debug("s%ix: %.60Rf\n", n, snx);

	mpfr_add_ui(m1, sn, 1, round);
	mpfr_div(m1, m1, t, round);
	mpfr_pow_ui(m1, m1, 4, round);

	progress_bar(an_name, 20);

	mpfr_pow_si(m2, t, -4, round);

	progress_bar(an_name, 25);

	mpfr_debug("a%i: %.60Rf\nt : %.60Rf\nm1 : %.60Rf\nm2 : %.60Rf\n", n - 1, an_p, t, m1, m2);

	mpfr_t tmp;
	mpfr_init2(tmp, prec);

	progress_bar(an_name, 30);

	mpfr_mul_si(an, m1, -4, round);
	mpfr_mul_si(tmp, m2, -12, round);
	mpfr_add(an, tmp, an, round);
	mpfr_add_ui(an, an, 1, round);
	// mpfr_debug("PLUS2: %.60Rg\n", an_c[n]);

	progress_bar(an_name, 50);

	mpfr_ui_pow_ui(tmp, 4, (2 * n) - 1, round);
	mpfr_div_ui(tmp, tmp, 3, round);
	// mpfr_debug("PLUS1: %.60Rg\n", tmp);

	progress_bar(an_name, 65);

	mpfr_mul(an, an, tmp, round);
	mpfr_neg(an, an, round);
	// mpfr_debug("PLUS: %.60Rf\n", an_c[n]);

	progress_bar(an_name, 80);

	mpfr_mul_ui(tmp, m1, 16, round);
	mpfr_mul(tmp, tmp, an_p, round);
	// mpfr_debug("start: %.60Rf\n", start);

	progress_bar(an_name, 95);

	mpfr_sub(an, tmp, an, round);
	// mpfr_debug("an: %.60Rf\n", an_c[n]);

	mpfr_clear(tmp);
	mpfr_clears(t, m1, m2, (mpfr_ptr)0);
	mpfr_debug("a%i: %.60Rf\n", n, an);

	progress_bar(an_name, 100);
	printf("\n");

	mpfr_debug("End A%i\n", n);
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

	int ex_t_h, ex_t_m, ex_t_s;

	time_t t;
	time(&t);

	int dif = (double)(t - start);
	ex_t_s = dif % 60;
	ex_t_m = (dif - ex_t_s) / 60;
	ex_t_h = (dif - ex_t_s) / 3600;

	printf("|  %i%% %dh%dm%ds", progress, ex_t_h, ex_t_m, ex_t_s);
	fflush(stdout);
}

FILE *open_file_or_panic(const char *path, const char *mode)
{
	FILE *f = fopen(path, mode);
	if (f == NULL)
	{
		fprintf(stderr, "could not open file '%s' because : %s", path, strerror(errno));
		exit(1);
	}
	return f;
}