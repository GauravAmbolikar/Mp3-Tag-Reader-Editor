#include"types.h"
#include"mp3_reader.h"
#include<stdio.h>
#include<string.h>


int main(int argc, char *argv[])
{
    MP3Tags V1;
    int res = check_operation_type(argc,argv);

    if(res == e_format)
    {
        printf("Help menu for Mp3 Tag Reader and Editor:\n"
       "For viewing the tags-    ./mp3_tag_reader -v <filename.mp3>\n"
       "For editing the tags-    ./mp3_tag_reader -e <filename.mp3> <modifier>\n"
       "Modifier    Function\n"
       "-t          Modify Title Tag\n"
       "-T          Modify Track Tag\n"
       "-a          Modify Artist Tag\n"
       "-A          Modify Album Tag\n"
       "-y          Modify Year Tag\n"
       "-c          Modify Comment Tag\n"
       "-g          Modify Genre Tag\n");
    }
    else if(res == e_reader)
    {
        if(read_and_validate_args(argv, &V1) == e_success)
        {   
            if(open_files(&V1) == e_success)
            {
                if(mp3_viewer(&V1) == e_success)
                {
                    print_mp3_tags(&V1);
                }
                else
                {
                    printf("Failed to view .mp3 file\n");
                }
            }
            else
            {
                printf("Failed to open .mp3 file\n");
                return 0;
            }
        }
        else
        {
            printf("Error: Please Enter File in .mp3 Format\n");
            return 0;
        }
    }
    else if(res == e_editor)
    {
        //do_editing
    }
    else
    {
        printf("ERROR: Incorrect format of command line arguments.\n"
                "Use \"./mp3_tag_reader -h\" for help\n");
    }

    return 0;
}

OperationType check_operation_type(int argc, char *argv[])
{
    if (argc < 2)
        return e_unsupported;

    if (strcmp(argv[1], "-v") == 0)
    {
        if (argc == 3)
            return e_reader;
    }
    else if (strcmp(argv[1], "-e") == 0)
    {
        if (argc == 5)
            return e_editor;
    }
    else if(strcmp(argv[1], "-h") == 0)
    {
        if(argc == 2)
            return e_format;
    }

    return e_unsupported;
}