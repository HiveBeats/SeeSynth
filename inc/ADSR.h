#pragma once
#include "Effect.h"
#include <cstddef>

struct ADSRParameters {
    float attack_time;   // Attack time in seconds
    float decay_time;    // Decay time in seconds
    float sustain_level; // Sustain level (0 to 1)
    float release_time;
};

enum ADSRState { Attack, Decay, Sustain, Release };

class ADSR : public Effect {
  private:
    ADSRParameters m_parameters;
    ADSRState m_state;
    std::size_t m_counter;
    void set_state(std::size_t attack_samples, std::size_t decay_samples,
                   std::size_t release_samples);
    void process_sample(float* sample, std::size_t attack_samples,
                        std::size_t decay_samples, std::size_t release_samples);

  public:
    ADSR(/* args */);
    ADSR(ADSRParameters param);
    ~ADSR();
    void RetriggerState() override;
    void Process(std::vector<float>& samples) override;
    void Reset();
};
