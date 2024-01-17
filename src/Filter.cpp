#include "Filter.h"
#include "Settings.h"
#include <algorithm>
#include <cmath>

Filter::Filter(/* args */) {}

Filter::~Filter() {}

void Filter::Trigger() {}

void Filter::Release() {}

void Filter::Process(std::vector<float>& samples) {
    for (std::size_t i = 0; i < samples.size(); i++) {
        samples[i] = Process(samples[i]);
    }
}


float Filter::Process(float in) {
    m_notcho = in - m_damp * m_bando;
    m_lowo = m_lowo + m_freq * m_bando;
    m_higho = m_notcho - m_lowo;
    m_bando =
        m_freq * m_higho + m_bando - m_drive * m_bando * m_bando * m_bando;
    // (m_notcho or m_lowo or m_higho or m_bando or m_peako)
    float out = 0.5 * GetSampleForFilterType();
    m_notcho = in - m_damp * m_bando;
    m_lowo = m_lowo + m_freq * m_bando;
    m_higho = m_notcho - m_lowo;
    m_bando =
        m_freq * m_higho + m_bando - m_drive * m_bando * m_bando * m_bando;
    out += 0.5 * GetSampleForFilterType();

    return out;
}

void Filter::SetParameters(float freq, float res, float drive) {
    m_fc = freq;
    m_res = res;
    m_drive = drive;

    // the fs*2 is because it's double sampled
    m_freq =
        2.0 * std::sinf(SYNTH_PI * std::min(0.25f, m_fc / (m_fs * 2)));

    m_damp = std::min(2.0f * (1.0f - std::powf(m_res, 0.25f)),
                            std::min(2.0f, 2.0f / m_freq - m_freq * 0.5f));
}
