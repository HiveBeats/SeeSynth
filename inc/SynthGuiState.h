#pragma once
#include "OscillatorType.h"
#include "raygui.h"
#include <vector>

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

struct SynthGuiState {
    std::vector<OscillatorGuiState*> oscillators;
    ADSRGuiState adsr;
};