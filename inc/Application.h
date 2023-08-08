#pragma once
#include "Note.h"
#include "Synth.h"
#include "raylib.h"
#include "RingBuffer.h"
#include "Renderer.h"
#include "SynthGuiState.h"

class Application
{
private:
    Synth m_synth;
    SynthGuiState m_synth_gui_state;
    RingBuffer<float>* m_ring_buffer;
    AudioStream m_synth_stream;
    int m_sound_played_count;
    float* m_temp_buffer;
    Note* m_current_note;
    Renderer m_renderer;
    std::size_t detect_note_pressed(Note* note);
    void init_synth();
    void init_audio();
    void update_on_note_input();
    void play_buffered_audio();
    void fill_audio_buffer();

public:
    Application(/* args */);
    ~Application();
    void Run();
};

