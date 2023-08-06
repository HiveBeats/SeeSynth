#include "Synth.h"
#include "Settings.h"
#include "KeyBoard.h"
#include "OscillatorType.h"
#include "Note.h"

std::vector<float> & Synth::sum_oscillators(float duration)
{
    size_t sample_count = (size_t)(duration * SAMPLE_RATE);

    std::vector<float> output;// = new std::vector<float>();
    output.reserve(sample_count);

    for (size_t i = 0; i < sample_count; i++) 
    {
        float sample = 0.0f;
        for (Oscillator& osc : m_oscillators)
        {
            sample += osc.GenerateSample(duration);
        }

        output.push_back(sample);
    }

    return output;
}

std::vector<float> & Synth::note(int semitone, float beats) 
{
    float hz = KeyBoard::GetHzBySemitone(semitone);
    float duration = beats * BEAT_DURATION;
    
    // will change after oscillator starts to be more autonomous
    for (size_t i = 0; i < m_oscillators.size(); i++) {
        m_oscillators[i].SetFreq(hz);
    }

    return sum_oscillators(duration);
}

std::vector<float> & Synth::GetNoteSound(Note input) {
    float length = 1.f / input.length;
    int semitone_shift = KeyBoard::GetSemitoneShift(input.name);
    return note(semitone_shift, length);
}
