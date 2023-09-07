#pragma once

#include "Filter.h"

class LowPassFilter: public Filter
{
protected:
    void calculate_coefficients() override;
public:
    LowPassFilter(/* args */);
    ~LowPassFilter();
};

