#include "BandPassFilter.h"
#include "Settings.h"

BandPassFilter::BandPassFilter() {
    SetParameters(200, 0.1, 0.001);
}

BandPassFilter::BandPassFilter(
    Filter* filter) {
    SetParameters(filter->GetFreq(), filter->GetRes(), filter->GetPeakGain());
}

BandPassFilter::BandPassFilter(float freq, float res,
                                                       float q) {
    SetParameters(freq, res, q);
}

BandPassFilter::~BandPassFilter() {}

float BandPassFilter::GetSampleForFilterType() {
    return m_lowo;
}