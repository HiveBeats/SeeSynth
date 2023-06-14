#include "parser.h"
#include "string.h"
#include "stdio.h"

struct StringArray {
    char** array;
    size_t count;
};

static void trim(char* str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == ' ')) {
        str[--len] = '\0';
    }
}

static struct StringArray parse_note_parts(char* input) {
    size_t count = 0;
    size_t i = 0;
    while (input[i] != '\0') {
        if (input[i] == ' ')
            count++;

        i++;
    }

    char** array = malloc(sizeof(char*) * count);

    char* sep = " ";
    char* line = strtok(input, sep);
    i = 0;
    while (line != NULL) {
        array[i] = strdup(line);
        line = strtok(NULL, sep);
        i++;
    }

    struct StringArray result = {
        .array = array,
        .count = count
    };

    return result;
}

NoteArray parse_notes(char* input, size_t len) {
    struct StringArray note_strings = parse_note_parts(input);
    
    NoteArray notes;
    notes.count = note_strings.count;

    char* end;
    for (size_t i = 0; i < note_strings.count; i++) {
        char* line = note_strings.array[i];
        trim(line);
        
        char* note_name = strtok(line, "-");
        char* note_length_str = strtok(NULL, "-");

        int note_length = strtol(note_length_str, &end, 10);
        if (*end != '\0') {
            fprintf(stderr,
                "Failed to parse note length: %s\n", note_length_str);
            return notes;
        }
        
        char* buf = malloc(strlen(note_name) + 1);
        strcpy(buf, note_name);
        
        Note note = {
            .length = note_length,
            .name = buf
        };

        notes.notes[i] = note;
    }

    return notes;
}

/*
static int test(int argc, char **argv) {
    char* input = "A4-4 A4-2 C5-8 C5-4 ";
    char* buf = malloc(strlen(input) + 1);
    strcpy(buf, input);

    NoteArray note_array = parse_notes(buf, strlen(buf));
    for (size_t i = 0; i < note_array.count; i++) {
        Note note = note_array.notes[i];
    }

    return 0;
}
*/
