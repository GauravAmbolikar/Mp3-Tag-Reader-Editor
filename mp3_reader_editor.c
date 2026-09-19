#include <stdio.h>
#include "mp3_reader_editor.h"
#include "types.h"
#include<string.h>
#include<stdlib.h>

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

    fread(header, 1, 10, mp3->fptr_mp3);

    if (header[0] != 'I' || header[1] != 'D' || header[2] != '3')
    {
        fprintf(stderr, "ERROR: No ID3v2 tag found\n");
        return e_failure;
    }

    mp3->version_major = header[3];   
    mp3->version_minor = header[4];  
    mp3->totalTagSize = ((header[6] & 0x7F) << 21) | ((header[7] & 0x7F) << 14) | ((header[8] & 0x7F) << 7)  | (header[9] & 0x7F);

    return e_success;
}

void decode_text(unsigned char *buffer, int frameSize, char *dest, int destSize)
{
    int encoding = buffer[0];

    if (encoding == 0)
    {
        // ISO-8859-1 / ASCII 
        strncpy(dest, (char *)buffer + 1, destSize - 1);
        dest[destSize - 1] = '\0';
    }
    else if (encoding == 1)
    {
        // UTF-16LE
        int j = 0;

        for (int i = 3; i < frameSize && j < destSize - 1; i += 2)
        {
            dest[j++] = buffer[i];
        }

        dest[j] = '\0';
    }
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

        if (strcmp(frameID, "TIT2") == 0)
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->title, sizeof(mp3->title));
        }
        else if (strcmp(frameID, "TPE1") == 0)
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->artist, sizeof(mp3->artist));
        }
        else if (strcmp(frameID, "TALB") == 0)
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->album, sizeof(mp3->album));
        }
        else if (strcmp(frameID, "TYER") == 0)
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->year, sizeof(mp3->year));
        }
        else if (strcmp(frameID, "TRCK") == 0)
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->track, sizeof(mp3->track));
        }
        else if (strcmp(frameID, "TCON") == 0)
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->genre, sizeof(mp3->genre));
        }
        else if (strcmp(frameID, "COMM") == 0) 
        {
            unsigned char buffer[frameSize];
            fread(buffer, 1, frameSize, mp3->fptr_mp3);
            decode_text(buffer, frameSize, mp3->comment, sizeof(mp3->comment));
        }
        else {
            fseek(mp3->fptr_mp3, frameSize, SEEK_CUR);
        }

        bytesRead += 10 + frameSize;
    }

    return e_success;
}

char *find_tag(char *str)
{
    if (strcmp(str, "-t") == 0)
        return "TIT2";

    else if (strcmp(str, "-a") == 0)
        return "TPE1";

    else if (strcmp(str, "-A") == 0)
        return "TALB";

    else if (strcmp(str, "-y") == 0)
        return "TYER";

    else if (strcmp(str, "-g") == 0)
        return "TCON";

    return NULL;
}

Status find_frame(char *frameID, MP3Tags *mp3, long *pos, int *size)
{
    int bytesRead = 0;

    while (bytesRead < mp3->totalTagSize)
    {
        long currentPos = ftell(mp3->fptr_mp3);

        char id[5];

        if (fread(id, 1, 4, mp3->fptr_mp3) != 4)
            return e_failure;

        id[4] = '\0';

        /* Padding reached */
        if (id[0] == '\0')
            return e_failure;

        unsigned char sizeBytes[4];

        if (fread(sizeBytes, 1, 4, mp3->fptr_mp3) != 4)
            return e_failure;

        int frameSize = (sizeBytes[0] << 24) | (sizeBytes[1] << 16) | (sizeBytes[2] << 8)  | sizeBytes[3];

        fseek(mp3->fptr_mp3, 2, SEEK_CUR);

        if (strcmp(id, frameID) == 0)
        {
            *pos = currentPos;
            *size = frameSize;

            return e_success;
        }

        fseek(mp3->fptr_mp3, frameSize, SEEK_CUR);
        bytesRead += 10 + frameSize;
    }

    return e_failure;
}

Status create_new_frame(char *frameID, char *newText, unsigned char **newFrame, int *newSize)
{
    int textLen = strlen(newText);

    int dataSize = 1 + 2 + (textLen * 2);

    *newSize = 10 + dataSize;

    *newFrame = malloc(*newSize);

    if (*newFrame == NULL)
        return e_failure;

    memcpy(*newFrame, frameID, 4);

    (*newFrame)[4] = (dataSize >> 24) & 0xFF;
    (*newFrame)[5] = (dataSize >> 16) & 0xFF;
    (*newFrame)[6] = (dataSize >> 8) & 0xFF;
    (*newFrame)[7] = dataSize & 0xFF;

    (*newFrame)[8] = 0x00;
    (*newFrame)[9] = 0x00;

    (*newFrame)[10] = 0x01;

    (*newFrame)[11] = 0xFF;
    (*newFrame)[12] = 0xFE;

    for (int i = 0; i < textLen; i++)
    {
        (*newFrame)[13 + (i * 2)] = newText[i];
        (*newFrame)[14 + (i * 2)] = 0x00;
    }

    return e_success;
}

