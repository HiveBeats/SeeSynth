#pragma once
#include "Filter.h"

class HighPassFilter : public Filter {
  protected:
    float GetSampleForFilterType() override;

  public:
    HighPassFilter();
    HighPassFilter(Filter* filter);
    HighPassFilter(float freq, float res, float q);
    ~HighPassFilter();
    bool IsSameFilterType(FilterType type) override { return type == HighPass; };
};
