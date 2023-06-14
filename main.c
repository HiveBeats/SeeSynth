#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "parser.h"

#define SAMPLE_RATE 48000.f
#define BPM 120.f
#define BEAT_DURATION 60.f/BPM
#define PITCH_STANDARD 440.f
#define VOLUME 0.5f
#define ATTACK_MS 100.f

#define PI 3.1415926535f

typedef struct Sound {
    float* samples;
    size_t sample_count;
} Sound;

/*
static void reverse_array(float arr[], size_t start, size_t end)
{
    float temp;
    while (start < end)
    {
        temp = arr[start];   
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}
*/

static size_t get_semitone_shift_internal(char* root_note, char* target_note) {
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

static float get_hz_by_semitone(size_t semitone) {
    return PITCH_STANDARD * powf(powf(2.f, (1.f / 12.f)), semitone);
}

static Sound get_init_samples(float duration) {
    size_t sample_count = (size_t)(duration * SAMPLE_RATE);
    float* samples = malloc(sizeof(float) * sample_count);

    for (double i = 0.0; i < duration * SAMPLE_RATE; i++) {
        samples[(int)i] = i / SAMPLE_RATE;
    }

    Sound res = {
        .samples = samples,
        .sample_count = sample_count
    };

    return res;
}

/*
static Sound get_attack_samples() {
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

    Sound res = {
        .samples = attack,
        .sample_count = sample_count
    };

    return res;
}
*/

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

static float pos(float hz, float x) {
    return fmodf(hz * x / SAMPLE_RATE, 1);
}

float sineosc(float hz, float x) {
    return sinf(x * (2.f * PI * hz / SAMPLE_RATE));
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

static Sound freq(float duration, OscillatorParameterList osc) {
    Sound samples = get_init_samples(duration);
    // Sound attack = get_attack_samples();

    float* output = malloc(sizeof(float) * samples.sample_count);
    for (int i = 0; i < samples.sample_count; i++) {
        float sample = samples.samples[i];
        // todo: 
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

    // return zipped array
    Sound res = {
        .samples = output,
        .sample_count = samples.sample_count
    };

    return res;
}

size_t get_semitone_shift(char* target_note) {
    return get_semitone_shift_internal("A4", target_note);
}

Sound note(size_t semitone, float beats) {
    float hz = get_hz_by_semitone(semitone);
    float duration = beats * BEAT_DURATION;

    OscillatorParameter first = {
        .osc = Saw,
        .freq = hz/4.f
    };

    OscillatorParameter second = {
        .osc = Saw,
        .freq = hz + 0.5
    };

    OscillatorParameter third = {
        .osc = Saw,
        .freq = hz - 1.f
    };

    OscillatorParameter oscArray[] = { first, second, third };
    OscillatorParameterList parameters = {
        .array = oscArray,
        .count = 3
    };

    return freq(duration, parameters);
}

Sound get_note_sound(Note input) {
    float length = 1.f / input.length;
    size_t semitone_shift = get_semitone_shift(input.name);
    return note(semitone_shift, length);
}

// frees the original sounds
Sound concat_sounds(Sound* sounds, size_t count) {
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

    Sound result = {
        .samples = total,
        .sample_count = total_count
    };

    return result;
}

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



int main(int argc, char **argv) {
    char* input = "A4-4 A4-4 A4-4 A4-4 A4-2 A4-4 A4-4 A4-4 A4-4 A4-4 A4-2 D5-4 D5-4 D5-4 D5-4 D5-4 D5-4 D5-2 C5-4 C5-4 C5-4 C5-4 C5-4 C5-4 C5-2 G4-2 ";
    char* buf = malloc(strlen(input) + 1);
    strcpy(buf, input);

    NoteArray note_array = parse_notes(buf, strlen(buf));
    Sound* sounds = malloc(sizeof(Sound) * note_array.count);
    for (size_t i = 0; i < note_array.count; i++) {
        Note note = note_array.notes[i];
        sounds[i] = get_note_sound(note);
    }

    Sound song = concat_sounds(sounds, note_array.count);
    uint16_t* song_pcm = malloc(sizeof(uint16_t) * song.sample_count);
    for (size_t i = 0; i < song.sample_count; i++) {
        song_pcm[i] = toInt16Sample(song.samples[i]);
        // printf("Sample: %d\n", song_pcm[i]);
    }

    pack(song_pcm, song.sample_count);

    return 0;
}
