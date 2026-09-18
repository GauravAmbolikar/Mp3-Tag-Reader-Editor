#ifndef TYPES_H
#define TYPES_H

/* Status will be used in fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;

typedef enum
{
    e_reader,
    e_editor,
    e_format,
    e_unsupported
} OperationType;

#endif
