#pragma once
#include "Filter.h"

class HighPassFilter : public Filter {
  private:
    void CalculateCoefficients() override;

  public:
    HighPassFilter();
    HighPassFilter(Filter* filter);
    HighPassFilter(float freq, float res, float q);
    ~HighPassFilter();
    bool IsSameFilterType(FilterType type) override { return type == HighPass; };
};
