# Pihex
A repo for my experiments on calculating pi with an hexadecimalic method.\
The formula used can be found on the fantastic site [pi314.net](http://www.pi314.net/eng/algo.php) at the very bottom of the page.

# Quick start

## Set up

everything is done with `make`:

```bash
make
```

compiles the project and runs it with default settings.\
It will, by default also compute, agregate and display statistics on the program runtime, split in a per function basis.

## Options

you will be asked for a precision (in digits) and to confirm you want to start a computation with specific parameters.\
The program will then compute pi. Bar graphs should keep you informed of the progress.

# Compiling for debug mode

## debuging with a debugger (preffered option)

compile with:
```bash
make db
```

will create an executable file `hex-gmp_db` into `bin`.

## native debugging

If a debugger is not availible is not best suited for the current task, one can build this project in a 'native debugging' mode, where as well being compiled with debugging symbols, the executable contains `printf` which can inform you on the values/states of some variable.\
This compilation path can be reach with:

```bash
make native_db
```