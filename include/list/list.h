#ifndef LIST_H
#define LIST_H

/* Contains list implementation specific for storing tuple space */

#include <uchar.h>  // For size_t
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define TS_YES 1
#define TS_NO 0
#define TS_INT 0
#define TS_FLOAT 1
#define TS_STRING 2
#define TS_SUCCESS 1
#define TS_FAILURE 0

#define INT_YES 1
#define INT_NO 2
#define FLOAT_YES 3
#define FLOAT_NO 4
#define STRING_YES 5
#define STRING_NO 6

#define END_STRING 0
#define END_MESSAGE 23  // Number of ASCII character for End of Transmission Block 

#define NUMERIC_LENGTH 4    // Length of int/float in bytes
#define BYTE_LENGTH 8

// TODO check if fields in structures are in right order so alignment will not increase size of structure

// Used in tuple_space_storage to decode incoming tuple from network
typedef struct {
    int is_actual; /* does the data member contains data */
    int type; /* what is the type of the data member */
    union {
        int int_field;
        float float_field;
        char* string_field;
    } data;
} field_t;

typedef struct Head_Node {
    char* tuple_name;
    size_t size;    // Without topic, only amount fields
    field_t* fields;
    struct Head_Node* next_tuple;
} Head_Node;

typedef struct Tuples_List {
    size_t tuples_amount;
    Head_Node* head;
} Tuples_List;

void initiate_tuples_list(Tuples_List* tuples_list);    // Using it on already modified and initiated tuples_list is causing memory leaks
bool remove(Tuples_List* tuples_list, const size_t index);    // Removes tuple of given index from list, returns 1 if suucesfull, 0 otherwise
bool add(Tuples_List* tuples_list, char* const tuple_name, const field_t* const tuple, const size_t size);   // Adds tuple to list, returns 1 if suucesfull, 0 otherwise
field_t* get(Tuples_List* tuples_list, const size_t index);   // Returns tuple of given index in field_t structures
int find(Tuples_List* tuples_list, char* const tuple_name, const field_t* const tuple, const size_t size); // Returns index of tuple that matches or -1 if there is not any matching tuple

#endif // LIST_H