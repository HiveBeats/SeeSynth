#pragma once

class Ramp
{
private:
    float m_level;
    float m_sample_rate;
    float m_increment;
    int m_counter;
public:
    Ramp(float starting_level, float sample_rate);
    ~Ramp();
    void RampTo(float value, float time);
    float Process();
    bool IsCompleted();
};

