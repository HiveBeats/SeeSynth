#include "Synth.h"
#include "Settings.h"
#include "KeyBoard.h"
#include "OscillatorType.h"

Synth::Synth(/* args */)
{
    m_oscillators.push_back(new Oscillator(OscillatorType::Sine, 440.f, VOLUME));
}

Synth::~Synth()
{
}

std::vector<float> & Synth::get_note(int semitone, float beats) 
{
    float hz = KeyBoard::GetHzBySemitone(semitone);
    float duration = beats * BEAT_DURATION;
    
    // will change after oscillator starts to be more autonomous
    for (Oscillator* osc : m_oscillators) 
    {
        osc->SetFreq(hz);
    }

    return m_adder.SumOscillators(m_oscillators, duration); //todo: add other pipeline steps (e.g ADSR, Filters, FX);
}

void Synth::ProduceNoteSound(Note input) {
    float length = 1.f / input.length;
    int semitone_shift = KeyBoard::GetSemitoneShift(input.name);
    m_out_signal = get_note(semitone_shift, length);
}
