#include "Application.h"
#include "Logger.h"
#include "Settings.h"
#include <string>

Application::Application(/* args */) {
    InitSynth();
    InitAudio();
}

Application::~Application() {
    StopAudioStream(m_synth_stream);
    UnloadAudioStream(m_synth_stream);
    CloseAudioDevice();
    CloseWindow();
    // todo: move to gui state class destructor (make it a class)
    for (int i = 0; i < m_synth_gui_state.oscillators.size(); i++) {
        delete m_synth_gui_state.oscillators[i];
    }
}

void Application::InitAudio() {
    m_sound_played_count = 0;

    InitAudioDevice();
    SetMasterVolume(SYNTH_VOLUME);
    SetAudioStreamBufferSizeDefault(STREAM_BUFFER_SIZE);
    m_synth_stream = LoadAudioStream(SAMPLE_RATE, sizeof(float) * 8, 1);
    SetAudioStreamVolume(m_synth_stream, 0.5f);

    PlayAudioStream(m_synth_stream);
}

void Application::InitSynth() {
    std::string* nameString = new std::string(std::string(new char[3]));
    m_current_note = new Note{.length = 1, .name = (*nameString)};
    m_current_note->name.assign("G4");

    std::vector<Oscillator*> oscillators = m_synth.GetOscillators();
    m_synth_gui_state.oscillators.reserve(oscillators.size());
    for (size_t i = 0; i < oscillators.size(); i++) {
        Oscillator* osc = oscillators[i];
        assert(osc);

        OscillatorGuiState* ui =
            new OscillatorGuiState{.fine = osc->GetFine(),
                                   .waveshape = osc->GetType(),
                                   .volume = osc->GetVolume()};
        m_synth_gui_state.oscillators.push_back(ui);
    }
}

bool Application::DetectNotePressed(Note* note) {
    std::size_t is_pressed = 0;
    note->length = 8;
    if (IsKeyDown(KEY_A)) {
        note->name.assign("A2");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_B)) {
        note->name.assign("B2");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_C)) {
        note->name.assign("C2");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_D)) {
        note->name.assign("D2");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_E)) {
        note->name.assign("E2");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_F)) {
        note->name.assign("F2");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_G)) {
        note->name.assign("G2");
        is_pressed = 1;
    }
    return is_pressed == 1;
}

bool is_note_up() {
    return IsKeyUp(KEY_A) || IsKeyUp(KEY_B) || IsKeyUp(KEY_C) ||
           IsKeyUp(KEY_D) || IsKeyUp(KEY_E) || IsKeyUp(KEY_F) || IsKeyUp(KEY_G);
}

void Application::UpdateOnNoteInput() {
    if (DetectNotePressed(m_current_note)) {
        if (!m_synth.GetIsNoteTriggered()) {
            m_synth.Trigger((*m_current_note));
        }
        //write_log("Note played: %s\n", m_current_note->name.c_str());
    } else if (is_note_up() && m_synth.GetIsNoteTriggered()) {
        m_synth.Release();
    }
    // will produce 0 signal if ADSR is in off state
    m_synth.Process();
}

// Play ring-buffered audio
void Application::PlayBufferedAudio() {
    if (IsAudioStreamProcessed(m_synth_stream)) {
        UpdateOnNoteInput();
        UpdateAudioStream(m_synth_stream, m_synth.GetOutSignal().data(),
                          STREAM_BUFFER_SIZE);
    }
}

void Application::Run() {
    // Main game loop
    while (!WindowShouldClose()) {
        PlayBufferedAudio();
        m_renderer.Draw(m_synth, m_synth_gui_state);
    }
}
