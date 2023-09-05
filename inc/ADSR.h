#pragma once
#include "Effect.h"
#include "Ramp.h"
#include <cstddef>

struct ADSRParameters {
    float attack_time;   // Attack time in seconds
    float decay_time;    // Decay time in seconds
    float sustain_level; // Sustain level (0 to 1)
    float release_time;
};

enum ADSRState { Off, Attack, Decay, Sustain, Release };

class ADSR : public Effect {
  private:
    ADSRParameters m_parameters;
    ADSRState m_state;
    Ramp* m_ramp;

    void process_sample(float* sample);
    bool is_attack_elapsed();
    bool is_decay_elapsed();
    bool is_release_elapsed();
    void recheck_state();

  public:
    ADSR(/* args */);
    ADSR(ADSRParameters param);
    ~ADSR();
    void OnSetNote() override;
    void OnUnsetNote() override;
    // void RetriggerState() override;
    void Process(std::vector<float>& samples) override;
    void Reset();
    void SetParameters(float attack, float decay, float sustain, float release);
};
