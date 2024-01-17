#include "HighPassFilter.h"
#include "Settings.h"

HighPassFilter::HighPassFilter() {
    SetParameters(200, 0.1, 0.001);
}

HighPassFilter::HighPassFilter(
    Filter* filter) {
    SetParameters(filter->GetFreq(), filter->GetRes(), filter->GetPeakGain());
}

HighPassFilter::HighPassFilter(float freq, float res,
                                                       float q) {
    SetParameters(freq, res, q);
}

HighPassFilter::~HighPassFilter() {}

float HighPassFilter::GetSampleForFilterType() {
    return m_higho;
}