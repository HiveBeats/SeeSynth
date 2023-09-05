#pragma once
#include "Synth.h"
#include "SynthGuiState.h"
#include "raylib.h"
#include <vector>
#include "ADSR.h"

class Renderer {
  private:
    void draw_main_panel(const Rectangle& panel_bounds);
    void draw_add_oscillator_button(Synth& synth, SynthGuiState& synth_gui,
                                    Rectangle panel_bounds);
    float draw_oscillators_panels(
        const std::vector<Oscillator*>& oscillators,
        const std::vector<OscillatorGuiState*>& gui_oscillators,
        const Rectangle& panel_bounds);
    void draw_oscillators_shape_inputs(
        const std::vector<Oscillator*>& oscillators,
        const std::vector<OscillatorGuiState*>& guiOscillators);
    void draw_ui(Synth& synth, SynthGuiState& synth_gui);
    void draw_signal(Synth& synth, SynthGuiState& synth_gui);
    void draw_adsr_panel(ADSR* adsr, ADSRGuiState& gui_adsr, const Rectangle& panel_bounds, float panel_y_offset);
  public:
    Renderer(/* args */);
    ~Renderer();
    void Draw(Synth& synth, SynthGuiState& synth_gui);
};
