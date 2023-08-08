#pragma once

#include "Adder.h"
#include "Effect.h"
#include "Note.h"
#include "Oscillator.h"
#include "Settings.h"
#include <vector>

class Synth {
  private:
    std::vector<Oscillator*> m_oscillators;
    Adder m_adder;
    std::vector<Effect*> m_effects;
    // OscillatorUI* ui_oscillators;
    // Note m_current_note;
    std::vector<float> m_out_signal;
    std::vector<float>& get_note(int semitone, float beats);
    void apply_effects();

  public:
    Synth(/* args */);
    ~Synth();
    void ProduceNoteSound(Note input);
    void AddOscillator();
    void AddEffect(Effect* fx);
    const std::vector<float>& GetOutSignal() { return m_out_signal; }
    const std::vector<Oscillator*>& GetOscillators() { return m_oscillators; }
};