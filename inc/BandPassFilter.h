#pragma once

#include "Filter.h"

class BandPassFilter : public Filter {
  protected:
    float GetSampleForFilterType() override;

  public:
    BandPassFilter();
    BandPassFilter(Filter* filter);
    BandPassFilter(float freq, float res, float q);
    ~BandPassFilter();
    bool IsSameFilterType(FilterType type) override { return type == BandPass; };
};


