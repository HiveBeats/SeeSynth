#include "Oscillator.h"
#include "Settings.h"

#define TWO_PI 2*SYNTH_PI

Oscillator::Oscillator(OscillatorType osc, float freq, float volume)
{
    SetType(osc);
    m_freq = freq;
    m_volume = volume;
}

Oscillator::~Oscillator()
{
}

void Oscillator::Reset()
{
    m_volume = 0;
    m_phase = 0;
    m_phase_dt = 0;
}

void Oscillator::SetType(OscillatorType osc)
{
    m_osc = osc;
    switch (m_osc) {
        case Sine:
            m_osc_function = &sineosc;
            m_dt_function = &calc_sine_phase_delta;
            break;
        case Triangle:
            m_osc_function = &triangleosc;
            m_dt_function = &calc_saw_phase_delta;
            break;
        case Square:
            m_osc_function = &squareosc;
            m_dt_function = &calc_sine_phase_delta;
            break;
        case Saw:
            m_osc_function = &sawosc;
            m_dt_function = &calc_saw_phase_delta;
            break;
    }
}

void Oscillator::SetFreq(float freq)
{
    m_freq = freq;
    m_phase = 0;
    m_phase_dt = m_dt_function(freq);
}

float Oscillator::GenerateSample(float duration)
{
    return m_osc_function() * m_volume;
}

void Oscillator::sine_osc_phase_incr() 
{
    m_phase += m_phase_dt;
    if (m_phase >= TWO_PI)
        m_phase -= TWO_PI;
}

void Oscillator::saw_osc_phase_incr() 
{
    m_phase += m_phase_dt;
    if (m_phase >= 1.0f)
        m_phase -= 1.0f;
}

float Oscillator::calc_saw_phase_delta(float freq) 
{
    return freq / SAMPLE_RATE;
}

float Oscillator::calc_sine_phase_delta(float freq) 
{
    return (TWO_PI * freq) / SAMPLE_RATE;
}

float Oscillator::sineosc() 
{
    float result = sinf(m_phase);
    sine_osc_phase_incr();
    return result;
}

float Oscillator::sign(float v) 
{
    return (v > 0.0) ? 1.f : -1.f;
}

float Oscillator::squareosc() 
{
    return sign(sineosc());
}

float Oscillator::triangleosc() 
{
    float result = 1.f - fabsf(m_phase - 0.5f) * 4.f;
    saw_osc_phase_incr();
    return result;
}

float Oscillator::sawosc() 
{
    float result = m_phase * 2.f - 1.f;
    saw_osc_phase_incr();
    return result;
}