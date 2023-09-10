#pragma once
#include "IEffect.h"
#include "Ramp.h"
#include <cstddef>

class ADSR : public IEffect {
    enum ADSRState { sOff, sAttack, sDecay, sSustain, sRelease };

  private:
    float m_attack_time;
    float m_decay_time;
    float m_sustain_level;
    float m_release_time;
    ADSRState m_state;
    Ramp* m_ramp;

    void ProcessSample(float* sample);
    bool IsAttackElapsed();
    bool IsDecayElapsed();
    bool IsReleaseElapsed();
    void RecheckState();

  public:
    ADSR(/* args */);
    ~ADSR();
    void Trigger() override;
    void Release() override;
    void Process(std::vector<float>& samples) override;
    void SetParameters(float attack, float decay, float sustain, float release);
};
