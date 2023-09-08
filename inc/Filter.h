#pragma once
#include "Effect.h"

class Filter : public Effect {
  protected:
    float m_freq;  // cutoff frequency
    float m_q;     // filter quantity (resonance)
    float m_order; // filter order (peakGain)
    /* todo: filter adsr */
    float m_norm, m_v, m_k;
    float m_a0, m_a1, m_a2, m_b1, m_b2;
    float m_z1, m_z2;

    void calculate_normals();
    virtual void calculate_coefficients(){};

  public:
    Filter(/* args */);
    ~Filter();
    void Trigger() override;
    void Release() override;
    float Process(float in);
    void Process(std::vector<float>& samples) override;
    void SetParameters(float freq, float res, float q);
    float GetFreq() { return m_freq; }
    float GetRes() { return m_q; }
    float GetPeakGain() { return m_norm; }
};
