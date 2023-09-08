#pragma once
#include "Effect.h"
#include "Ramp.h"
#include <cstddef>

class ADSR : public Effect {
    enum ADSRState { sOff, sAttack, sDecay, sSustain, sRelease };

  private:
    float m_attack_time;
    float m_decay_time;
    float m_sustain_level;
    float m_release_time;
    ADSRState m_state;
    Ramp* m_ramp;

    void process_sample(float* sample);
    bool is_attack_elapsed();
    bool is_decay_elapsed();
    bool is_release_elapsed();
    void recheck_state();

  public:
    ADSR(/* args */);
    ~ADSR();
    void Trigger() override;
    void Release() override;
    void Process(std::vector<float>& samples) override;
    void SetParameters(float attack, float decay, float sustain, float release);
};
