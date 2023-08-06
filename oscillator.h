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
    float phase;
    float phase_dt;
} Oscillator;

typedef struct OscillatorArray {
    Oscillator* array;
    size_t count;
} OscillatorArray;

typedef struct OscillatorGenerationParameter {
    OscillatorArray oscillators;
} OscillatorGenerationParameter;

void osc_set_freq(Oscillator* osc, float freq);
void osc_reset(Oscillator* osc);
float multiosc(OscillatorGenerationParameter param);
SynthSound freq(float duration, OscillatorArray osc);



#endif