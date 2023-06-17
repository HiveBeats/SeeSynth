#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "parser.h"

#include "raylib.h"

#define SAMPLE_RATE 48000.f
#define BPM 120.f
#define BEAT_DURATION 60.f/BPM
#define PITCH_STANDARD 440.f
#define VOLUME 0.5f
#define ATTACK_MS 100.f

#define SYNTH_PI 3.1415926535f
#define SYNTH_VOLUME 0.5f

//------------------------------------------------------------------------------------
// General SynthSound
//------------------------------------------------------------------------------------

typedef struct SynthSound {
    float* samples;
    size_t sample_count;
} SynthSound;

float synth_buffer[1024];
SynthSound synth_sound;

// frees the original sounds
SynthSound concat_sounds(SynthSound* sounds, size_t count) {
    size_t total_count = 0;
    for (size_t i = 0; i < count; i++) {
        total_count += sounds[i].sample_count;
    }
    // array to hold the result
    float* total = malloc(total_count * sizeof(float));

    size_t current_count = 0;
    for (size_t i = 0; i < count; i++) {
        memcpy(total + current_count,
            sounds[i].samples,
            sounds[i].sample_count * sizeof(float));
        current_count += sounds[i].sample_count;

        free(sounds[i].samples);
    }

    SynthSound result = {
        .samples = total,
        .sample_count = total_count
    };

    return result;
}

//------------------------------------------------------------------------------------
// Oscillator
//------------------------------------------------------------------------------------

typedef enum {
    Sine,
    Triangle,
    Saw,
    Square
} OscillatorType;

typedef struct OscillatorParameter {
    OscillatorType osc;
    float freq;
} OscillatorParameter;

typedef struct OscillatorParameterList {
    OscillatorParameter* array;
    size_t count;
} OscillatorParameterList;

typedef struct OscillatorGenerationParameter {
    OscillatorParameterList oscillators;
    float sample;
} OscillatorGenerationParameter;

static SynthSound get_init_samples(float duration) {
    size_t sample_count = (size_t)(duration * SAMPLE_RATE);
    float* samples = malloc(sizeof(float) * sample_count);

    for (double i = 0.0; i < duration * SAMPLE_RATE; i++) {
        samples[(int)i] = i;
    }

    SynthSound res = {
        .samples = samples,
        .sample_count = sample_count
    };

    return res;
}

static float pos(float hz, float x) {
    return fmodf(hz * x / SAMPLE_RATE, 1);
}

float sineosc(float hz, float x) {
    return sinf(x * (2.f * SYNTH_PI * hz / SAMPLE_RATE));
}

static float sign(float v) {
    return (v > 0.0) ? 1.f : -1.f;
}

float squareosc(float hz, float x) {
    return sign(sineosc(hz, x));
}

float triangleosc(float hz, float x) {
    return 1.f - fabsf(pos(hz, x) - 0.5f) * 4.f;
}

float sawosc(float hz, float x) {
    return pos(hz, x) * 2.f - 1.f;
}

float multiosc(OscillatorGenerationParameter param) {
    float osc_sample = 0.f;
    for (size_t i = 0; i < param.oscillators.count; i++) {
        OscillatorParameter osc = param.oscillators.array[i];
        switch (osc.osc) {
            case Sine:
                osc_sample += sineosc(osc.freq, param.sample);
                break;
            case Triangle:
                osc_sample += triangleosc(osc.freq, param.sample);
                break;
            case Square:
                osc_sample += squareosc(osc.freq, param.sample);
                break;
            case Saw:
                osc_sample += sawosc(osc.freq, param.sample);
                break;
        }
    }

    return osc_sample;
}

static SynthSound freq(float duration, OscillatorParameterList osc) {
    SynthSound samples = get_init_samples(duration);
    // SynthSound attack = get_attack_samples();

    float* output = malloc(sizeof(float) * samples.sample_count);
    for (int i = 0; i < samples.sample_count; i++) {
        float sample = samples.samples[i];
        OscillatorGenerationParameter param = {
            .oscillators = osc,
            .sample = sample
        };
        output[i] = multiosc(param) * VOLUME;
    }

    // create attack and release
    /*
        let adsrLength = Seq.length output
        let attackArray = attack |> Seq.take adsrLength
        let release = Seq.rev attackArray
    */
    /*
     todo: I will change the ADSR approach to an explicit ADSR module(with it's own state)
    size_t adsr_length = samples.sample_count;
    float *attackArray = NULL, *releaseArray = NULL;

    if (adsr_length > 0) {
        //todo: calloc
        attackArray = malloc(sizeof(float) * adsr_length);
        size_t attack_length = attack.sample_count < adsr_length
                ? attack.sample_count
                : adsr_length;
        
        memcpy(attackArray, attack.samples, attack_length);
        //todo: calloc
        releaseArray = malloc(sizeof(float) * adsr_length);
        memcpy(releaseArray, attackArray, attack_length);
        reverse_array(releaseArray, 0, adsr_length);
    }
    */

    // if (samples.sample_count > 1024) {
    //     samples.sample_count = 1024;
    // }
    // //todo: move to somewhere
    // for (size_t i = 0; i < 1024; i++) {
    //     synth_sound.samples[i] = 0.0f;
    // }

    // for (size_t i = 0; i < samples.sample_count; i++) {
    //     synth_sound.samples[i] = output[i];
    // }
    // synth_sound.sample_count = samples.sample_count;


    // return zipped array
    SynthSound res = {
        .samples = output,
        .sample_count = samples.sample_count
    };

    return res;
}

