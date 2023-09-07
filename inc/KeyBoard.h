#pragma once
#include "Settings.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <string>

class KeyBoard {
  private:
    static int get_semitone_shift_internal(const char* root_note,
                                           char* target_note) {
        const char* pitch_classes[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                         "F#", "G",  "G#", "A",  "A#", "B"};

        // Extract the note number and pitch class for the root note
        int root_note_num = (int)root_note[strlen(root_note) - 1] - '0';

        char* root_pitch_class_str =
            (char*)malloc((strlen(root_note) - 1) * sizeof(char));
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
            (char*)malloc((strlen(target_note) - 1) * sizeof(char));
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

  public:
    KeyBoard(/* args */);
    ~KeyBoard();

    static float GetHzBySemitone(int semitone) {
        return PITCH_STANDARD * powf(powf(2.f, (1.f / 12.f)), semitone);
    }

    static int GetSemitoneShift(const std::string& target_note) {
        char* target_note_cstr = new char[target_note.length() + 1];
        strcpy(target_note_cstr, target_note.c_str());

        int result = get_semitone_shift_internal("A4", target_note_cstr);

        delete[] target_note_cstr;
        return result;
    }
};

KeyBoard::KeyBoard(/* args */) {}

KeyBoard::~KeyBoard() {}
