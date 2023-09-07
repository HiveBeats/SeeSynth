#include "Filter.h"

Filter::Filter(/* args */) {
}

Filter::~Filter() {
}

Filter::Normals Filter::calculate_normals() {
    Filter::Normals res;
    res.V = powf(10, fabs(m_order) / 20.0);
    res.K = tanf(M_PI * m_freq);

    return res;
}

void Filter::Trigger() {
}

void Filter::Release() {
}

float Filter::Process(float in) {
    float out = in * m_a0 + m_z1;
    m_z1 = in * m_a1 + m_z2 - m_b1 * out;
    m_z2 = in * m_a2 - m_b2 * out;
    return out;
}

void Filter::Process(std::vector<float>& samples) {
    // todo: that will not work for ADSR-controlled filter. So, let's calculate all the possible frequency values into array/dictionary
    // then just lookup for each sample
    calculate_coefficients();
    for (std::size_t i = 0; i < samples.size(); i++) {
        samples[i] = Process(samples[i]);
    }
}

void Filter::SetParameters(float freq, float res, float q) {
    m_freq = freq;
    m_q = res;
    m_order = q;
}
