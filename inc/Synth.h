#pragma once

#include "Adder.h"
#include "Effect.h"
#include "Note.h"
#include "Oscillator.h"
#include "Settings.h"
#include <vector>
#include "ADSR.h"

class Synth {
  private:
    bool is_note_triggered;
    std::vector<Oscillator*> m_oscillators;
    std::vector<Effect*> m_effects;
    // OscillatorUI* ui_oscillators;
    // Note m_current_note;
    std::vector<float> m_out_signal;
    void zero_signal();
    void get_note();
    void trigger_note_on_effects();
    void untrigger_note_on_effects();
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
    ADSR* GetADSR() { return (ADSR*)m_effects[0]; }
};