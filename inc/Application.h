#pragma once
#include "Note.h"
#include "Renderer.h"
#include "Synth.h"
#include "SynthGuiState.h"
#include "raylib.h"

class Application {
  private:
    Synth m_synth;
    SynthGuiState m_synth_gui_state;
    AudioStream m_synth_stream;
    int m_sound_played_count;
    Note* m_current_note;
    Renderer m_renderer;
    bool DetectNotePressed(Note* note);
    void InitSynth();
    void InitAudio();
    void UpdateOnNoteInput();
    void PlayBufferedAudio();

  public:
    Application(/* args */);
    ~Application();
    void Run();
};
