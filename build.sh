#!/bin/bash
CC="${CXX:-c++}"
LL="-lm -lraylib"
FLAGS="-Wall -std=c++17 -I./inc/"
$CC $FLAGS $(find ./src -type f -iregex ".*\.cpp") $LL -o ./bin/main
