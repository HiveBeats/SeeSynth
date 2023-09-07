#include "HighPassFilter.h"

HighPassFilter::HighPassFilter(/* args */) {
}

HighPassFilter::~HighPassFilter() {
}

void HighPassFilter::calculate_coefficients() {
    Filter::Normals base = calculate_normals();
    m_norm = 1 / (1 + base.K / m_q + base.K * base.K);
    m_a0 = 1 * m_norm;
    m_a1 = -2 * m_a0;
    m_a2 = m_a0;
    m_b1 = 2 * (base.K * base.K - 1) * m_norm;
    m_b2 = (1 - base.K / m_q + base.K * base.K) * m_norm;
}