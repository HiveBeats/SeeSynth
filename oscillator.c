#include "oscillator.h"
#include "settings.h"
#include "math.h"
#include "stdlib.h"

#define TWO_PI 2*SYNTH_PI

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

static void sine_osc_phase_incr(Oscillator* osc) {
    osc->phase += osc->phase_dt;
    if (osc->phase >= TWO_PI)
        osc->phase -= TWO_PI;
}

static void saw_osc_phase_incr(Oscillator* osc) {
    osc->phase += osc->phase_dt;
    if (osc->phase >= 1.0f)
        osc->phase -= 1.0f;
}

static float calc_saw_phase_delta(float freq) {
    return freq / SAMPLE_RATE;
}

static float calc_sine_phase_delta(float freq) {
    return (TWO_PI * freq) / SAMPLE_RATE;
}

static float sineosc(Oscillator* osc) {
    float result = sinf(osc->phase);
    sine_osc_phase_incr(osc);
    return result;
}

static float sign(float v) {
    return (v > 0.0) ? 1.f : -1.f;
}

static float squareosc(Oscillator* osc) {
    return sign(sineosc(osc));
}

static float triangleosc(Oscillator* osc) {
    float result = 1.f - fabsf(osc->phase - 0.5f) * 4.f;
    saw_osc_phase_incr(osc);
    return result;
}

static float sawosc(Oscillator* osc) {
    float result = osc->phase * 2.f - 1.f;
    saw_osc_phase_incr(osc);
    return result;
}

void osc_set_freq(Oscillator* osc, float freq) {
    osc->freq = freq;
    osc->phase = 0;
    switch (osc->osc)
    {
        case Sine:
            osc->phase_dt = calc_sine_phase_delta(freq);
            break;
        case Square:
            osc->phase_dt = calc_sine_phase_delta(freq);
            break;
        case Triangle:
            osc->phase_dt = calc_saw_phase_delta(freq);
            break;
        case Saw:
            osc->phase_dt = calc_saw_phase_delta(freq);
            break;
        default:
            break;
    }
}

void osc_reset(Oscillator* osc) {
    osc->volume = 0;
    osc->phase = 0;
    osc->phase_dt = 0;
}

float multiosc(OscillatorGenerationParameter param) {
    float osc_sample = 0.f;
    for (size_t i = 0; i < param.oscillators.count; i++) {
        Oscillator* osc = &param.oscillators.array[i];
        assert(osc);
        
        switch (osc->osc) {
            case Sine:
                osc_sample += sineosc(osc) * osc->volume;
                break;
            case Triangle:
                osc_sample += triangleosc(osc) * osc->volume;
                break;
            case Square:
                osc_sample += squareosc(osc) * osc->volume;
                break;
            case Saw:
                osc_sample += sawosc(osc) * osc->volume;
                break;
        }
    }

    return osc_sample;
}

SynthSound freq(float duration, OscillatorArray osc) {
    size_t sample_count = (size_t)(duration * SAMPLE_RATE);
    
    float* output = malloc(sizeof(float) * sample_count);
    for (size_t i = 0; i < sample_count; i++) {
        OscillatorGenerationParameter param = {
            .oscillators = osc
        };
        output[i] = multiosc(param);
    }
    
    SynthSound res = {
        .samples = output,
        .sample_count = sample_count
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