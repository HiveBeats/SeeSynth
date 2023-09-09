#pragma once

#include "ADSR.h"
#include "Filter.h"
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
    std::vector<Effect*> m_effects;
    std::vector<float> m_out_signal;
    Oscillator* m_lfo;
    void zero_signal();
    void get_note();
    void trigger_note_on_effects();
    void untrigger_note_on_effects();
    void apply_effects();
    void add_oscillator();
    void apply_filter_lfo();
  public:
    Synth(/* args */);
    ~Synth();
    void Trigger(Note input);
    void Process();
    void Release();
    void AddEffect(Effect* fx);
    const std::vector<float>& GetOutSignal() { return m_out_signal; }
    const std::vector<Oscillator*>& GetOscillators() { return m_oscillators; }
    const bool& GetIsNoteTriggered() { return is_note_triggered; }
    ADSR* GetADSR() { return (ADSR*)m_effects[0]; }
    Filter* GetFilter() { return (Filter*)m_effects[1]; }
    void SetFilter(FilterType type);
};