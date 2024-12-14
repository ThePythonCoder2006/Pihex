CC := gcc

SRC_DIR := ./src
SRC_FILE := hex-gmp.c
SRC := $(SRC_DIR)/$(SRC_FILE)

BIN_DIR := ./bin
OUT_FNAME := hex-gmp
BIN := $(BIN_DIR)/$(OUT_FNAME)
BIN_DB := $(BIN)_db
BIN_ALT := $(BIN)_alt

IDIR := include

DBFLAGS := -ggdb
FASTFLAGS := -O3
LFLAGS := -lgmp -I$(IDIR)
CFLAGS := -Wall -Wextra -pedantic $(LFLAGS)

TIMES_DIR := $(SRC_DIR)/times
TIMES_SRC := $(SRC_DIR)/process_times.c
TIMES_BIN := $(BIN_DIR)/process_times

.PHONY: all run times

all: run times

run: $(BIN)
db: $(BIN_DB) | output times_dir
native_db: native_db_setup db

run: | output times_dir
	./$<

times: $(TIMES_BIN)
	./$<

native_db_setup:
	$(eval "DBFLAGS += -DDEBUG")

$(BIN) $(BIN_DB): $(SRC) Makefile | $(BIN_DIR)

$(BIN):
	$(CC) $< -o $@ $(CFLAGS) $(FASTFLAGS)

$(BIN_DB):
	$(CC) $< -o $@ $(CFLAGS) $(DBFLAGS)

$(TIMES_BIN): $(TIMES_SRC) Makefile
	$(CC) $< -o $@ $(CFLAGS)

times_dir: $(TIMES_DIR)/sn $(TIMES_DIR)/snx $(TIMES_DIR)/an

$(BIN_DIR) output $(TIMES_DIR)/an $(TIMES_DIR)/snx $(TIMES_DIR)/sn:
	- @mkdir "$@"