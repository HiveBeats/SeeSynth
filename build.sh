#!/bin/bash
CC="${CXX:-cc}"
$CC -Wall -std=c11 ./main.c ./utils.c ./ring_buffer.c ./oscillator.c ./parser.c ./export.c -lm -lraylib -o ./bin/main
