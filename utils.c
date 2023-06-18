#include "utils.h"
#include "stdlib.h"
#include "string.h"

// frees the original sounds
SynthSound concat_sounds(SynthSound* sounds, size_t count) {
    size_t total_count = 0;
    for (size_t i = 0; i < count; i++) {
        total_count += sounds[i].sample_count;
    }
    // array to hold the result
    float* total = malloc(total_count * sizeof(float));

    size_t current_count = 0;
    for (size_t i = 0; i < count; i++) {
        memcpy(total + current_count,
            sounds[i].samples,
            sounds[i].sample_count * sizeof(float));
        current_count += sounds[i].sample_count;

        free(sounds[i].samples);
    }

    SynthSound result = {
        .samples = total,
        .sample_count = total_count
    };

    return result;
}