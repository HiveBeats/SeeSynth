#pragma once
#include "OscillatorType.h"
#include <vector>

class Oscillator {
  private:
    OscillatorType m_osc;
    float m_freq;
    float m_volume;
    float m_phase;
    float m_phase_dt;
    float (Oscillator::*m_osc_function)(void);
    float (Oscillator::*m_dt_function)(float freq);
    void sine_osc_phase_incr();
    void saw_osc_phase_incr();
    float calc_saw_phase_delta(float freq);
    float calc_sine_phase_delta(float freq);
    float sawosc();
    float triangleosc();
    float squareosc();
    float sign(float v);
    float sineosc();

  public:
    Oscillator(OscillatorType osc, float freq, float volume);
    ~Oscillator();
    OscillatorType GetType() { return m_osc; }
    void SetType(OscillatorType osc);
    float GetVolume() { return m_volume; }
    void SetVolume(float volume) { m_volume = volume; }
    float GetFreq() { return m_freq; }
    void SetFreq(float freq);
    void Reset();
    float Process();
};
