#include "Synth.h"
#include "ADSR.h"
#include "KeyBoard.h"
#include "OscillatorType.h"
#include "Settings.h"

Synth::Synth(/* args */) {
    AddOscillator();
    AddEffect(new ADSR());
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        float sample = 0.0f;
        m_out_signal.push_back(sample);
    }
}

Synth::~Synth() {
    m_oscillators.clear();
    m_effects.clear();
    m_out_signal.clear();
}

void Synth::get_note() {
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        float sample = 0.0f;
        m_out_signal[i] = sample;
    }

    // todo: add other pipeline steps (e.g ADSR, Filters, FX);
    Adder::SumOscillators(m_oscillators, m_out_signal);
}

void Synth::apply_effects() {
    for (Effect* effect : m_effects) {
        // maybe not here
        //effect->RetriggerState();
        effect->Process(m_out_signal);
    }
}

void Synth::TriggerNote(Note input) {
    float length = 1.f / input.length;
    int semitone_shift = KeyBoard::GetSemitoneShift(input.name);
    float hz = KeyBoard::GetHzBySemitone(semitone_shift);

    // will change after oscillator starts to be more autonomous
    for (Oscillator* osc : m_oscillators) {
        osc->SetFreq(hz);
    }
    is_note_triggered = true;
}

void Synth::ProduceSound() {
    get_note();
    apply_effects();
}

void Synth::StopSound() {
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        float sample = 0.0f;
        m_out_signal[i] = sample;
    }
    is_note_triggered = false;
}

void Synth::AddOscillator() {
    m_oscillators.push_back(
        new Oscillator(OscillatorType::Sine, 440.f, VOLUME));
}

void Synth::AddEffect(Effect* fx) { m_effects.push_back(fx); }
