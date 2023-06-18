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

//------------------------------------------------------------------------------------
// Synth
//------------------------------------------------------------------------------------

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

SynthSound note(int semitone, float beats) {
    float hz = get_hz_by_semitone(semitone);
    float duration = beats * BEAT_DURATION;

    OscillatorParameter first = {
        .osc = Square,
        .freq = hz
    };

    OscillatorParameter second = {
        .osc = Saw,
        .freq = hz + 0.5
    };

    OscillatorParameter third = {
        .osc = Saw,
        .freq = hz - 1.f
    };

    OscillatorParameter oscArray[] = { first/*, second, third */};
    OscillatorParameterList parameters = {
        .array = oscArray,
        .count = 1
    };

    return freq(duration, parameters);
}

SynthSound get_note_sound(Note input) {
    float length = 1.f / input.length;
    int semitone_shift = get_semitone_shift(input.name);
    return note(semitone_shift, length);
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
// Main
//------------------------------------------------------------------------------------

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "SeeSynth - v0.1");
    SetTargetFPS(60);

    Note current_note = {
        .length = 1,
        .name = malloc(sizeof(char) * 3)
    };

    SynthSound sound = {
        .sample_count = 0
    };
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
        size_t size_for_buffer = 0;
        if (!ring_buffer.is_full && sound.sample_count != sound_played_count) {
            write_log("[INFO] IsFull:%d Samples:%zu Played:%zu\n", 
                ring_buffer.is_full, 
                sound.sample_count, 
                sound_played_count);
            
            // how many samples need write
            size_t size_to_fill = 0;
            
            if ((sound.sample_count - sound_played_count) > ring_buffer.size) {
                size_to_fill = ring_buffer.size;
            } else {
                size_to_fill = sound.sample_count - sound_played_count;
            }

            write_log("[INFO] SizeToFill:%zu\n", size_to_fill);
            for (size_t i = 0; i < size_to_fill; i++) {
                temp_buffer[i] = sound.samples[i];
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
                if (sound.sample_count == sound_played_count) {
                    ring_buffer_reset(&ring_buffer);
                }
        }
        //----------------------------------------------------------------------------------
        
        // Update On Input
        //----------------------------------------------------------------------------------
        if (detect_note_pressed(&current_note)) {
            sound = get_note_sound(current_note);
            sound_played_count = 0;
            write_log("Note played: %s\n", current_note.name);
        }
        //----------------------------------------------------------------------------------
 
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
 
            ClearBackground(RAYWHITE);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            DrawFPS(0,0);
 
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
        sounds[i] = get_note_sound(note);
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
