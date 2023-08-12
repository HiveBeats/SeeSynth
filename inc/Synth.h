#pragma once

#include "Adder.h"
#include "Effect.h"
#include "Note.h"
#include "Oscillator.h"
#include "Settings.h"
#include <vector>

class Synth {
  private:
    bool is_note_triggered;
    std::vector<Oscillator*> m_oscillators;
    Adder m_adder;
    std::vector<Effect*> m_effects;
    // OscillatorUI* ui_oscillators;
    // Note m_current_note;
    std::vector<float> m_out_signal;
    void get_note();
    void apply_effects();

  public:
    Synth(/* args */);
    ~Synth();
    void TriggerNote(Note input);
    void ProduceSound();
    void StopSound();
    void AddOscillator();
    void AddEffect(Effect* fx);
    const std::vector<float>& GetOutSignal() { return m_out_signal; }
    const std::vector<Oscillator*>& GetOscillators() { return m_oscillators; }
    const bool& GetIsNoteTriggered() { return is_note_triggered; }
};