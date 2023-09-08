#include "Synth.h"
#include "KeyBoard.h"
#include "Logger.h"
#include "OscillatorType.h"
#include "Settings.h"
#include "LowPassFilter.h"

Synth::Synth(/* args */) {
    m_lfo = new Oscillator(OscillatorType::Sine, 5.f, VOLUME);
    add_oscillator();
    add_oscillator();
    AddEffect(new ADSR());
    AddEffect(new LowPassFilter());
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
        effect->Trigger();
    }
}

void Synth::untrigger_note_on_effects() {
    for (Effect* effect : m_effects) {
        effect->Release();
    }
}

void Synth::add_oscillator() {
    m_oscillators.push_back(
        new Oscillator(OscillatorType::Sine, 440.f, VOLUME));
}

void Synth::Trigger(Note input) {
    int semitone_shift = KeyBoard::GetSemitoneShift(input.name);
    float hz = KeyBoard::GetHzBySemitone(semitone_shift);

    for (Oscillator* osc : m_oscillators) {
        osc->SetFreq(hz);
    }
    is_note_triggered = true;
    trigger_note_on_effects();
}

void Synth::apply_filter_lfo() {
    float dt = m_lfo->GenerateSample();
    Filter* filter = (Filter*)m_effects[1];
    float freq = filter->GetFreq();
    //todo: check formula
    //filter->SetParameters(freq + dt * 0.2f, filter->GetRes(), filter->GetPeakGain());
}

void Synth::Process() {
    //todo: on each sample. 
    //in order to do that, we need to move to per-sample processing
    apply_filter_lfo();
    get_note();
    apply_effects();
}

void Synth::Release() {
    zero_signal();
    is_note_triggered = false;
    untrigger_note_on_effects();
}

void Synth::AddEffect(Effect* fx) { m_effects.push_back(fx); }
