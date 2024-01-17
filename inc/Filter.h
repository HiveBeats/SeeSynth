#pragma once
#include "IEffect.h"
#include "Settings.h"

enum FilterType { LowPass, BandPass, HighPass };

class Filter : public IEffect {
  protected:
    // float* m_output; // output buffer
    float m_fs = SAMPLE_RATE; // sampling frequency;
    float m_fc;    // cutoff frequency normally something like: 440.0*pow(2.0,
                   // (midi_note - 69.0)/12.0);
    float m_res;   // resonance 0 to 1;
    float m_drive; // internal distortion 0 to 0.1
    float m_freq;  
    float m_damp; 
    float m_notcho; // notch output
    float m_lowo;   // low pass output
    float m_higho;  // high pass output
    float m_bando;  // band pass output
    float m_peako;  // peaking output = low - high
    virtual float GetSampleForFilterType(){ return 0.0; };

  public:
    Filter(/* args */);
    virtual ~Filter();
    void Trigger() override final;
    void Release() override final;
    float Process(float in);
    void Process(std::vector<float>& samples) override final;
    void SetParameters(float freq, float res, float drive);
    float GetFreq() { return m_fc; }
    float GetRes() { return m_res; }
    float GetPeakGain() { return m_drive; }
    virtual bool IsSameFilterType(FilterType type) { return false; };
};
