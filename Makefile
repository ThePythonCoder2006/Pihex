CC=gcc

SRC_DIR=./src
SRC_GMP_FILE=hex-gmp.c
SRC_GMP=$(SRC_DIR)/$(SRC_GMP_FILE)

SRC_MPFR_FILE=hex-mpfr.c
SRC_MPFR=$(SRC_DIR)/$(SRC_MPFR_FILE)

BIN_DIR=./bin
BIN_GMP_FILE=hex-gmp
BIN_GMP=$(BIN_DIR)/$(BIN_GMP_FILE)

BIN_MPFR_FILE=hex-mpfr
BIN_MPFR=$(BIN_DIR)/$(BIN_MPFR_FILE)

CFLAGS=-Wall -Wextra -pedantic
DBFLAGS=-ggdb
FASTFLAGS=-O3

IDIR=include

LFLAGS= -lmpfr -lgmp

mpfr: fast_mpfr
gmp:  fast_gmp

fast_mpfr: comp_fast_mpfr
	$(BIN_MPFR)
fast_gmp: comp_fast_gmp
	$(BIN_GMP)

plain_mpfr: comp_mpfr
	$(BIN_MPFR)
plain_gmp: comp_gmp
	$(BIN_GMP)

all: mpfr gmp

comp_fast_mpfr:
	$(CC) $(SRC_MPFR) -o $(BIN_MPFR) $(CFLAGS) $(FASTFLAGS) -I$(IDIR) $(LFLAGS) 
comp_fast_gmp:
	$(CC) $(SRC_GMP) -o $(BIN_GMP) $(CFLAGS) $(FASTFLAGS) -I$(IDIR) -lgmp -lm

comp_plain_mpfr:
	$(CC) $(SRC_MPFR) -o $(BIN_MPFR) $(CFLAGS) -I$(IDIR) $(LFLAGS) 
comp_plain_gmp:
	$(CC) $(SRC_GMP) -o $(BIN_GMP) $(CFLAGS) -I$(IDIR) -lgmp -lm

comp_db_mpfr:
	$(CC) $(SRC_MPFR) -o $(BIN_MPFR) $(CFLAGS) $(DBFLAGS) -I$(IDIR) $(LFLAGS) -DDEBUG
comp_db_gmp:
	$(CC) $(SRC_GMP) -o $(BIN_GMP) $(CFLAGS) $(DBFLAGS) -I$(IDIR) -lgmp -lm -DDEBUG

test:
	$(CC) $(SRC_DIR)/test.c -o $(BIN_DIR)/test -I $(IDIR) -L $(LDIR) -lmpfr -lgmp -lm