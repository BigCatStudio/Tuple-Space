#include "server/tuple_space_storage.h"


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

#include <uchar.h>  // For size_t


typedef struct {
    int type;
    union {
        int int_field;
        float float_field;
        char* string_field;
    } data;
    struct Node* next;
} Node;

typedef struct {
    char* topic;
    size_t size;    // Without topic, only amount fields
    struct Node* next;
} Head_Node;


bool server_out(char* const data, size_t amount) {
    // Save tuple in storage
    Node node;
    return true;
}

bool server_inp(char* const data, char* buffer, size_t amount) {

    return true;
}

bool server_rdp(char* const data, char* buffer, size_t amount) {

    return true;
}