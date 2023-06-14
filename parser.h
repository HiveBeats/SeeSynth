#ifndef PARSER_H
#define PARSER_H

#include "stdlib.h"

#define MAX_NOTES 1024

typedef struct Note {
    char* name;
    int length;
} Note;

typedef struct NoteArray {
    Note notes[MAX_NOTES];
    size_t count;
} NoteArray;

NoteArray parse_notes(char* input, size_t len);

#endif