Status mp3_editor(char *argv[], MP3Tags *mp3)
{
    if (read_id3_header(mp3) == e_failure)
        return e_failure;

    char *frameID = find_tag(argv[3]);

    if (frameID == NULL)
    {
        printf("Invalid modifier\n");
        return e_failure;
    }

    fseek(mp3->fptr_mp3, 10, SEEK_SET);

    long framePos;
    int oldFrameSize;

    if (find_frame(frameID, mp3, &framePos, &oldFrameSize) == e_failure)
    {
        printf("Frame not found\n");
        return e_failure;
    }

    unsigned char *newFrame;
    int newFrameSize;

    if (create_new_frame(frameID, argv[4], &newFrame, &newFrameSize) == e_failure)
    {
        return e_failure;
    }

    if (rewrite_file(mp3, framePos, oldFrameSize, newFrame, newFrameSize) == e_failure)
    {
        free(newFrame);
        return e_failure;
    }

    free(newFrame);

    mp3->fptr_mp3 = fopen(mp3->mp3_filename, "rb");

    if (mp3->fptr_mp3 == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    if (mp3_viewer(mp3) == e_failure)
    {
        fclose(mp3->fptr_mp3);
        return e_failure;
    }

    print_mp3_tags(mp3);

    if (strcmp(argv[3], "-t") == 0)
        printf("Title Modification - Done\n");

    else if (strcmp(argv[3], "-a") == 0)
        printf("Artist Modification - Done\n");

    else if (strcmp(argv[3], "-A") == 0)
        printf("Album Modification - Done\n");

    else if (strcmp(argv[3], "-y") == 0)
        printf("Year Modification - Done\n");

    else if (strcmp(argv[3], "-g") == 0)
        printf("Genre Modification - Done\n");

    fclose(mp3->fptr_mp3);

    return e_success;
}

Status rewrite_file(MP3Tags *mp3, long framePos, int oldFrameSize, unsigned char *newFrame, int newFrameSize)
{
    FILE *temp = fopen("temp.mp3", "wb");

    if (temp == NULL)
    {
        perror("temp.mp3");
        return e_failure;
    }

    int oldCompleteSize = 10 + oldFrameSize;

    int newTagSize = mp3->totalTagSize - oldCompleteSize + newFrameSize;

    unsigned char header[10];

    fseek(mp3->fptr_mp3, 0, SEEK_SET);

    if (fread(header, 1, 10, mp3->fptr_mp3) != 10)
    {
        fclose(temp);
        return e_failure;
    }

    header[6] = (newTagSize >> 21) & 0x7F;
    header[7] = (newTagSize >> 14) & 0x7F;
    header[8] = (newTagSize >> 7)  & 0x7F;
    header[9] = newTagSize & 0x7F;

    fwrite(header, 1, 10, temp);

    fseek(mp3->fptr_mp3, 10, SEEK_SET);

    long bytesToCopy = framePos - 10;

    for (long i = 0; i < bytesToCopy; i++)
    {
        int ch = fgetc(mp3->fptr_mp3);

        if (ch == EOF)
        {
            fclose(temp);
            return e_failure;
        }

        fputc(ch, temp);
    }

    fwrite(newFrame, 1, newFrameSize, temp);

    fseek(mp3->fptr_mp3, oldFrameSize + 10, SEEK_CUR);

    int ch;

    while ((ch = fgetc(mp3->fptr_mp3)) != EOF)
    {
        fputc(ch, temp);
    }

    fclose(temp);
    fclose(mp3->fptr_mp3);

    if (remove(mp3->mp3_filename) != 0)
    {
        remove("temp.mp3");
        return e_failure;
    }

    if (rename("temp.mp3", mp3->mp3_filename) != 0)
    {
        return e_failure;
    }

    mp3->totalTagSize = newTagSize;

    return e_success;
}

void print_mp3_tags(MP3Tags *mp3)
{
    printf("\nMp3 Tag Reader & Editor:\n");
    printf("------------------------\n");
    printf("Version ID : 2.%d\n", mp3->version_major);
    printf("Title : %s\n", mp3->title);
    printf("Album : %s\n", mp3->album);
    printf("Year : %s\n", mp3->year);
    // printf("Track : %s\n", mp3->track); // tag not found in both mp3 file
    printf("Genre : %s\n", mp3->genre);
    printf("Artist : %s\n", mp3->artist);
    // printf("Comment : %s\n", mp3->comment);// tag not found in both mp3 file
    printf("------------------------\n");
    printf("\n");
}