#pragma once

#include <vector>
#include "Oscillator.h"
#include "Note.h"

class Synth
{
private:
    std::vector<Oscillator&> m_oscillators;
    //OscillatorUI* ui_oscillators;
    Note m_current_note;
    std::vector<float> m_out_signal;
    std::vector<float> & sum_oscillators(float duration);
    std::vector<float> & note(int semitone, float beats);
    
public:
    Synth(/* args */);
    ~Synth();
    std::vector<float> & GetNoteSound(Note input)
};

Synth::Synth(/* args */)
{
    m_oscillators.push_back(new Oscillator(OscillatorType.Sine, 440.f, VOLUME));
}

Synth::~Synth()
{
}