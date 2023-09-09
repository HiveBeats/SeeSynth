#pragma once
#include "OscillatorType.h"
#include "raygui.h"
#include <vector>
#include "Filter.h"

struct OscillatorGuiState {
    float volume;
    float freq; // todo: remove or change to pitch shift
    OscillatorType waveshape;
    bool is_dropdown_open;
    Rectangle shape_dropdown_rect;
};

struct ADSRGuiState {
    float attack;
    float decay;
    float sustain;
    float release;
};

struct FilterGuiState {
    float freq;
    float res; //todo: res
    FilterType type;
    bool is_dropdown_open;
};

struct SynthGuiState {
    std::vector<OscillatorGuiState*> oscillators;
    ADSRGuiState adsr;
    FilterGuiState filter;
};