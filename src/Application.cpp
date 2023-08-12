#include "Application.h"
#include "Logger.h"
#include "Settings.h"
#include <string>

Application::Application(/* args */) {
    m_ring_buffer = new RingBuffer<float>((std::size_t)STREAM_BUFFER_SIZE);
    m_temp_buffer = new float[STREAM_BUFFER_SIZE];
    init_synth();
    init_audio();
}

Application::~Application() {
    StopAudioStream(m_synth_stream);
    UnloadAudioStream(m_synth_stream);
    CloseAudioDevice();
    CloseWindow();
    delete m_ring_buffer;
    delete[] m_temp_buffer;
    // todo: move to gui state class destructor (make it a class)
    for (int i = 0; i < m_synth_gui_state.oscillators.size(); i++) {
        delete m_synth_gui_state.oscillators[i];
    }
}

void Application::init_audio() {
    m_sound_played_count = 0;

    InitAudioDevice();
    SetMasterVolume(SYNTH_VOLUME);
    SetAudioStreamBufferSizeDefault(STREAM_BUFFER_SIZE);
    m_synth_stream = LoadAudioStream(SAMPLE_RATE, sizeof(float) * 8, 1);
    SetAudioStreamVolume(m_synth_stream, 0.5f);

    PlayAudioStream(m_synth_stream);
}

void Application::init_synth() {
    // todo: move that variables to Synth declaration
    std::string* nameString = new std::string(std::string(new char[3]));
    m_current_note = new Note{.length = 1, .name = (*nameString)};
    m_current_note->name.assign("G4");
    // todo: move somewhere in initialization
    std::vector<Oscillator*> oscillators = m_synth.GetOscillators();
    m_synth_gui_state.oscillators.reserve(oscillators.size());
    for (size_t i = 0; i < oscillators.size(); i++) {
        Oscillator* osc = oscillators[i];
        assert(osc);

        OscillatorGuiState* ui =
            new OscillatorGuiState{.freq = osc->GetFreq(),
                                   .waveshape = osc->GetType(),
                                   .volume = osc->GetVolume()};
        m_synth_gui_state.oscillators.push_back(ui);
    }
}

bool Application::detect_note_pressed(Note* note) {
    std::size_t is_pressed = 0;
    note->length = 8;
    if (IsKeyDown(KEY_A)) {
        note->name.assign("A4");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_B)) {
        note->name.assign("B4");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_C)) {
        note->name.assign("C5");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_D)) {
        note->name.assign("D5");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_E)) {
        note->name.assign("E5");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_F)) {
        note->name.assign("F5");
        is_pressed = 1;
    }
    if (IsKeyDown(KEY_G)) {
        note->name.assign("G5");
        is_pressed = 1;
    }
    return is_pressed == 1;
}

// Update On Input
void Application::update_on_note_input() {
    if (detect_note_pressed(m_current_note)) {
        // if (detect_note_pressed(m_current_note)) {
        if (!m_synth.GetIsNoteTriggered()){
            m_synth.TriggerNote((*m_current_note));
        }
        m_synth.ProduceSound();
        //m_sound_played_count = 0;
        write_log("Note played: %s\n", m_current_note->name.c_str());
        //}
    }
    else {
        m_synth.StopSound();
    }
    
}

// Play ring-buffered audio
void Application::play_buffered_audio() {
    //std::size_t size_to_read = m_ring_buffer->GetSize();
    size_t sample_count = STREAM_BUFFER_SIZE;//(size_t)(1.f/FPS * SAMPLE_RATE);
    if (IsAudioStreamProcessed(m_synth_stream)) {
        //write_log("Samples to play:%zu \n", size_to_read);
        // todo: try to start reading directly from ring buffer, avoiding
        // temp_buffer
        //todo: can be optimized to only move pointers, without the usage of temp buffer
        //m_synth.ProduceNoteSound((*m_current_note));
        // can try the SetAudioStreamCallback
        const float audio_frame_start_time = GetTime();
        update_on_note_input();
        UpdateAudioStream(m_synth_stream, m_synth.GetOutSignal().data(), sample_count);
        // can overwrite the ring buffer to avoid that
        const float audio_freme_duration = GetTime() - audio_frame_start_time;
        write_log("Frame time: %.3f%% \n", 100.0f / ((1.0f / audio_freme_duration) / ((float)SAMPLE_RATE/STREAM_BUFFER_SIZE)));

        
        // if (m_synth.GetOutSignal().size() == m_sound_played_count) {
        //     m_ring_buffer->Reset();
        // }
    }
    //m_ring_buffer->Read(m_temp_buffer, sample_count);
    //m_ring_buffer->Reset();
    //if (m_ring_buffer->IsFull() || m_ring_buffer->IsEmpty()) {
    //}
}

// Fill ring buffer from current sound
void Application::fill_audio_buffer() {
    if (!m_ring_buffer->IsFull() && 
        m_synth.GetOutSignal().size() != m_sound_played_count) {
        write_log("[INFO] IsFull:%d Samples:%zu Played:%d\n",
                  m_ring_buffer->IsFull(), m_synth.GetOutSignal().size(),
                  m_sound_played_count);

        // how many samples need write
        std::size_t size_to_fill = m_ring_buffer->GetCapacity() - m_synth.GetOutSignal().size();
        // if ((m_synth.GetOutSignal().size() - m_sound_played_count) >
        //     m_ring_buffer->GetCapacity()) {
            
        // } else {
        //     size_to_fill = m_synth.GetOutSignal().size() - m_sound_played_count;
        // }

        write_log("[INFO] SizeToFill:%zu\n", size_to_fill);
        // for (size_t i = 0; i < size_to_fill; i++) {
        //     m_temp_buffer[i] = m_synth.GetOutSignal()[i];
        // }

        m_ring_buffer->Write((float*)m_synth.GetOutSignal().data(), size_to_fill);
        m_sound_played_count += size_to_fill;
    }
}

void Application::Run() {
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //fill_audio_buffer();
        //update_on_note_input();
        play_buffered_audio();

        m_renderer.Draw(m_synth, m_synth_gui_state);
    }
}
