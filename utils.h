#ifndef UTILS_H
#define UTILS_H

#include "stdio.h"

#define write_log(format,args...) do { \
        printf(format, ## args); \
    } while(0)

//------------------------------------------------------------------------------------
// General SynthSound
//------------------------------------------------------------------------------------

typedef struct SynthSound {
    float* samples;
    size_t sample_count;
} SynthSound;

// frees the original sounds
SynthSound concat_sounds(SynthSound* sounds, size_t count);

#endif