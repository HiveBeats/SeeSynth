#include "export.h"

#include "stdio.h"
#include "string.h"
#include "settings.h"

uint16_t toInt16Sample(float sample) {
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