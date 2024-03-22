#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H


#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <features.h>
#include <endian.h> // for transforming values to big endian (network standard)

// #define NDEBUG   // disables all asserts and clauses with NDEBUG
#ifndef NDEBUG
#include <stdio.h>  // Only for debugging purpouses to print values
#endif // NDEBUG

#include <assert.h>

#define TS_YES 1
#define TS_NO 0
#define TS_INT 0
#define TS_FLOAT 1
#define TS_STRING 2
#define TS_SUCCESS 1
#define TS_FAILURE 0

typedef struct {
    int is_actual; /* does the data member contains data */
    int type; /* what is the type of the data member */
    union {
        int int_field;
        float float_field;
        char* string_field;
    } data;
} field_t; /* a new type corresponding to one field of a tuple */

/* API */
// TODO MOVE ALL COMMENTS ABOU WHAT FUNCTIONS DO FROM .C FILE TO THIS .H FILE

bool init_string(char** const string_field, const char* const string_source, const size_t length);
bool change_string(char** const string_field, const char* const string_source, const size_t length);
void destroy_tuple(field_t* tuples, const size_t fields_amount);

/* these functions return TS_SUCCESS or TS_FAILURE */
/* parameters: tuple name, other fields, no. of other fields */
/* they return status of operation: */
/*   0 - operation successful */
/*   1 - operation failed */
bool ts_out(const char* tuple_name, field_t fields[const], const int fields_amount);
bool ts_inp(const char* tuple_name, field_t fields[], const int fields_amount);
bool ts_rdp(const char* tuple_name, field_t fields[], const int fields_amount);


#endif  // TUPLE_SPACE_H