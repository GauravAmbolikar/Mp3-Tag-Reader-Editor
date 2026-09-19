#ifndef TYPES_H
#define TYPES_H

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
