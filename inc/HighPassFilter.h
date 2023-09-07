#pragma once
#include "Filter.h"

class HighPassFilter: public Filter
{
private:
    void calculate_coefficients() override;
public:
    HighPassFilter(/* args */);
    ~HighPassFilter();
};

