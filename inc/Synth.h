#pragma once

#include "ADSR.h"
#include "Filter.h"
#include "Adder.h"
#include "IEffect.h"
#include "Note.h"
#include "Oscillator.h"
#include "Settings.h"
#include <vector>
#include "LFO.h"

class Synth {
  private:
    bool is_note_triggered;
    std::vector<Oscillator*> m_oscillators;
    std::vector<IEffect*> m_effects;
    std::vector<float> m_out_signal;
    LFO* m_lfo;
    void ZeroSignal();
    void GetNote();
    void TriggerNoteOnEffects();
    void UntriggerNoteOnEffects();
    void ApplyEffects();
    void AddOscillator();
    void ApplyFilterLfo();
  public:
    Synth(/* args */);
    ~Synth();
    void Trigger(Note input);
    void Process();
    void Release();
    void AddEffect(IEffect* fx);
    const std::vector<float>& GetOutSignal() { return m_out_signal; }
    const std::vector<Oscillator*>& GetOscillators() { return m_oscillators; }
    const bool& GetIsNoteTriggered() { return is_note_triggered; }
    ADSR* GetADSR() { return (ADSR*)m_effects[0]; }
    Filter* GetFilter() { return (Filter*)m_effects[1]; }
    void SetFilter(FilterType type);
};