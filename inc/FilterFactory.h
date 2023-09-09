#pragma once
#include "Filter.h"
#include "LowPassFilter.h"
#include "BandPassFilter.h"
#include "HighPassFilter.h"

struct FilterFactory {
    static Filter* CreateFilter(Filter* oldFilter, FilterType newType) {
        Filter* newFilter;
        switch (newType) {
        case LowPass:
            newFilter = new LowPassFilter(oldFilter);
            break;
        case BandPass:
            newFilter = new BandPassFilter(oldFilter);
            break;
        case HighPass:
            newFilter = new HighPassFilter(oldFilter);
            break;
        default:
            break;
        }
        return newFilter;
    }

    static Filter* GetDefaultFilter() {
        return new LowPassFilter();
    }
};