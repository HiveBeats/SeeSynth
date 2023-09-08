#pragma once

#include "Filter.h"

class LowPassFilter : public Filter {
  protected:
    void calculate_coefficients() override;

  public:
    LowPassFilter();
    LowPassFilter(Filter* filter);
    LowPassFilter(float freq, float res, float q);
    ~LowPassFilter();
};
