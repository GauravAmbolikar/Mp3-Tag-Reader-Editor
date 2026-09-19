#ifndef READER_H
#define READER_H

#include<stdio.h>
#include "types.h" 

typedef struct {
    char *mp3_filename;
    FILE *fptr_mp3;
    int version_major;
    int version_minor;
    int totalTagSize;
    char title[50];
    char album[50];
    char year[10];      
    char track[10];     
    char genre[50];
    char artist[50];
    char comment[100];  
} MP3Tags;

OperationType check_operation_type(int argc, char *argv[]);
Status read_and_validate_args(char *argv[], MP3Tags *mp3);
Status open_files(MP3Tags *mp3);
Status read_id3_header(MP3Tags *mp3);
void decode_text(unsigned char *buffer, int frameSize, char *dest, int destSize);
Status mp3_viewer(MP3Tags *mp3);

char *find_tag(char *str);
Status find_frame(char *frameID, MP3Tags *mp3, long *pos, int *size);
Status create_new_frame(char *frameID, char *newText, unsigned char **newFrame, int *newSize);
Status mp3_editor(char *argv[], MP3Tags *mp3);
Status rewrite_file(MP3Tags *mp3, long framePos, int oldFrameSize, unsigned char *newFrame, int newFrameSize);
void print_mp3_tags(MP3Tags *mp3);

#endif
