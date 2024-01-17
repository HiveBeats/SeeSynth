#include "LowPassFilter.h"
#include "Settings.h"

LowPassFilter::LowPassFilter() {
    SetParameters(200, 0.1, 0.001);
}

LowPassFilter::LowPassFilter(
    Filter* filter) {
    SetParameters(filter->GetFreq(), filter->GetRes(), filter->GetPeakGain());
}

LowPassFilter::LowPassFilter(float freq, float res,
                                                       float q) {
    SetParameters(freq, res, q);
}

LowPassFilter::~LowPassFilter() {}

float LowPassFilter::GetSampleForFilterType() {
    return m_lowo;
}