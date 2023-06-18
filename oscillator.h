#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "utils.h"
#define WAVE_SHAPE_OPTIONS "Sine;Triangle;Sawtooth;Square"
typedef enum {
    Sine,
    Triangle,
    Saw,
    Square
} OscillatorType;

typedef struct Oscillator {
    OscillatorType osc;
    float freq;
    float volume;
} Oscillator;

typedef struct OscillatorArray {
    Oscillator* array;
    size_t count;
} OscillatorArray;

typedef struct OscillatorGenerationParameter {
    OscillatorArray oscillators;
    float sample;
} OscillatorGenerationParameter;

float multiosc(OscillatorGenerationParameter param);
SynthSound freq(float duration, OscillatorArray osc);



#endif