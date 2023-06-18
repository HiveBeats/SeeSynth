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
#define STREAM_BUFFER_SIZE 4096

#define SYNTH_PI 3.1415926535f
#define SYNTH_VOLUME 0.5f

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define write_log(format,args...) do { \
        printf(format, ## args); \
    } while(0)

//------------------------------------------------------------------------------------
// Ring Buffer
//------------------------------------------------------------------------------------

typedef struct RingBuffer {
    float* items;
    size_t head;
    size_t tail;
    int is_full;
    int is_empty;
    size_t size;
} RingBuffer;

RingBuffer ring_buffer_init(size_t buffer_size) {
    RingBuffer buffer = {
        .items = calloc(buffer_size, sizeof(float)),
        .head = 0,
        .tail = 0,
        .is_full = 0,
        .is_empty = 1,
        .size = buffer_size
    };

    return buffer;
}

void ring_buffer_reset(RingBuffer* me) {
    me->head = 0;
    me->tail = 0;
    me->is_full = 0;
}

// +
static void advance_pointer(RingBuffer* me) {
    if(me->is_full) {
        me->tail++;
		if (me->tail == me->size) { 
			me->tail = 0;
		}
	}
    me->head++;
	if (me->head == me->size) { 
		me->head = 0;
	}
    size_t is_full = me->head == me->tail ? 1 : 0;
	me->is_full = is_full;
}

// -
static void retreat_pointer(RingBuffer* me) {
	me->is_full = 0;
    me->tail++;
	if (me->tail == me->size) { 
		me->tail = 0;
	}
}

void ring_buffer_write(RingBuffer* buffer, float* data, size_t count) {
    if (buffer->is_full || buffer->head + count > buffer->size) {
        write_log("[WARN] Trying to overfill the ring buffer: \n\tIsFull:%d\n\tHead:%zu\n\tCount:%zu\n\t", 
            buffer->is_full,
            buffer->head, 
            count);
        return;
    }
    buffer->is_empty = 0;

    for (size_t i = 0; i < count; i++) {
        buffer->items[buffer->head] = data[i];
        advance_pointer(buffer);
    }
    //me->is_empty = is_full && (me->head == me->tail);
}

int ring_buffer_read(RingBuffer* buffer, float* output, size_t count) {
    if (buffer->is_empty) {
        write_log("[WARN] Trying to read empty buffer");
        return 0;
    }

    for (size_t i = 0; i < count; i++) {
        output[i] = buffer->items[buffer->tail];
        retreat_pointer(buffer);
    }
    buffer->is_empty = !buffer->is_full && (buffer->head == buffer->tail);
    return 1;
}

size_t ring_buffer_size(RingBuffer* buffer) {
	size_t size = buffer->size;
	if(!buffer->is_full) {
		if(buffer->head >= buffer->tail) {
			size = (buffer->head - buffer->tail);
		}
		else {
			size = (buffer->size + buffer->head - buffer->tail);
		}
	}

	return size;
}

void ring_buffer_print(RingBuffer* me) {
    write_log("[INFO] The ring buffer: \n\tIsFull:%d\n\tIsEmpty:%d\n\tHead:%zu\n\tTail:%zu\n\t", 
                me->is_full, 
                me->is_empty, 
                me->head, 
                me->tail);
}


//------------------------------------------------------------------------------------
// General SynthSound
//------------------------------------------------------------------------------------

typedef struct SynthSound {
    float* samples;
    size_t sample_count;
} SynthSound;

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
// UI
//------------------------------------------------------------------------------------
/*
int get_zero_crossing(SynthSound* sound) {
    int zero_crossing_index = 0;
    for (size_t i = 1; i < sound->sample_count; i++)
    {
        if (sound->samples[i] >= 0.0f && sound->samples[i-1] < 0.0f) // zero-crossing
        {
            zero_crossing_index = i;
            break;
        }
    }
    return zero_crossing_index;
}

Vector2* GetVisualSignal(SynthSound* sound, int zero_crossing_index)
{
    const int signal_amp = 300;
    
    Vector2* signal_points = malloc(sizeof(Vector2) * STREAM_BUFFER_SIZE);
    
    const float screen_vertical_midpoint = (WINDOW_HEIGHT/2);
    for (size_t point_idx = 0; point_idx < sound->sample_count; point_idx++)
    {
        const int signal_idx = (point_idx + zero_crossing_index) % STREAM_BUFFER_SIZE;
        signal_points[point_idx].x = (float)point_idx + WINDOW_WIDTH;
        signal_points[point_idx].y = screen_vertical_midpoint + (int)(sound->samples[signal_idx] * 300);
    }

    return signal_points;
}

*/

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
    AudioStream synth_stream = LoadAudioStream(SAMPLE_RATE, sizeof(float) * 8, 1);//float*8
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
            // int zero_crossing = get_zero_crossing(&sound);
            // Vector2* visual_signal = GetVisualSignal(&sound, zero_crossing);
            // DrawLineStrip(visual_signal, STREAM_BUFFER_SIZE - zero_crossing, RED);
 
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
