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
//   gcc -o bin/hex-gmp src/hex.c -I include -L lib -lgmp -lmpfr

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

void mpf_list_init2(const int prec, mpf_t list[], size_t size);
void mpf_list_clear(mpf_t list[], size_t size);

void mpf_qtrt(mpf_t rop, mpf_t op);
void mpf_ui_div_ui(mpf_t rop, unsigned int op1, unsigned int op2);

void mpf_sn(int n, mp_bitcnt_t prec);
void mpf_snx(int n, mp_bitcnt_t prec);
void mpf_an(int n, mp_bitcnt_t prec);

void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2);

int compare_files(FILE *file1, FILE *file2);
void yellow(void);
void reset(void);
void progress_bar(const char *name, int progress);
FILE *open_file_or_panic(const char *path, const char *mode);

//--------------------------------------------------------------------------------------------

#define DEBUG 0

#if DEBUG == 0
#define mpf_debug(format, ...)                              \
	yellow();                                                 \
	gmp_printf("[DEBUG] " format __VA_OPT__(, ) __VA_ARGS__); \
	reset()
#else
#define mpf_debug(format, ...)
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

mpf_t sn, snx, an;
mpf_t sn_p, snx_p, an_p;

uint64_t digits, prec;
unsigned char iter;

time_t start;

int main(void)
{
	// getting iter and prec variables
	//----------------------------------------------------------------
	printf("Enter the number of digits you want to calculate : ");
	fflush(stdout);
	scanf("%i", &digits);
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
	sprintf(out_path, "output/pihex-out-%02d-%02d-%d-%02d-%02d-%02d-iter-%i-digits-%i.txt", day, month, year, hours, minutes, seconds, iter, digits);

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

	// initialize th value of sn to s1 (sqrt(2) - 1)
	//----------------------------------
	mpf_sqrt_ui(sn, 2);
	mpf_sub_ui(sn, sn, 1);

	// initialize th value of an to a0 (1/3)
	//----------------------------------
	mpf_ui_div_ui(an, 1, 3);

	//----------------------------------

	for (int i = 1; i < iter; ++i)
	{
		mpf_snx(i, prec);

		mpf_an(i, prec);

		if (i != iter - 1)
			mpf_sn(i + 1, prec);
	}

	printf("\n\n[INFO] main:\n");

	mpf_debug("an: %.100Ff\n", an);

	mpf_ui_div(pi, 1, an);

	FILE *out = open_file_or_panic(out_path, "ab+");
	gmp_fprintf(out, "%.*Ff\n", digits, pi);

	fclose(out);

	FILE *correct_pi = open_file_or_panic("pi copy.txt", "r");
	FILE *our_pi = open_file_or_panic(out_path, "r");

	int correctness = compare_files(our_pi, correct_pi);

	printf("[INFO] %i decimals of pi are correct in the calculations\n", correctness);

	time_t end;
	time(&end);

	double time_taken = (double)(end - start);
	printf("[INFO] it took %i seconds\n", time_taken);

	fclose(correct_pi);
	fclose(our_pi);

	mpf_debug("PI = %.100Ff\n", pi);

	printf("[INFO] End\n");

	mpf_t multest;
	mpf_init(multest);

	mpf_set_ui(multest, 100);

	mpf_debug("%.60Ff\n", multest);

	mpf_mul_si(multest, multest, -7);

	mpf_debug("100 x -7 = %.60Ff\n", multest);

	mpf_clear(multest);

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
	mpf_debug("\n[INFO] S%i\n", n);

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

	mpf_swap(sn, sn_p); // store the previous sn in the right varial and so frees the

	progress_bar(sn_name, 5);

	// consts --------------

	mpf_t u;
	mpf_init2(u, prec);
	mpf_mul(u, snx, snx);
	mpf_add_ui(u, u, 1);
	mpf_mul(u, u, snx);
	mpf_mul_ui(u, u, 8);
	mpf_qtrt(u, u);

	progress_bar(sn_name, 40);

	mpf_t t;
	mpf_init2(t, prec);
	mpf_add_ui(t, snx, 1);

	progress_bar(sn_name, 65);

	//--------------

	mpf_ui_sub(sn, 1, snx);
	mpf_pow_ui(sn, sn, 4);

	progress_bar(sn_name, 75);

	mpf_t mul, tmp;
	mpf_init2(mul, prec);
	mpf_init2(tmp, prec);

	mpf_add(mul, t, u);
	mpf_mul(mul, mul, mul);
	mpf_div(sn, sn, mul);

	progress_bar(sn_name, 85);

	mpf_mul(tmp, u, u);
	mpf_mul(mul, t, t);
	mpf_add(mul, mul, tmp);

	progress_bar(sn_name, 90);

	mpf_div(sn, sn, mul);

	mpf_clears(mul, tmp, (mpf_ptr)0);

	//--------------
	mpf_clear(u);
	mpf_clear(t);

	mpf_debug("s%i:  %.60Ff\n", n, sn);

	progress_bar(sn_name, 100);
	printf("\n");

	mpf_debug("[INFO] End S%i\n", n);
}

