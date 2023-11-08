CC=gcc

SRC_DIR=src
SRC_GMP_FILE=hex-gmp-6.2.1.c
SRC_GMP=$(SRC_DIR)/$(SRC_GMP_FILE)

SRC_MPFR_FILE=hex-mpfr-4.1.0.c
SRC_MPFR=$(SRC_DIR)/$(SRC_MPFR_FILE)

BIN_DIR=bin
BIN_GMP_FILE=hex-gmp
BIN_GMP=$(BIN_DIR)/$(BIN_GMP_FILE)

BIN_MPFR_FILE=hex-mpfr
BIN_MPFR=$(BIN_DIR)/$(BIN_MPFR_FILE)

IDIR=include

LDIR=lib

CFLAGS=-Wall -Wextra -O3

LFLAGS=-lmpfr -lgmp

mpfr: comp_mpfr
	$(BIN_MPFR)
gmp: comp_gmp
	$(BIN_GMP)

all: mpfr gmp

comp_mpfr:
	$(CC) $(SRC_MPFR) -o $(BIN_MPFR) -I$(IDIR) -L$(LDIR) $(CFLAGS) $(LFLAGS) 
comp_gmp:
	$(CC) $(SRC_GMP) -o $(BIN_GMP) -I $(IDIR) -L $(LDIR) $(CFLAGS) -lgmp -lm

test:
	$(CC) $(SRC_DIR)/test.c -o $(BIN_DIR)/test -I $(IDIR) -L $(LDIR) -lmpfr -lgmp -lm