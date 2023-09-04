#include "ADSR.h"
#include "Settings.h"
#include "Logger.h"

ADSR::ADSR(/* args */) {
    m_parameters.attack_time = 1.f;
    m_parameters.decay_time = 0.4f;
    m_parameters.sustain_level = 0.6f;
    m_parameters.release_time = 0.8f;
    m_ramp = new Ramp(0, SAMPLE_RATE);
}

ADSR::ADSR(ADSRParameters param) {
    m_parameters = param;
}

ADSR::~ADSR() {
    delete m_ramp;
}

bool ADSR::is_attack_elapsed() {
    return m_state == Attack && m_ramp->IsCompleted();
}

bool ADSR::is_decay_elapsed() {
    return m_state == Decay && m_ramp->IsCompleted();
}

bool ADSR::is_release_elapsed() {
    return m_state == Release && m_ramp->IsCompleted();
}

void ADSR::recheck_state() {
    switch (m_state)
    {
    case Attack:
        if (is_attack_elapsed()) {
            m_state = Decay;
            m_ramp->RampTo(m_parameters.sustain_level, m_parameters.decay_time);
        }
        break;
    case Decay:
        if (is_decay_elapsed()) {
            m_state = Sustain;
        }
        break;
    case Release:
        if (is_release_elapsed()) {
            m_state = Off;
        }
        break;
    default:
        break;
    }
}

void ADSR::process_sample(float* sample) {
    if (m_state == Off) {
        (*sample) = 0;
    }
    else if (m_state == Attack) {
        (*sample) = (*sample) * m_ramp->Process();
    } 
    else if (m_state == Decay) {
        (*sample) = (*sample) * m_ramp->Process();
    }
    else if (m_state == Sustain) {
        (*sample) = (*sample) * m_parameters.sustain_level;
    }
    else if (m_state == Release) {
        (*sample) = (*sample) * m_ramp->Process();
    }
}

void ADSR::OnSetNote() {
    write_log("Set ADSR\n");
    if (m_state == Off) {
        m_state = Attack;
    }
    else if (m_state == Release) {
        m_state = Attack;
    };

    m_ramp->RampTo(1, m_parameters.attack_time);
}

void ADSR::OnUnsetNote() {
    write_log("Unset ADSR\n");
    m_state = Release;
    m_ramp->RampTo(0, m_parameters.release_time);
}

void ADSR::Process(std::vector<float>& samples) {
    for (std::size_t i = 0; i < samples.size(); i++) {
        recheck_state();
        process_sample(&samples[i]);
    }
}