#pragma once
#include "Synth.h"
#include "SynthGuiState.h"
#include <vector>
#include "raylib.h"

class Renderer
{
private:
    void DrawMainPanel(const Rectangle& panel_bounds);
    void DrawOscillatorsPanels(const std::vector<Oscillator*>& oscillators, 
        const std::vector<OscillatorGuiState*>& guiOscillators,
        const Rectangle& panel_bounds);
    void DrawOscillatorsShapeInputs(const std::vector<Oscillator*>& oscillators, 
        const std::vector<OscillatorGuiState*>& guiOscillators);
    void DrawUi(Synth & synth, const SynthGuiState & synthGui);
    void DrawSignal(Synth & synth, const SynthGuiState & synthGui);
public:
    Renderer(/* args */);
    ~Renderer();
    void Draw(Synth& synth, const SynthGuiState & synthGui);
};




