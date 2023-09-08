#include "BandPassFilter.h"

BandPassFilter::BandPassFilter(/* args */) {}

BandPassFilter::BandPassFilter(Filter* filter) {
    m_freq = filter->GetFreq();
    m_q = filter->GetRes();
    m_order = filter->GetPeakGain();
}

BandPassFilter::BandPassFilter(float freq, float res, float q) {}

BandPassFilter::~BandPassFilter() {}

void BandPassFilter::calculate_coefficients() {
    calculate_normals();
    m_norm = 1 / (1 + m_k / m_q + m_k * m_k);
    m_a0 = m_k / m_q * m_norm;
    m_a1 = 0;
    m_a2 = -m_a0;
    m_b1 = 2 * (m_k * m_k - 1) * m_norm;
    m_b2 = (1 - m_k / m_q + m_k * m_k) * m_norm;
}