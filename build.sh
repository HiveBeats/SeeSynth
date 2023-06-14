#!/bin/bash
CC="${CXX:-cc}"
$CC -Wall -std=c11 ./main.c ./parser.c -lm -o ./bin/main
