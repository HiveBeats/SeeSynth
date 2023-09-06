#include "Ramp.h"
#include "Logger.h"

Ramp::Ramp(float starting_level, float sample_rate) {
    m_level = starting_level;
    m_sample_rate = sample_rate;
}

Ramp::~Ramp() {}

void Ramp::RampTo(float val, float time) {
    m_increment = (val - m_level) / (m_sample_rate * time);
    m_counter = (int)(m_sample_rate * time);
    write_log("Ramping from: %.1f to: %.1f by: %lf for: %d\n", m_level, val,
              m_increment, m_counter);
}

float Ramp::Process() {
    if (m_counter > 0) {
        m_counter--;
        m_level += m_increment;
    }

    return m_level;
}

bool Ramp::IsCompleted() { return m_counter == 0; }