#include "LowPassFilter.h"
#include "Settings.h"

LowPassFilter::LowPassFilter() {
    // todo: defaults
    m_freq = 200.f / SAMPLE_RATE;
    m_q = 1.0f;//0.707f;
    m_order = 0;
}

LowPassFilter::LowPassFilter(float freq, float res, float q) {
    m_freq = freq / SAMPLE_RATE;
    m_q = res;
    m_order = q;
}

LowPassFilter::LowPassFilter(Filter* filter) {
    m_freq = filter->GetFreq();
    m_q = filter->GetRes();
    m_order = filter->GetPeakGain();
}

LowPassFilter::~LowPassFilter() {}

void LowPassFilter::calculate_coefficients() {
    Filter::Normals base = calculate_normals();
    m_norm = 1 / (1 + base.K / m_q + base.K * base.K);
    m_a0 = base.K * base.K * m_norm;
    m_a1 = 2 * m_a0;
    m_a2 = m_a0;
    m_b1 = 2 * (base.K * base.K - 1) * m_norm;
    m_b2 = (1 - base.K / m_q + base.K * base.K) * m_norm;
}