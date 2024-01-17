#include "Synth.h"
#include "FilterFactory.h"
#include "KeyBoard.h"
#include "Logger.h"
#include "OscillatorType.h"
#include "Settings.h"
#include "LowPassFilter.h"

Synth::Synth(/* args */) {
    m_lfo = new LFO();
    m_lfo->SetFreq(5.0);
    AddOscillator();
    AddOscillator();
    AddEffect(new ADSR());
    AddEffect(FilterFactory::GetDefaultFilter());
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        float sample = 0.0f;
        m_out_signal.push_back(sample);
    }
    ZeroSignal();
}

Synth::~Synth() {
    m_oscillators.clear();
    m_effects.clear();
    m_out_signal.clear();
}

void Synth::ZeroSignal() {
    float sample = 0.0f;
    for (size_t i = 0; i < STREAM_BUFFER_SIZE; i++) {
        m_out_signal[i] = sample;
    }
}

void Synth::GetNote() {
    ZeroSignal();
    Adder::SumOscillators(m_oscillators, m_out_signal);
}

void Synth::ApplyEffects() {
    auto* adsr = m_effects[0];
    adsr->Process(m_out_signal);

    Filter* filter = (Filter*)m_effects[1];

    for (std::size_t i = 0; i < m_out_signal.size(); i++) {
        ApplyFilterLfo();
        m_out_signal[i] = filter->Process(m_out_signal[i]);
    }
}

void Synth::TriggerNoteOnEffects() {
    for (IEffect* effect : m_effects) {
        effect->Trigger();
    }
}

void Synth::UntriggerNoteOnEffects() {
    for (IEffect* effect : m_effects) {
        effect->Release();
    }
}

void Synth::AddOscillator() {
    m_oscillators.push_back(new Oscillator(OscillatorType::Sine, 0.0f, VOLUME));
}

void Synth::Trigger(Note input) {
    int semitone_shift = KeyBoard::GetSemitoneShift(input.name);

    for (Oscillator* osc : m_oscillators) {
        osc->SetKey(semitone_shift);
    }
    is_note_triggered = true;
    TriggerNoteOnEffects();
}

void Synth::ApplyFilterLfo() {
    float dt = m_lfo->Process();
    Filter* filter = (Filter*)m_effects[1];
    float freq = filter->GetFreq();
    filter->SetParameters(freq + dt, filter->GetRes(), filter->GetPeakGain());
}

void Synth::Process() {
    GetNote();
    ApplyEffects();
}

void Synth::Release() {
    ZeroSignal();
    is_note_triggered = false;
    UntriggerNoteOnEffects();
}

void Synth::AddEffect(IEffect* fx) { m_effects.push_back(fx); }

void Synth::SetFilter(FilterType type) {
    Filter* old_filter = this->GetFilter();
    if (!old_filter->IsSameFilterType(type)) {
        // todo: implement other types of state variable filters;
        Filter* new_filter = FilterFactory::CreateFilter(old_filter, type);
        delete old_filter;
        m_effects[1] = new_filter;
    }
}
