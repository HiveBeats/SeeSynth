#pragma once
#include "Filter.h"

class HighPassFilter : public Filter {
  private:
    void calculate_coefficients() override;

  public:
    HighPassFilter();
    HighPassFilter(Filter* filter);
    HighPassFilter(float freq, float res, float q);
    ~HighPassFilter();
};
