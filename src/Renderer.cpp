#include "Renderer.h"
#define RAYGUI_IMPLEMENTATION
#include "Logger.h"
#include "Settings.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "raygui.h"
#pragma clang diagnostic pop

Renderer::Renderer(/* args */) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SeeSynth - v0.2");
    SetTargetFPS(FPS);
}

Renderer::~Renderer() {}

void Renderer::Draw(Synth& synth, SynthGuiState& synth_gui) {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    // todo: implement renderer
    draw_ui(synth, synth_gui);
    draw_signal(synth, synth_gui);
    // DrawText("Congrats! You created your first window!", 190, 200, 20,
    // LIGHTGRAY); DrawFPS(0,0);

    EndDrawing();
}

void Renderer::draw_signal(Synth& synth, SynthGuiState& synth_gui) {
    GuiGrid((Rectangle){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, "",
            WINDOW_HEIGHT / 8, 2);
    auto signal = synth.GetOutSignal();
    Vector2* signal_points = new Vector2[signal.size()];
    const float screen_vertical_midpoint = (WINDOW_HEIGHT / 2);
    for (int point_idx = 0; point_idx < signal.size(); point_idx++) {
        signal_points[point_idx].x = (float)point_idx + OSCILLATOR_PANEL_WIDTH;
        signal_points[point_idx].y =
            screen_vertical_midpoint + (int)(signal[point_idx] * 300);
    }
    DrawLineStrip(signal_points, signal.size(), RED);
    delete[] signal_points;
}

void Renderer::draw_oscillators_shape_inputs(
    const std::vector<Oscillator*>& oscillators,
    const std::vector<OscillatorGuiState*>& gui_oscillators) {
#define WAVE_SHAPE_OPTIONS "Sine;Triangle;Sawtooth;Square"

    // DRAW OSCILLATOR SHAPE INPUTS
    for (int i = 0; i < oscillators.size(); i += 1) {
        OscillatorGuiState* ui_osc = gui_oscillators[i];
        assert(ui_osc);

        Oscillator* osc = oscillators[i];
        assert(osc);

        // Shape select
        int shape_index = (int)(ui_osc->waveshape);
        bool is_dropdown_click =
            GuiDropdownBox(ui_osc->shape_dropdown_rect, WAVE_SHAPE_OPTIONS,
                           &shape_index, ui_osc->is_dropdown_open);

        if (is_dropdown_click) {
            write_log("Dropdown clicked!\n");
            ui_osc->is_dropdown_open = !ui_osc->is_dropdown_open;
            ui_osc->waveshape = (OscillatorType)(shape_index);
            // APPLY STATE TO REAL OSC
            osc->SetType(ui_osc->waveshape);
        }
        if (ui_osc->is_dropdown_open)
            break;
    }
}

float Renderer::draw_oscillators_panels(
    const std::vector<Oscillator*>& oscillators,
    const std::vector<OscillatorGuiState*>& gui_oscillators,
    const Rectangle& panel_bounds) {
    float panel_y_offset = 0;
    for (int i = 0; i < oscillators.size(); i++) {
        OscillatorGuiState* ui_osc = gui_oscillators[i];
        assert(ui_osc);

        Oscillator* osc = oscillators[i];
        assert(osc);

        const bool has_shape_param = (ui_osc->waveshape == Square);

        // Draw Oscillator Panel
        const int osc_panel_width = panel_bounds.width - 20;
        const int osc_panel_height = has_shape_param ? 130 : 100;
        const int osc_panel_x = panel_bounds.x + 10;
        const int osc_panel_y = panel_bounds.y + 50 + panel_y_offset;
        panel_y_offset += osc_panel_height + 5;
        GuiPanel((Rectangle){(float)osc_panel_x, (float)osc_panel_y,
                             (float)osc_panel_width, (float)osc_panel_height},
                 "");

        const float slider_padding = 50.f;
        const float el_spacing = 5.f;
        Rectangle el_rect = {.x = (float)osc_panel_x + slider_padding + 30,
                             .y = (float)osc_panel_y + 10,
                             .width =
                                 (float)osc_panel_width - (slider_padding * 2),
                             .height = 25.f};

        // Volume slider
        float decibels = (20.f * log10f(osc->GetVolume()));
        char amp_slider_label[32];
        snprintf(amp_slider_label, 7, "%.1f dB", decibels);
        decibels =
            GuiSlider(el_rect, amp_slider_label, "", decibels, -60.0f, 0.0f);
        ui_osc->volume = powf(10.f, decibels * (1.f / 20.f));
        osc->SetVolume(ui_osc->volume);

        el_rect.y += el_rect.height + el_spacing;

        // Defer shape drop-down box.
        ui_osc->shape_dropdown_rect = el_rect;
        el_rect.y += el_rect.height + el_spacing;

        Rectangle delete_button_rect = el_rect;
        delete_button_rect.x = osc_panel_x + 5;
        delete_button_rect.y -= el_rect.height + el_spacing;
        delete_button_rect.width = 30;
        bool is_delete_button_pressed = GuiButton(delete_button_rect, "X");
        if (is_delete_button_pressed) {
            //     memmove(
            //             synth->ui_oscillator + ui_osc_i,
            //             synth->ui_oscillator + ui_osc_i + 1,
            //             (synth->ui_oscillator_count - ui_osc_i) *
            // sizeof(UiOscillator)
            //             );
            //     synth->ui_oscillator_count -= 1;
        }
    }

    return panel_y_offset;
}

void Renderer::draw_main_panel(const Rectangle& panel_bounds) {
    GuiPanel(panel_bounds, "");
}

void Renderer::draw_add_oscillator_button(Synth& synth,
                                          SynthGuiState& synth_gui,
                                          Rectangle panel_bounds) {
    //clang-format off
    bool click_add_oscillator =
        GuiButton((Rectangle){panel_bounds.x + 10, panel_bounds.y + 10,
                              panel_bounds.width - 20, 25.f},
                  "Add Oscillator");
    //clang-format on

    if (click_add_oscillator) {
        synth.AddOscillator();
        Oscillator* osc = synth.GetOscillators().back();

        OscillatorGuiState* ui =
            new OscillatorGuiState{.freq = osc->GetFreq(),
                                   .waveshape = osc->GetType(),
                                   .volume = osc->GetVolume()};
        synth_gui.oscillators.push_back(ui);
    }
}

void Renderer::draw_ui(Synth& synth, SynthGuiState& synth_gui) {
    Rectangle panel_bounds = {.x = 0,
                              .y = 0,
                              .width = OSCILLATOR_PANEL_WIDTH,
                              .height = WINDOW_HEIGHT};
    draw_main_panel(panel_bounds);
    draw_add_oscillator_button(synth, synth_gui, panel_bounds);
    // Draw Oscillators
    std::vector<Oscillator*> oscillators = synth.GetOscillators();
    std::vector<OscillatorGuiState*> gui_oscillators = synth_gui.oscillators;

    float panel_y_offset =
        draw_oscillators_panels(oscillators, gui_oscillators, panel_bounds);
    draw_oscillators_shape_inputs(oscillators, gui_oscillators);

    draw_adsr_panel(synth.GetADSR(), synth_gui.adsr, panel_bounds,
                    panel_y_offset);
}

void Renderer::draw_adsr_panel(ADSR* adsr, ADSRGuiState& gui_adsr,
                               const Rectangle& panel_bounds,
                               float panel_y_offset) {
    // Draw ADSR Panel
    const int osc_panel_width = panel_bounds.width - 20;
    const int osc_panel_height = 120;
    const int osc_panel_x = panel_bounds.x + 10;
    const int osc_panel_y = panel_bounds.y + 50 + panel_y_offset;
    panel_y_offset += osc_panel_height + 5;
    GuiPanel((Rectangle){(float)osc_panel_x, (float)osc_panel_y,
                         (float)osc_panel_width, (float)osc_panel_height},
             "ADSR");

    const float slider_padding = 50.f;
    const float el_spacing = 5.f;
    Rectangle el_rect = {.x = (float)osc_panel_x + slider_padding + 30,
                         .y = (float)osc_panel_y + 10,
                         .width = (float)osc_panel_width - (slider_padding * 2),
                         .height = 25.f};

    // Attack slider
    float attack = gui_adsr.attack;
    char attack_slider_label[32];
    snprintf(attack_slider_label, 7, "%.1f s", attack);
    attack = GuiSlider(el_rect, attack_slider_label, "", attack, 0.0f, 2.0f);
    gui_adsr.attack = attack;
    el_rect.y += el_rect.height + el_spacing;

    // Decay slider
    float decay = gui_adsr.decay;
    char decay_slider_label[32];
    snprintf(decay_slider_label, 7, "%.1f s", decay);
    decay = GuiSlider(el_rect, decay_slider_label, "", decay, 0.0f, 1.0f);
    gui_adsr.decay = decay;
    el_rect.y += el_rect.height + el_spacing;

    // Sustain slider
    float sustain = gui_adsr.sustain;
    char sustain_slider_label[32];
    snprintf(sustain_slider_label, 7, "%.1f u", sustain);
    sustain = GuiSlider(el_rect, sustain_slider_label, "", sustain, 0.0f, 1.0f);
    gui_adsr.sustain = sustain;
    el_rect.y += el_rect.height + el_spacing;

    // Release slider
    float release = gui_adsr.release;
    char release_slider_label[32];
    snprintf(release_slider_label, 7, "%.1f s", release);
    release = GuiSlider(el_rect, release_slider_label, "", release, 0.0f, 5.0f);
    gui_adsr.release = release;
    el_rect.y += el_rect.height + el_spacing;

    // apply values to real one
    adsr->SetParameters(gui_adsr.attack, gui_adsr.decay, gui_adsr.sustain,
                        gui_adsr.release);
}
