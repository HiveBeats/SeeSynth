#include "Application.h"
#include "Settings.h"
#include "Logger.h"
#include <string>
Application::Application(/* args */)
{
    m_ring_buffer = new RingBuffer<float>((std::size_t)STREAM_BUFFER_SIZE);
    m_temp_buffer = new float[STREAM_BUFFER_SIZE];
    init_gui();
    init_synth();
    init_audio();
}

Application::~Application()
{
    delete m_ring_buffer;
    delete m_temp_buffer;
}

void Application::init_gui() 
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SeeSynth - v0.2");
    SetTargetFPS(60);
}

void Application::init_audio() 
{
    m_sound_played_count = 0;

    InitAudioDevice();
    SetMasterVolume(SYNTH_VOLUME);
    SetAudioStreamBufferSizeDefault(STREAM_BUFFER_SIZE);
    m_synth_stream = LoadAudioStream(SAMPLE_RATE, sizeof(float) * 8, 1);
    SetAudioStreamVolume(m_synth_stream, 0.5f);

    PlayAudioStream(m_synth_stream);
}

void Application::init_synth() {
    //todo: move that variables to Synth declaration
    std::string* nameString = new std::string(std::string(new char[3])); 
    m_current_note = new Note {
        .length = 1,
        .name = (*nameString)
    };

    //todo: move somewhere in initialization
    // synth.ui_oscillators = malloc(sizeof(OscillatorUI) * synth.oscillators.count);
    // for (size_t i = 0; i < synth.oscillators.count; i++)
    // {
    //     OscillatorUI* ui = &synth.ui_oscillators[i];
    //     assert(ui);

    //     ui->freq = synth.oscillators.array[i].freq;
    //     ui->waveshape = synth.oscillators.array[i].osc;
    //     ui->volume = synth.oscillators.array[i].volume;
    // }
}

std::size_t Application::detect_note_pressed(Note* note) {
    std::size_t is_pressed = 0;
    note->length = 8;
    if (IsKeyPressed(KEY_A)) {
        note->name.assign("A4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_B)) {
        note->name.assign("B4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_C)) {
        note->name.assign("C4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_D)) {
        note->name.assign("D4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_E)) {
        note->name.assign("E4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_F)) {
        note->name.assign("F4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_G)) {
        note->name.assign("G4");
        is_pressed = 1;
    }
    return is_pressed;
}

void Application::Run()
{
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update Audio states
        //----------------------------------------------------------------------------------
        // Fill ring buffer from current sound
        std::vector<float> sound = m_synth.GetOutSignal();

        size_t size_for_buffer = 0;
        if (!m_ring_buffer->IsFull() && sound.size() != m_sound_played_count) {
            write_log("[INFO] IsFull:%d Samples:%zu Played:%d\n", 
                m_ring_buffer->IsFull(), 
                sound.size(), 
                m_sound_played_count);
            
            // how many samples need write
            std::size_t size_to_fill = 0;
            
            if ((sound.size() - m_sound_played_count) > m_ring_buffer->GetCapacity()) {
                size_to_fill = m_ring_buffer->GetCapacity();
            } else {
                size_to_fill = sound.size() - m_sound_played_count;
            }

            write_log("[INFO] SizeToFill:%zu\n", size_to_fill);
            for (size_t i = 0; i < size_to_fill; i++) {
                m_temp_buffer[i] = sound[i];
            }
            
            m_ring_buffer->Write(m_temp_buffer, size_to_fill);
            m_sound_played_count += size_to_fill;
        }

        // Play ring-buffered audio
        if (IsAudioStreamProcessed(m_synth_stream) && !m_ring_buffer->IsEmpty()) {
                std::size_t size_to_read = m_ring_buffer->GetSize();
                
                write_log("Samples to play:%zu \n", size_to_read);
                //todo: try to start reading directly from ring buffer, avoiding temp_buffer
                m_ring_buffer->Read(m_temp_buffer, size_to_read);
                // can try the SetAudioStreamCallback
                UpdateAudioStream(m_synth_stream, m_temp_buffer, size_to_read);
                // can overwrite the ring buffer to avoid that
                if (sound.size() == m_sound_played_count) {
                    m_ring_buffer->Reset();
                }
        }
        //----------------------------------------------------------------------------------
        
        // Update On Input
        //----------------------------------------------------------------------------------        
        if (detect_note_pressed(m_current_note)) {
            m_synth.ProduceNoteSound((*m_current_note));
            sound = m_synth.GetOutSignal();
            m_sound_played_count = 0;
            write_log("Note played: %s\n", m_current_note->name);
        }
        //----------------------------------------------------------------------------------
 
        m_renderer.Draw();
    }
}