/*
static SynthSound get_attack_samples() {
    float attack_time = 0.001 * ATTACK_MS;
    size_t sample_count = (size_t)(attack_time * SAMPLE_RATE);
    float* attack = malloc(sizeof(float) * sample_count);
    float samples_to_rise = SAMPLE_RATE * attack_time;
    float rising_delta = 1.0 / samples_to_rise;
    float i = 0.0;

    for (int j = 0; j < sample_count; j++) {
        i += rising_delta;
        attack[j] = fmin(i, 1.0);
    }

    SynthSound res = {
        .samples = attack,
        .sample_count = sample_count
    };

    return res;
}
*/

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
        .osc = Saw,
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


//------------------------------------------------------------------------------------
// Wav File 
//------------------------------------------------------------------------------------

static uint16_t toInt16Sample(float sample) {
    return (uint16_t)(sample * 32767.f);
}

static void write_file(char* filename, void* data, int size) {
    FILE* fp = fopen(filename, "wb");  // open file for writing in binary mode
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        exit(1);
    }

    fwrite(data, size, 1, fp);  // write data to file

    fclose(fp);  // close file
}

void pack(uint16_t* d, size_t length) {
    size_t dataLength = length * 2;

    int bytesPerSample = 2;
    int byteRate = SAMPLE_RATE * bytesPerSample;

    size_t fileSize = 36 + dataLength;

    uint8_t* buffer = (uint8_t*)malloc(fileSize);

    int i = 0;

    // RIFF header
    memcpy(buffer + i, "RIFF", 4);
    i += 4;
    memcpy(buffer + i, &fileSize, 4);
    i += 4;
    memcpy(buffer + i, "WAVE", 4);
    i += 4;

    // fmt subchunk
    memcpy(buffer + i, "fmt ", 4);
    i += 4;
    int fmtSize = 16;
    memcpy(buffer + i, &fmtSize, 4);
    i += 4;
    uint16_t audioFormat = 1;
    memcpy(buffer + i, &audioFormat, 2);
    i += 2;
    uint16_t numChannels = 1;
    memcpy(buffer + i, &numChannels, 2);
    i += 2;
    int sampleRate = (int)SAMPLE_RATE;
    memcpy(buffer + i, &sampleRate, 4);
    i += 4;
    memcpy(buffer + i, &byteRate, 4);
    i += 4;
    memcpy(buffer + i, &bytesPerSample, 2);
    i += 2;
    int bitsPerSample = bytesPerSample * 8;
    memcpy(buffer + i, &bitsPerSample, 2);
    i += 2;

    // data subchunk
    memcpy(buffer + i, "data", 4);
    i += 4;
    memcpy(buffer + i, &dataLength, 4);
    i += 4;
    memcpy(buffer + i, d, dataLength);

    write_file("output.wav", buffer, fileSize);
}

size_t detect_note_pressed(Note* note) {
    size_t is_pressed = 0;
    note->length = 16;
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
    const size_t width = 640;
    const size_t height = 480;

    InitWindow(width, height, "SeeSynth - v0.1");
    SetTargetFPS(120); 

    Note current_note = {
        .length = 1,
        .name = malloc(sizeof(char) * 3)
    };

    synth_sound = (SynthSound){.sample_count = 1024, .samples = synth_buffer};

    InitAudioDevice();
    SetMasterVolume(SYNTH_VOLUME);
    AudioStream synth_stream = LoadAudioStream(SAMPLE_RATE, sizeof(float) * 8, 1);//float*8
    SetAudioStreamVolume(synth_stream, 0.5f);
    PlayAudioStream(synth_stream);
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (detect_note_pressed(&current_note)) {
            //get_note_sound(current_note);
            SynthSound sound = get_note_sound(current_note);
            //sound_buffer.sample_count = sound.sample_count;
            //memcpy(sound_buffer.samples, sound.samples, sound.sample_count);
            if (IsAudioStreamReady(synth_stream)) {
                if (IsAudioStreamProcessed(synth_stream)) {
                    printf("Samples to play:%zu \n", sound.sample_count);
                    UpdateAudioStream(synth_stream, sound.samples, sound.sample_count);
                }
            }
            //}
            printf("Note played: %s\n", current_note.name);
        }
        //UpdateAudioStream(synth_stream, synth_sound.samples, synth_sound.sample_count);
        //----------------------------------------------------------------------------------
 
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
 
            ClearBackground(RAYWHITE);
 
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
 
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
