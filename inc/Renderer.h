#pragma once
#include "Synth.h"
#include "SynthGuiState.h"
#include "raylib.h"
#include <vector>

class Renderer {
  private:
    void DrawMainPanel(const Rectangle& panel_bounds);
    void DrawAddOscillatorButton(Synth& synth, SynthGuiState& synthGui,
                                 Rectangle panel_bounds);
    void DrawOscillatorsPanels(
        const std::vector<Oscillator*>& oscillators,
        const std::vector<OscillatorGuiState*>& guiOscillators,
        const Rectangle& panel_bounds);
    void DrawOscillatorsShapeInputs(
        const std::vector<Oscillator*>& oscillators,
        const std::vector<OscillatorGuiState*>& guiOscillators);
    void DrawUi(Synth& synth, SynthGuiState& synthGui);
    void DrawSignal(Synth& synth, SynthGuiState& synthGui);

  public:
    Renderer(/* args */);
    ~Renderer();
    void Draw(Synth& synth, SynthGuiState& synthGui);
};
