#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "utils.h"

typedef enum {
    Sine,
    Triangle,
    Saw,
    Square
} OscillatorType;

typedef struct OscillatorParameter {
    OscillatorType osc;
    float freq;
} OscillatorParameter;

typedef struct OscillatorParameterList {
    OscillatorParameter* array;
    size_t count;
} OscillatorParameterList;

typedef struct OscillatorGenerationParameter {
    OscillatorParameterList oscillators;
    float sample;
} OscillatorGenerationParameter;

float multiosc(OscillatorGenerationParameter param);
SynthSound freq(float duration, OscillatorParameterList osc);



#endif