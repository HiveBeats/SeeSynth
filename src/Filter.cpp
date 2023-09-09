#include "Filter.h"
#include "Settings.h"

Filter::Filter(/* args */) {}

Filter::~Filter() {}

void Filter::CalculateNormals() {
    m_v = powf(10, fabs(m_order) / 20.0);
    m_k = tanf(M_PI * m_freq);
}

void Filter::Trigger() {}

void Filter::Release() {}

float Filter::Process(float in) {
    // may move to a compile-time dictionary calculation, if needed
    CalculateCoefficients();
    float out = in * m_a0 + m_z1;
    m_z1 = in * m_a1 + m_z2 - m_b1 * out;
    m_z2 = in * m_a2 - m_b2 * out;
    return out;
}

void Filter::Process(std::vector<float>& samples) {
    for (std::size_t i = 0; i < samples.size(); i++) {
        samples[i] = Process(samples[i]);
    }
}

void Filter::SetParameters(float freq, float res, float q) {
    m_freq = freq / SAMPLE_RATE;
    m_q = res;
    m_order = q;
}
