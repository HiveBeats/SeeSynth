#include "Synth.h"
#include "ADSR.h"
#include "KeyBoard.h"
#include "Logger.h"
#include "OscillatorType.h"
#include "Settings.h"

Synth::Synth(/* args */) {
    AddOscillator();
    AddEffect(new ADSR());
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        float sample = 0.0f;
        m_out_signal.push_back(sample);
    }
    zero_signal();
}

Synth::~Synth() {
    m_oscillators.clear();
    m_effects.clear();
    m_out_signal.clear();
}

void Synth::zero_signal() {
    float sample = 0.0f;
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        m_out_signal[i] = sample;
    }
}

void Synth::get_note() {
    zero_signal();
    Adder::SumOscillators(m_oscillators, m_out_signal);
}

void Synth::apply_effects() {
    for (Effect* effect : m_effects) {
        effect->Process(m_out_signal);
    }
}

void Synth::trigger_note_on_effects() {
    for (Effect* effect : m_effects) {
        effect->OnSetNote();
    }
}

void Synth::untrigger_note_on_effects() {
    for (Effect* effect : m_effects) {
        effect->OnUnsetNote();
    }
}

void Synth::TriggerNote(Note input) {
    int semitone_shift = KeyBoard::GetSemitoneShift(input.name);
    float hz = KeyBoard::GetHzBySemitone(semitone_shift);

    // will change after oscillator starts to be more autonomous
    for (Oscillator* osc : m_oscillators) {
        osc->SetFreq(hz);
    }
    is_note_triggered = true;
    trigger_note_on_effects();
}

void Synth::ProduceSound() {
    get_note();
    apply_effects();
}

// todo: rename to something like untrigger note
void Synth::StopSound() {
    zero_signal();
    is_note_triggered = false;
    untrigger_note_on_effects();
}

void Synth::AddOscillator() {
    m_oscillators.push_back(
        new Oscillator(OscillatorType::Sine, 440.f, VOLUME));
}

void Synth::AddEffect(Effect* fx) { m_effects.push_back(fx); }
