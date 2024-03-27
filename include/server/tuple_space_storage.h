#ifndef TUPLE_SPACE_STORAGE_H
#define TUPLE_SPACE_STORAGE_H


#include <stdbool.h>

typedef struct {
    int is_actual; /* does the data member contains data */
    int type; /* what is the type of the data member */
    union {
        int int_field;
        float float_field;
        char* string_field;
    } data;
} field_t; /* a new type corresponding to one field of a tuple */

bool server_out(char* const data, size_t amount);
bool server_inp(char* const data, char* buffer, size_t amount);
bool server_rdp(char* const data, char* buffer, size_t amount);

#endif  // TUPLE_SPACE_STORAGE_H