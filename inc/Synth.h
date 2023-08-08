#pragma once

#include <vector>
#include "Oscillator.h"
#include "Note.h"
#include "Adder.h"
#include "Settings.h"
 
class Synth
{
private:
    std::vector<Oscillator*> m_oscillators;
    Adder m_adder;
    //OscillatorUI* ui_oscillators;
    //Note m_current_note;
    std::vector<float> m_out_signal;
    std::vector<float> & get_note(int semitone, float beats);
    
public:
    Synth(/* args */);
    ~Synth();
    void ProduceNoteSound(Note input);
    const std::vector<float> & GetOutSignal() { return m_out_signal; }
    const std::vector<Oscillator*>& GetOscillators() { return m_oscillators; }
};