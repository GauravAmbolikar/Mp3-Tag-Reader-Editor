#include <stdio.h>
#include "mp3_reader.h"
#include "types.h"
#include<string.h>

Status read_and_validate_args(char *argv[], MP3Tags *mp3)
{
    if(strstr(argv[2],".mp3") != NULL)
    {
        mp3->mp3_filename = argv[2];
        return e_success;
    }
    return e_failure;
}

Status open_files(MP3Tags *mp3)
{
    mp3->fptr_mp3 = fopen(mp3->mp3_filename,"rb");
    // Do Error handling
    if (mp3->fptr_mp3 == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", mp3->mp3_filename);

    	return e_failure;
    }
    return e_success;
}

Status read_id3_header(MP3Tags *mp3)
{
    unsigned char header[10];

    // Step 1: read the fixed 10-byte header
    fread(header, 1, 10, mp3->fptr_mp3);

    // Step 2: validate "ID3" identifier
    if (header[0] != 'I' || header[1] != 'D' || header[2] != '3')
    {
        fprintf(stderr, "ERROR: No ID3v2 tag found\n");
        return e_failure;
    }

    // Step 3: extract Version ID
    mp3->version_major = header[3];   // e.g. 0x03 -> 3
    mp3->version_minor = header[4];   // usually 0x00

    // Step 4: decode synchsafe size (bytes 6-9)
    mp3->totalTagSize = ((header[6] & 0x7F) << 21) |
                         ((header[7] & 0x7F) << 14) |
                         ((header[8] & 0x7F) << 7)  |
                         (header[9] & 0x7F);

    return e_success;
}

Status mp3_viewer(MP3Tags *mp3)
{
    if (read_id3_header(mp3) == e_failure) {
        return e_failure;
    }

    int bytesRead = 0;
    while (bytesRead < mp3->totalTagSize)
    {
        char frameID[5];
        fread(frameID, 1, 4, mp3->fptr_mp3);
        frameID[4] = '\0';

        if (frameID[0] == 0x00) break;

        unsigned char sizeBytes[4];
        fread(sizeBytes, 1, 4, mp3->fptr_mp3);
        int frameSize = (sizeBytes[0]<<24)|(sizeBytes[1]<<16)|(sizeBytes[2]<<8)|sizeBytes[3];

        fseek(mp3->fptr_mp3, 2, SEEK_CUR);

        if (strcmp(frameID, "TIT2") == 0) {
            char buffer[frameSize+1];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            buffer[frameSize] = '\0';
            strncpy(mp3->title, buffer + 1, sizeof(mp3->title) - 1);
            mp3->title[sizeof(mp3->title) - 1] = '\0';
        }
        else if (strcmp(frameID, "TPE1") == 0) {
            char buffer[frameSize+1];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            buffer[frameSize] = '\0';
            strncpy(mp3->artist, buffer + 1, sizeof(mp3->artist) - 1);
            mp3->artist[sizeof(mp3->artist) - 1] = '\0';
        }
        else if (strcmp(frameID, "TALB") == 0) {
            char buffer[frameSize+1];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            buffer[frameSize] = '\0';
            strncpy(mp3->album, buffer + 1, sizeof(mp3->album) - 1);
            mp3->album[sizeof(mp3->album) - 1] = '\0';
        }
        else if (strcmp(frameID, "TYER") == 0) {
            char buffer[frameSize+1];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            buffer[frameSize] = '\0';
            strncpy(mp3->year, buffer + 1, sizeof(mp3->year) - 1);
            mp3->year[sizeof(mp3->year) - 1] = '\0';
        }
        // else if (strcmp(frameID, "TRCK") == 0) {
        //     char buffer[frameSize+1];
        //     fread(buffer, 1, frameSize, mp3->fptr_mp3);
        //     buffer[frameSize] = '\0';
        //     strncpy(mp3->track, buffer + 1, sizeof(mp3->track) - 1);
        //     mp3->track[sizeof(mp3->track) - 1] = '\0';
        // }
        else if (strcmp(frameID, "TCON") == 0) {
            char buffer[frameSize+1];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            buffer[frameSize] = '\0';
            strncpy(mp3->genre, buffer + 1, sizeof(mp3->genre) - 1);
            mp3->genre[sizeof(mp3->genre) - 1] = '\0';
        }
        else if (strcmp(frameID, "COMM") == 0) {
            char buffer[frameSize+1];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            buffer[frameSize] = '\0';
            strncpy(mp3->comment, buffer + 1, sizeof(mp3->genre) - 1);
            mp3->genre[sizeof(mp3->genre) - 1] = '\0';
        }
        else {
            fseek(mp3->fptr_mp3, frameSize, SEEK_CUR);
        }

        bytesRead += 10 + frameSize;
    }

    return e_success;
}

void print_mp3_tags(MP3Tags *mp3)
{
    printf("Mp3 Tag Reader & Editor:\n");
    printf("------------------------\n");
    printf("Version ID : 2.%d\n", mp3->version_major);
    printf("Title : %s\n", mp3->title);
    printf("Album : %s\n", mp3->album);
    printf("Year : %s\n", mp3->year);
    // printf("Track : %s\n", mp3->track);
    printf("Genre : %s\n", mp3->genre);
    printf("Artist : %s\n", mp3->artist);
    printf("Comment : %s\n", mp3->comment);
    printf("\n");
    printf("Extracting Album Art - Done\n");
}