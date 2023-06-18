#include "oscillator.h"
#include "settings.h"
#include "math.h"
#include "stdlib.h"

static SynthSound get_init_samples(float duration) {
    size_t sample_count = (size_t)(duration * SAMPLE_RATE);
    float* samples = malloc(sizeof(float) * sample_count);

    for (double i = 0.0; i < duration * SAMPLE_RATE; i++) {
        samples[(int)i] = i;
    }

    SynthSound res = {
        .samples = samples,
        .sample_count = sample_count
    };

    return res;
}

static float pos(float hz, float x) {
    return fmodf(hz * x / SAMPLE_RATE, 1);
}

static float sineosc(float hz, float x) {
    return sinf(x * (2.f * SYNTH_PI * hz / SAMPLE_RATE));
}

static float sign(float v) {
    return (v > 0.0) ? 1.f : -1.f;
}

static float squareosc(float hz, float x) {
    return sign(sineosc(hz, x));
}

static float triangleosc(float hz, float x) {
    return 1.f - fabsf(pos(hz, x) - 0.5f) * 4.f;
}

static float sawosc(float hz, float x) {
    return pos(hz, x) * 2.f - 1.f;
}

float multiosc(OscillatorGenerationParameter param) {
    float osc_sample = 0.f;
    for (size_t i = 0; i < param.oscillators.count; i++) {
        Oscillator osc = param.oscillators.array[i];
        switch (osc.osc) {
            case Sine:
                osc_sample += sineosc(osc.freq, param.sample) * osc.volume;
                break;
            case Triangle:
                osc_sample += triangleosc(osc.freq, param.sample) * osc.volume;
                break;
            case Square:
                osc_sample += squareosc(osc.freq, param.sample) * osc.volume;
                break;
            case Saw:
                osc_sample += sawosc(osc.freq, param.sample) * osc.volume;
                break;
        }
    }

    return osc_sample;
}

SynthSound freq(float duration, OscillatorArray osc) {
    SynthSound samples = get_init_samples(duration);

    float* output = malloc(sizeof(float) * samples.sample_count);
    for (int i = 0; i < samples.sample_count; i++) {
        float sample = samples.samples[i];
        OscillatorGenerationParameter param = {
            .oscillators = osc,
            .sample = sample
        };
        output[i] = multiosc(param);
    }
    
    SynthSound res = {
        .samples = output,
        .sample_count = samples.sample_count
    };

    return res;
}

/*
static SynthSound get_attack_samples() {
    float attack_time = 0.001 * ATTACK_MS;
    size_t sample_count = (size_t)(attack_time * SAMPLE_RATE);
    float* attack = malloc(sizeof(float) * sample_count);
    float samples_to_rise = SAMPLE_RATE * attack_time;
    float rising_delta = 1.0 / samples_to_rise;
    float i = 0.0;

    for (int j = 0; j < sample_count; j++) {
        i += rising_delta;
        attack[j] = fmin(i, 1.0);
    }

    SynthSound res = {
        .samples = attack,
        .sample_count = sample_count
    };

    return res;
}
*/