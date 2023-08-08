#!/bin/bash
CC="${CXX:-c++}"
$CC -Wall -std=c++17 -I./inc/ ./src/SeeSynth.cpp ./src/Application.cpp ./src/Renderer.cpp ./src/Synth.cpp ./src/Oscillator.cpp -lm -lraylib -o ./bin/main