void mpf_snx(int n, mp_bitcnt_t prec)
{
	mpf_debug("\n[INFO] S%iX\n", n);

	assert(n <= iter);
	assert(n >= -20);

	char snx_name[10];
	sprintf(snx_name, "S%iX", n);

	progress_bar(snx_name, 0);

	mpf_swap(snx, snx_p);

	progress_bar(snx_name, 5);

	mpf_pow_ui(snx, sn, 4);
	mpf_ui_sub(snx, 1, snx);

	progress_bar(snx_name, 50);

	mpf_qtrt(snx, snx);
	mpf_debug("s%ix: %.60Ff\n", n, snx);

	progress_bar(snx_name, 100);
	printf("\n");

	mpf_debug("\n[INFO] End S%iX\n", n);
}

void mpf_an(int n, mp_bitcnt_t prec)
{
	mpf_debug("\n[INFO] A%i\n", n);
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

	mpf_debug("s%ix: %.60Ff\n", n, snx);

	mpf_add_ui(m1, sn, 1);
	mpf_div(m1, m1, t);
	mpf_pow_ui(m1, m1, 4);

	progress_bar(an_name, 20);

	mpf_pow_ui(m2, t, 4);
	mpf_ui_div(m2, 1, m2);

	progress_bar(an_name, 25);

	mpf_debug("a%i: %.60Ff\nt : %.60Ff\nm1 : %.60Ff\nm2 : %.60Ff\n", n - 1, an_p, t, m1, m2);

	mpf_t tmp;
	mpf_init2(tmp, prec);

	progress_bar(an_name, 30);

	mpf_mul_si(an, m1, -4);
	mpf_mul_si(tmp, m2, -12);
	mpf_add(an, tmp, an);
	mpf_add_ui(an, an, 1);
	mpf_debug("PLUS2: %.60Rg\n", an);

	progress_bar(an_name, 50);

	mpz_t pow;
	mpz_init(pow);
	mpz_ui_pow_ui(pow, 4, (2 * n) - 1);
	mpz_div_ui(pow, pow, 3);
	mpf_debug("PLUS1: %.60Rg\n", tmp);

	progress_bar(an_name, 65);

	mpf_set_z(tmp, pow);

	mpf_mul(an, an, tmp);
	mpf_neg(an, an);
	mpz_clear(pow);
	mpf_debug("PLUS: %.60Ff\n", an);

	progress_bar(an_name, 80);

	mpf_mul_ui(tmp, m1, 16);
	mpf_mul(tmp, tmp, an_p);
	mpf_debug("start: %.60Ff\n", tmp);

	progress_bar(an_name, 95);

	mpf_sub(an, tmp, an);
	// mpf_debug("an: %.60Ff\n", an_c[n]);

	mpf_clear(tmp);
	mpf_clears(t, m1, m2, (mpf_ptr)0);
	mpf_debug("a%i: %.60Ff\n", n, an);

	progress_bar(an_name, 100);
	printf("\n");

	mpf_debug("\n[INFO] End A%i\n", n);
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
	if (DEBUG == 0)
	{
		return;
	}
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

void mpf_mul_si(mpf_t rop, mpf_t op1, signed long int op2)
{
	mpf_t tmp;
	mpf_init(tmp);
	mpf_set_si(tmp, op2);
	mpf_debug("op1 = %.60Ff\n op2 = %.60Ff\n", op1, tmp);
	mpf_mul(rop, tmp, op1);
	mpf_clear(tmp);
}