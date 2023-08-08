#pragma once
#include "Synth.h"
#include "SynthGuiState.h"
#include "raylib.h"
#include <vector>

class Renderer {
  private:
    void draw_main_panel(const Rectangle& panel_bounds);
    void draw_add_oscillator_button(Synth& synth, SynthGuiState& synth_gui,
                                 Rectangle panel_bounds);
    void draw_oscillators_panels(
        const std::vector<Oscillator*>& oscillators,
        const std::vector<OscillatorGuiState*>& gui_oscillators,
        const Rectangle& panel_bounds);
    void draw_oscillators_shape_inputs(
        const std::vector<Oscillator*>& oscillators,
        const std::vector<OscillatorGuiState*>& guiOscillators);
    void draw_ui(Synth& synth, SynthGuiState& synth_gui);
    void draw_signal(Synth& synth, SynthGuiState& synth_gui);

  public:
    Renderer(/* args */);
    ~Renderer();
    void Draw(Synth& synth, SynthGuiState& synth_gui);
};
