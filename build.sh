#!/bin/bash
gcc -o bin/hex-gmp src/hex-gmp-6.2.1.c -I include -L lib -lgmp -lmpfr -Wextra -Wall
gcc -o bin/hex-mpfr src/hex-mpfr-4.1.0.c -I include -L lib -lgmp -lmpfr -Wextra -Wall