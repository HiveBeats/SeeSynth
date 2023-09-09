#include "HighPassFilter.h"

HighPassFilter::HighPassFilter(/* args */) {}

HighPassFilter::HighPassFilter(Filter* filter) {
    m_freq = filter->GetFreq();
    m_q = filter->GetRes();
    m_order = filter->GetPeakGain();
}

HighPassFilter::HighPassFilter(float freq, float res, float q) {}

HighPassFilter::~HighPassFilter() {}

void HighPassFilter::CalculateCoefficients() {
    CalculateNormals();
    m_norm = 1 / (1 + m_k / m_q + m_k * m_k);
    m_a0 = 1 * m_norm;
    m_a1 = -2 * m_a0;
    m_a2 = m_a0;
    m_b1 = 2 * (m_k * m_k - 1) * m_norm;
    m_b2 = (1 - m_k / m_q + m_k * m_k) * m_norm;
}