#include "ADSR.h"
#include "Logger.h"
#include "Settings.h"

ADSR::ADSR(/* args */) {
    m_attack_time = 1.f;
    m_decay_time = 0.4f;
    m_sustain_level = 0.6f;
    m_release_time = 0.8f;
    m_ramp = new Ramp(0, SAMPLE_RATE);
}

ADSR::~ADSR() { delete m_ramp; }

bool ADSR::is_attack_elapsed() {
    return m_state == sAttack && m_ramp->IsCompleted();
}

bool ADSR::is_decay_elapsed() {
    return m_state == sDecay && m_ramp->IsCompleted();
}

bool ADSR::is_release_elapsed() {
    return m_state == sRelease && m_ramp->IsCompleted();
}

void ADSR::recheck_state() {
    switch (m_state) {
        case sAttack:
            if (is_attack_elapsed()) {
                m_state = sDecay;
                m_ramp->RampTo(m_sustain_level, m_decay_time);
            }
            break;
        case sDecay:
            if (is_decay_elapsed()) {
                m_state = sSustain;
            }
            break;
        case sRelease:
            if (is_release_elapsed()) {
                m_state = sOff;
            }
            break;
        default:
            break;
    }
}

void ADSR::process_sample(float* sample) {
    if (m_state == sOff) {
        (*sample) = 0;
    } else if (m_state == sAttack) {
        (*sample) = (*sample) * m_ramp->Process();
    } else if (m_state == sDecay) {
        (*sample) = (*sample) * m_ramp->Process();
    } else if (m_state == sSustain) {
        (*sample) = (*sample) * m_sustain_level;
    } else if (m_state == sRelease) {
        (*sample) = (*sample) * m_ramp->Process();
    }
}

void ADSR::Trigger() {
    write_log("Set ADSR\n");
    if (m_state == sOff) {
        m_state = sAttack;
    } else if (m_state == sRelease) {
        m_state = sAttack;
    };

    m_ramp->RampTo(1, m_attack_time);
}

void ADSR::Release() {
    write_log("Unset ADSR\n");
    m_state = sRelease;
    m_ramp->RampTo(0, m_release_time);
}

void ADSR::Process(std::vector<float>& samples) {
    for (std::size_t i = 0; i < samples.size(); i++) {
        recheck_state();
        process_sample(&samples[i]);
    }
}

void ADSR::SetParameters(float attack, float decay, float sustain,
                         float release) {
    m_attack_time = attack;
    m_decay_time = decay;
    m_sustain_level = sustain;
    m_release_time = release;
}