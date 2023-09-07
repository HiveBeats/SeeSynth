#pragma once
#include "Filter.h"

class BandPassFilter: public Filter
{
private:
    void calculate_coefficients() override;
public:
    BandPassFilter(/* args */);
    ~BandPassFilter();
};

