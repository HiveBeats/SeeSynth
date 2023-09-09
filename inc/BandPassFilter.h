#pragma once
#include "Filter.h"

class BandPassFilter : public Filter {
  private:
    void CalculateCoefficients() override;

  public:
    BandPassFilter(Filter* filter);
    BandPassFilter(float freq, float res, float q);
    BandPassFilter(/* args */);
    ~BandPassFilter();
    bool IsSameFilterType(FilterType type) override { return type == BandPass; };
};
