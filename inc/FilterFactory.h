#pragma once
#include "Filter.h"
#include "LowPassFilter.h"
#include "BandPassFilter.h"
#include "HighPassFilter.h"

struct FilterFactory {
    static Filter* CreateFilter(Filter* old_filter, FilterType new_type) {
        Filter* new_filter;
        switch (new_type) {
        case LowPass:
            new_filter = new LowPassFilter(old_filter);
            break;
        case BandPass:
            new_filter = new BandPassFilter(old_filter);
            break;
        case HighPass:
            new_filter = new HighPassFilter(old_filter);
            break;
        default:
            break;
        }
        return new_filter;
    }

    static Filter* GetDefaultFilter() {
        return new LowPassFilter();
    }
};