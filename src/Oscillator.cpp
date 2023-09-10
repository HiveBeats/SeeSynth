#include "Oscillator.h"
#include "Settings.h"
#include "KeyBoard.h"
#include "Logger.h"

#define TWO_PI 2 * SYNTH_PI

Oscillator::Oscillator(OscillatorType osc, float fine, float volume) {
    assert(fine >= -2.f && fine <= 2.f);
    assert(volume >= 0.f && volume <= 1.f);
    SetType(osc);
    m_fine = fine;
    m_volume = volume;
}

Oscillator::~Oscillator() {}

void Oscillator::Reset() {
    m_volume = 0;
    m_phase = 0;
    m_phase_dt = 0;
}

void Oscillator::SetType(OscillatorType osc) {
    m_osc = osc;
    switch (m_osc) {
        case Sine:
            m_osc_function = &Oscillator::SineOsc;
            m_dt_function = &Oscillator::CalcSinePhaseDelta;
            break;
        case Triangle:
            m_osc_function = &Oscillator::TriangleOsc;
            m_dt_function = &Oscillator::CalcSawPhaseDelta;
            break;
        case Square:
            m_osc_function = &Oscillator::SquareOsc;
            m_dt_function = &Oscillator::CalcSinePhaseDelta;
            break;
        case Saw:
            m_osc_function = &Oscillator::SawOsc;
            m_dt_function = &Oscillator::CalcSawPhaseDelta;
            break;
    }
}

void Oscillator::SetKey(float key) {
    m_key = key;
    float freq = KeyBoard::GetHzBySemitone(m_key + m_fine);
    m_phase = 0;
    m_phase_dt = (this->*m_dt_function)(freq);
}

float Oscillator::Process() {
    return (this->*m_osc_function)() * m_volume;
}

void Oscillator::SineOscPhaseIncr() {
    m_phase += m_phase_dt;
    if (m_phase >= TWO_PI)
        m_phase -= TWO_PI;
}

void Oscillator::SawOscPhaseIncr() {
    m_phase += m_phase_dt;
    if (m_phase >= 1.0f)
        m_phase -= 1.0f;
}

float Oscillator::CalcSawPhaseDelta(float freq) {
    return freq / SAMPLE_RATE;
}

float Oscillator::CalcSinePhaseDelta(float freq) {
    return (TWO_PI * freq) / SAMPLE_RATE;
}

float Oscillator::SineOsc() {
    float result = sinf(m_phase);
    SineOscPhaseIncr();
    return result;
}

float Oscillator::Sign(float v) { return (v > 0.0) ? 1.f : -1.f; }

float Oscillator::SquareOsc() { return Sign(SineOsc()); }

float Oscillator::TriangleOsc() {
    float result = 1.f - fabsf(m_phase - 0.5f) * 4.f;
    SawOscPhaseIncr();
    return result;
}

float Oscillator::SawOsc() {
    float result = m_phase * 2.f - 1.f;
    SawOscPhaseIncr();
    return result;
}