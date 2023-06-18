#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#include "parser.h"
#include "utils.h"
#include "ring_buffer.h"
#include "settings.h"
#include "oscillator.h"
#include "export.h"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//------------------------------------------------------------------------------------
// Synth
//------------------------------------------------------------------------------------

typedef struct OscillatorUI {
    float volume;
    float freq;//todo: remove or change to pitch shift
    OscillatorType waveshape;
    bool is_dropdown_open;
    Rectangle shape_dropdown_rect;
} OscillatorUI;

typedef struct Synth {
    OscillatorArray oscillators;
    OscillatorUI* ui_oscillators;
    Note current_note;
    SynthSound* out_signal;
} Synth;

static int get_semitone_shift_internal(char* root_note, char* target_note) {
    char* pitch_classes[12] =
        { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    // Extract the note number and pitch class for the root note
    int root_note_num = (int)root_note[strlen(root_note) - 1] - '0';

    char* root_pitch_class_str = malloc((strlen(root_note) - 1) * sizeof(char));
    strncpy(root_pitch_class_str, root_note, strlen(root_note) - 1);

    int root_pitch_class = -1;

    for (int i = 0; i < 12; i++) {
        if (strcmp(pitch_classes[i], root_pitch_class_str) == 0) {
            root_pitch_class = i;
            break;
        }
    }

    free(root_pitch_class_str);

    // Extract the note number and pitch class for the target note
    int target_note_num = (int)target_note[strlen(target_note) - 1] - '0';

    char* target_pitch_class_str =
        malloc((strlen(target_note) - 1) * sizeof(char));
    strncpy(target_pitch_class_str, target_note, strlen(target_note) - 1);

    int target_pitch_class = -1;

    for (int i = 0; i < 12; i++) {
        if (strcmp(pitch_classes[i], target_pitch_class_str) == 0) {
            target_pitch_class = i;
            break;
        }
    }

    free(target_pitch_class_str);

    // Calculate the semitone shift using the formula
    return (target_note_num - root_note_num) * 12 +
        (target_pitch_class - root_pitch_class);
}

static float get_hz_by_semitone(int semitone) {
    return PITCH_STANDARD * powf(powf(2.f, (1.f / 12.f)), semitone);
}

int get_semitone_shift(char* target_note) {
    return get_semitone_shift_internal("A4", target_note);
}

static OscillatorArray init_osc_array() {
    Oscillator first = {
        .osc = Square,
        .freq = 440.f,
        .volume = VOLUME
    };

    Oscillator* oscArray = malloc(sizeof(Oscillator*) * 1);
    oscArray[0] = first;

    OscillatorArray oscillators = {
        .array = oscArray,
        .count = 1
    };

    return oscillators;
}

SynthSound note(Synth* synth, int semitone, float beats) {
    float hz = get_hz_by_semitone(semitone);
    float duration = beats * BEAT_DURATION;
    
    // will change after oscillator starts to be more autonomous
    for (size_t i = 0; i < synth->oscillators.count; i++) {
        synth->oscillators.array[i].freq = hz;
    }

    return freq(duration, synth->oscillators);
}

SynthSound get_note_sound(Synth* synth, Note input) {
    float length = 1.f / input.length;
    int semitone_shift = get_semitone_shift(input.name);
    return note(synth, semitone_shift, length);
}
//-------------------------------------------------------

size_t detect_note_pressed(Note* note) {
    size_t is_pressed = 0;
    note->length = 8;
    if (IsKeyPressed(KEY_A)) {
        strcpy(note->name, "A4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_B)) {
        strcpy(note->name, "B4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_C)) {
        strcpy(note->name, "C4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_D)) {
        strcpy(note->name, "D4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_E)) {
        strcpy(note->name, "E4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_F)) {
        strcpy(note->name, "F4");
        is_pressed = 1;
    }
    if (IsKeyPressed(KEY_G)) {
        strcpy(note->name, "G4");
        is_pressed = 1;
    }
    return is_pressed;
}

//------------------------------------------------------------------------------------
// GUI
//------------------------------------------------------------------------------------

void DrawUi(Synth *synth) {
    const int panel_x_start = 0;
    const int panel_y_start = 0;
    const int panel_width = OSCILLATOR_PANEL_WIDTH;
    const int panel_height = WINDOW_HEIGHT;
    
    bool is_shape_dropdown_open = false;
    int shape_index = 0;
    
    
    GuiPanel((Rectangle){
                 panel_x_start,
                 panel_y_start,
                 panel_width,
                 panel_height
             }, 
            "");
    
    bool click_add_oscillator = GuiButton((Rectangle){
                                              panel_x_start + 10,
                                              panel_y_start + 10,
                                              panel_width - 20,
                                              25
                                          }, "Add Oscillator");
    if (click_add_oscillator)
    {
        // synth->ui_oscillator_count += 1;
        // // Set defaults:
        // UiOscillator *ui_osc = synth->ui_oscillator + (synth->ui_oscillator_count - 1);
        // ui_osc->shape = WaveShape_SINE;
        // ui_osc->freq = BASE_NOTE_FREQ;
        // ui_osc->amplitude_ratio = 0.1f;
        // ui_osc->shape_parameter_0 = 0.5f;
    }

    // Draw Oscillators
    float panel_y_offset = 0;
    //synth->ui_oscillator_count = 1
    for (int ui_osc_i = 0; ui_osc_i < synth->oscillators.count; ui_osc_i++)
    {
        OscillatorUI* ui_osc = &synth->ui_oscillators[ui_osc_i];
        Oscillator* osc = &synth->oscillators.array[ui_osc_i];
        const bool has_shape_param = (ui_osc->waveshape == Square);
        
        // Draw Oscillator Panel
        const int osc_panel_width = panel_width - 20;
        const int osc_panel_height = has_shape_param ? 130 : 100;
        const int osc_panel_x = panel_x_start + 10;
        const int osc_panel_y = panel_y_start + 50 + panel_y_offset;
        panel_y_offset += osc_panel_height + 5;
        GuiPanel((Rectangle){
                     osc_panel_x,
                     osc_panel_y,
                     osc_panel_width,
                     osc_panel_height
                 },
                "");
        
        const float slider_padding = 50.f;
        const float el_spacing = 5.f;
        Rectangle el_rect = {
            .x = osc_panel_x + slider_padding + 30,
            .y = osc_panel_y + 10,
            .width = osc_panel_width - (slider_padding * 2),
            .height = 25
        };
        
        // Volume slider
        float decibels = (20.f * log10f(osc->volume));
        char amp_slider_label[32];
        sprintf(amp_slider_label, "%.1f dB", decibels);
        decibels = GuiSlider(el_rect, 
                             amp_slider_label, 
                             "", 
                             decibels, 
                             -60.0f, 
                             0.0f
                             );
        ui_osc->volume = powf(10.f, decibels * (1.f/20.f));
        osc->volume = ui_osc->volume;

        el_rect.y += el_rect.height + el_spacing;
        
        // Defer shape drop-down box.
        ui_osc->shape_dropdown_rect = el_rect;
        el_rect.y += el_rect.height + el_spacing;
        /*
        Rectangle delete_button_rect = el_rect;
        delete_button_rect.x = osc_panel_x + 5;
        delete_button_rect.y -= el_rect.height + el_spacing;
        delete_button_rect.width = 30;
        bool is_delete_button_pressed = GuiButton(delete_button_rect, "X");
        if (is_delete_button_pressed)
        {
            memmove(
                    synth->ui_oscillator + ui_osc_i, 
                    synth->ui_oscillator + ui_osc_i + 1, 
                    (synth->ui_oscillator_count - ui_osc_i) * sizeof(UiOscillator)
                    );
            synth->ui_oscillator_count -= 1;
        }
        */
    }

    // DRAW OSCILLATOR SHAPE INPUTS
    for (int ui_osc_i = 0; ui_osc_i < synth->oscillators.count; ui_osc_i += 1)
    {
        OscillatorUI* ui_osc = &synth->ui_oscillators[ui_osc_i];
        Oscillator* osc = &synth->oscillators.array[ui_osc_i];
        // Shape select
        int shape_index = (int)(ui_osc->waveshape);
        bool is_dropdown_click = GuiDropdownBox(ui_osc->shape_dropdown_rect, 
                                                WAVE_SHAPE_OPTIONS, 
                                                &shape_index, 
                                                ui_osc->is_dropdown_open
                                                );
        
        if (is_dropdown_click)
        {
            ui_osc->is_dropdown_open = !ui_osc->is_dropdown_open;
            ui_osc->waveshape = (OscillatorType)(shape_index);
            // APPLY STATE TO REAL OSC
            osc->osc = (OscillatorType)(shape_index);
        }
        if (ui_osc->is_dropdown_open) break;
    }
}

void DrawSignal(Synth* synth) {
    GuiGrid((Rectangle){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}, "", WINDOW_HEIGHT / 8, 2);
    Vector2* signal_points = malloc(sizeof(Vector2) * synth->out_signal->sample_count);
    const float screen_vertical_midpoint = (WINDOW_HEIGHT/2);
    for (int point_idx = 0; point_idx < synth->out_signal->sample_count; point_idx++)
    {
        signal_points[point_idx].x = (float)point_idx + OSCILLATOR_PANEL_WIDTH;
        signal_points[point_idx].y = screen_vertical_midpoint + (int)(synth->out_signal->samples[point_idx] * 300);
    }
    DrawLineStrip(signal_points, synth->out_signal->sample_count, RED);
}


//------------------------------------------------------------------------------------
// Main
//------------------------------------------------------------------------------------

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SeeSynth - v0.2");
    SetTargetFPS(60);

    //todo: move that variables to Synth declaration
    Note g_current_note = {
        .length = 1,
        .name = malloc(sizeof(char) * 3)
    };

    SynthSound g_sound = {
        .sample_count = 0
    };

    Synth synth = {
        .current_note = g_current_note,
        .out_signal = &g_sound,
        .oscillators = init_osc_array()
    };
    //todo: move somewhere in initialization
    synth.ui_oscillators = malloc(sizeof(OscillatorUI) * synth.oscillators.count);
    for (size_t i = 0; i < synth.oscillators.count; i++)
    {
        OscillatorUI* ui = &synth.ui_oscillators[i];
        ui->freq = synth.oscillators.array[i].freq;
        ui->waveshape = synth.oscillators.array[i].osc;
        ui->volume = synth.oscillators.array[i].volume;
    }
    
    int sound_played_count = 0;
    float temp_buffer[STREAM_BUFFER_SIZE];
    RingBuffer ring_buffer = ring_buffer_init(STREAM_BUFFER_SIZE);

    InitAudioDevice();
    SetMasterVolume(SYNTH_VOLUME);
    SetAudioStreamBufferSizeDefault(STREAM_BUFFER_SIZE);
    AudioStream synth_stream = LoadAudioStream(SAMPLE_RATE, sizeof(float) * 8, 1);
    SetAudioStreamVolume(synth_stream, 0.5f);

    PlayAudioStream(synth_stream);
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update Audio states
        //----------------------------------------------------------------------------------
        // Fill ring buffer from current sound
        SynthSound* sound = synth.out_signal;
        size_t size_for_buffer = 0;
        if (!ring_buffer.is_full && sound->sample_count != sound_played_count) {
            write_log("[INFO] IsFull:%d Samples:%zu Played:%zu\n", 
                ring_buffer.is_full, 
                sound->sample_count, 
                sound_played_count);
            
            // how many samples need write
            size_t size_to_fill = 0;
            
            if ((sound->sample_count - sound_played_count) > ring_buffer.size) {
                size_to_fill = ring_buffer.size;
            } else {
                size_to_fill = sound->sample_count - sound_played_count;
            }

            write_log("[INFO] SizeToFill:%zu\n", size_to_fill);
            for (size_t i = 0; i < size_to_fill; i++) {
                temp_buffer[i] = sound->samples[i];
            }
            
            ring_buffer_write(&ring_buffer, temp_buffer, size_to_fill);
            sound_played_count += size_to_fill;
        }

        // Play ring-buffered audio
        if (IsAudioStreamProcessed(synth_stream) && !ring_buffer.is_empty) {
                size_t size_to_read = ring_buffer_size(&ring_buffer);
                
                write_log("Samples to play:%zu \n", size_to_read);
                //todo: try to start reading directly from ring buffer, avoiding temp_buffer
                ring_buffer_read(&ring_buffer, temp_buffer, size_to_read);
                // can try the SetAudioStreamCallback
                UpdateAudioStream(synth_stream, temp_buffer, size_to_read);
                // can overwrite the ring buffer to avoid that
                if (sound->sample_count == sound_played_count) {
                    ring_buffer_reset(&ring_buffer);
                }
        }
        //----------------------------------------------------------------------------------
        
        // Update On Input
        //----------------------------------------------------------------------------------
        Note current_note = synth.current_note;
        if (detect_note_pressed(&current_note)) {
            *sound = get_note_sound(&synth, current_note);
            sound_played_count = 0;
            write_log("Note played: %s\n", current_note.name);
        }
        //----------------------------------------------------------------------------------
 
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
 
            ClearBackground(RAYWHITE);
            DrawUi(&synth);
            DrawSignal(&synth);
            //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            //DrawFPS(0,0);
 
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    
    char* input = "A4-4 A4-4 A4-4 A4-4 A4-2 A4-4 A4-4 A4-4 A4-4 A4-4 A4-2 D5-4 D5-4 D5-4 D5-4 D5-4 D5-4 D5-2 C5-4 C5-4 C5-4 C5-4 C5-4 C5-4 C5-2 G4-2 ";
    char* buf = malloc(strlen(input) + 1);
    strcpy(buf, input);

    NoteArray note_array = parse_notes(buf, strlen(buf));
    SynthSound* sounds = malloc(sizeof(SynthSound) * note_array.count);
    for (size_t i = 0; i < note_array.count; i++) {
        Note note = note_array.notes[i];
        sounds[i] = get_note_sound(&synth, note);
    }

    SynthSound song = concat_sounds(sounds, note_array.count);
    uint16_t* song_pcm = malloc(sizeof(uint16_t) * song.sample_count);
    for (size_t i = 0; i < song.sample_count; i++) {
        song_pcm[i] = toInt16Sample(song.samples[i]);
    }

    pack(song_pcm, song.sample_count);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    StopAudioStream(synth_stream);
    UnloadAudioStream(synth_stream);    
    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
