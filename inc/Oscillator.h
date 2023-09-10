#pragma once
#include "OscillatorType.h"
#include <vector>

class Oscillator {
  private:
    OscillatorType m_osc;
    float m_fine;
    float m_key;
    float m_volume;
    float m_phase;
    float (Oscillator::*m_osc_function)(void);
    void SineOscPhaseIncr();
    void SawOscPhaseIncr();
    float CalcSawPhaseDelta(float freq);
    float CalcSinePhaseDelta(float freq);
    float SawOsc();
    float TriangleOsc();
    float SquareOsc();
    float Sign(float v);
    float SineOsc();

  protected:
    float m_phase_dt;
    float (Oscillator::*m_dt_function)(float freq);

  public:
    Oscillator(OscillatorType osc, float fine, float volume);
    ~Oscillator();
    OscillatorType GetType() { return m_osc; }
    void SetType(OscillatorType osc);
    float GetVolume() { return m_volume; }
    void SetVolume(float volume) { m_volume = volume; }
    float GetKey() { return m_key; }
    void SetKey(float key);
    float GetFine() { return m_fine; }
    void SetFine(float fine) {
        if (fine != m_fine) {
            assert(fine >= -2.f && fine <= 2.f);
            m_fine = fine;
        }
    }
    void Reset();
    float Process();
};
