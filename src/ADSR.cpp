#include "ADSR.h"
#include "Settings.h"
#include "Logger.h"

ADSR::ADSR(/* args */) {
    m_parameters.attack_time = 1.f;
    m_parameters.decay_time = 0.3f;
    m_parameters.sustain_level = 0.6f;
    m_parameters.release_time = 1.0f;
    m_counter = 0;
}

ADSR::ADSR(ADSRParameters param) {
    m_parameters = param;
    m_counter = 0;
}

ADSR::~ADSR() {}

void ADSR::set_state(std::size_t attack_samples, std::size_t decay_samples,
                     std::size_t release_samples) {
    if (m_counter < attack_samples) {
        m_state = Attack;
    } else if (m_counter >= attack_samples &&
               m_counter < attack_samples + decay_samples) {
        m_state = Decay;
    } else if (m_counter >= attack_samples + decay_samples) {
        m_state = Sustain;
    }
}

void ADSR::process_sample(float* sample, std::size_t attack_samples,
                          std::size_t decay_samples,
                          std::size_t release_samples) {

    set_state(attack_samples, decay_samples, release_samples);
    if (m_state == Attack) {
        (*sample) = (*sample) * ((float)(1.f / attack_samples) * m_counter);
    } else if (m_state == Decay) {
    }
    m_counter++;
    // todo: release state on note off (in reset function?)
}

void ADSR::RetriggerState() {
    m_counter = 0;
    m_state = Attack;
}

void ADSR::Process(std::vector<float>& samples) {
    const std::size_t attack_samples =
        (std::size_t)(m_parameters.attack_time * SAMPLE_RATE);
    const std::size_t decay_samples =
        (std::size_t)(m_parameters.decay_time * SAMPLE_RATE);
    const std::size_t release_samples =
        (std::size_t)(m_parameters.release_time * SAMPLE_RATE);
    write_log("Attack samples: %zu \n", attack_samples);
    for (std::size_t i = 0; i < samples.size(); i++) {
        process_sample(&samples[i], attack_samples, decay_samples,
                       release_samples);
    }
    write_log("Processed samples: %zu \n", m_counter);
}