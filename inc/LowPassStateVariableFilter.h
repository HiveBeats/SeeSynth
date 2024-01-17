#pragma once

#include "StateVariableFilter.h"

class LowPassStateVariableFilter : StateVariableFilter {
  protected:
    float GetSampleForFilterType() override;

  public:
    LowPassStateVariableFilter();
    LowPassStateVariableFilter(StateVariableFilter* filter);
    LowPassStateVariableFilter(float freq, float res, float q);
    ~LowPassStateVariableFilter();
    bool IsSameFilterType(FilterType type) override { return type == LowPass; };
};
