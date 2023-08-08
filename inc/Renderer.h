#pragma once
#include "Synth.h"
#include "SynthGuiState.h"
class Renderer
{
private:
    void DrawUi(Synth & synth, const SynthGuiState & synthGui);
    void DrawSignal(Synth & synth, const SynthGuiState & synthGui);
public:
    Renderer(/* args */);
    ~Renderer();
    void Draw(Synth& synth, const SynthGuiState & synthGui);
};




