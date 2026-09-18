#ifndef READER_H
#define READER_H

#include<stdio.h>
#include "types.h" // Contains user defined types

typedef struct {
    char *mp3_filename;
    FILE *fptr_mp3;
    int version_major;
    int version_minor;
    int totalTagSize;
    char title[50];
    char album[50];
    char year[10];      // "2017" is short, doesn't need 50
    char track[10];     // "7/8" is short too
    char genre[50];
    char artist[50];
    char comment[100];  // comments tend to be longer
} MP3Tags;

OperationType check_operation_type(int argc, char *argv[]);
Status read_and_validate_args(char *argv[], MP3Tags *mp3);
Status open_files(MP3Tags *mp3);
Status read_id3_header(MP3Tags *mp3);
Status mp3_viewer(MP3Tags *mp3);
void print_mp3_tags(MP3Tags *mp3);

#endif
