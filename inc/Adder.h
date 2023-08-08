#pragma once

#include "Oscillator.h"
#include "Settings.h"
#include <numeric>
#include <vector>

struct Adder {
    static std::vector<float>&
    SumOscillators(const std::vector<Oscillator*>& oscillators,
                   float duration) {
        size_t sample_count = (size_t)(duration * SAMPLE_RATE);

        std::vector<float>* output = new std::vector<float>();
        output->reserve(sample_count);

        for (size_t i = 0; i < sample_count; i++) {
            float sample = 0.0f;
            for (Oscillator* osc : oscillators) {
                sample += osc->GenerateSample(duration);
            }

            output->push_back(sample);
        }

        return (*output);
    }
};
