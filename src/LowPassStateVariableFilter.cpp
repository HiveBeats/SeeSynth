#include "LowPassStateVariableFilter.h"

LowPassStateVariableFilter::LowPassStateVariableFilter() {
    SetParameters(200, 0.1, 0.001);
}

LowPassStateVariableFilter::LowPassStateVariableFilter(
    StateVariableFilter* filter) {
    SetParameters(filter->GetFreq(), filter->GetRes(), filter->GetPeakGain());
}

LowPassStateVariableFilter::LowPassStateVariableFilter(float freq, float res,
                                                       float q) {
    SetParameters(freq, res, q);
}

LowPassStateVariableFilter::~LowPassStateVariableFilter() {}

float LowPassStateVariableFilter::GetSampleForFilterType() {
    return m_lowo;
}