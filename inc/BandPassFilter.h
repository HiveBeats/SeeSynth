#pragma once
#include "Filter.h"

class BandPassFilter : public Filter {
  private:
    void calculate_coefficients() override;

  public:
    BandPassFilter(Filter* filter);
    BandPassFilter(float freq, float res, float q);
    BandPassFilter(/* args */);
    ~BandPassFilter();
};
