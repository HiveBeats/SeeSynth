#pragma once

#include "Oscillator.h"
#include "Settings.h"
#include <numeric>
#include <vector>

struct Adder {
    static void SumOscillators(const std::vector<Oscillator*>& oscillators,
                               std::vector<float>& signal) {
        size_t sample_count = STREAM_BUFFER_SIZE;

        for (size_t i = 0; i < sample_count; i++) {
            float sample = 0.0f;
            for (Oscillator* osc : oscillators) {
                sample += osc->GenerateSample(1.f);
            }

            signal[i] = sample;
        }
    }
};
