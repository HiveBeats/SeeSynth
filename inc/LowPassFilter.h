#pragma once

#include "Filter.h"

class LowPassFilter : public Filter {
  protected:
    float GetSampleForFilterType() override;

  public:
    LowPassFilter();
    LowPassFilter(Filter* filter);
    LowPassFilter(float freq, float res, float q);
    ~LowPassFilter();
    bool IsSameFilterType(FilterType type) override { return type == LowPass; };
};

