#pragma once
#include "Oscillator.h"

class LFO: public Oscillator
{
private:
    /* data */
public:
    LFO(/* args */);
    ~LFO();
    void SetFreq(float freq) { m_phase_dt = (this->*m_dt_function)(freq); }
};

