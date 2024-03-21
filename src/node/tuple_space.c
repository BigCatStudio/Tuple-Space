#include "node/tuple_space.h"


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

#define BUFFER_LENGTH 1024
char buffer[BUFFER_LENGTH];

// TODO add translation to big endian for transmitting through network
static void code_int(char numeric_buffer[static NUMERIC_LENGTH], const int value) {
    memcpy(numeric_buffer, &value, NUMERIC_LENGTH);
}

static void code_float(char numeric_buffer[static NUMERIC_LENGTH], const float value) {
    memcpy(numeric_buffer, &value, NUMERIC_LENGTH);     // Assuming that nodes and server use IEEE-754 for coding float
}

/* returns size of data segment in bytes */
static size_t generate_data_segment(const char* tuple_name, field_t fields[const], const int fields_amount) {
    memset(buffer, '\0', BUFFER_LENGTH);    // Clearing buffer
    strncpy(buffer, tuple_name, strlen(tuple_name));

    size_t data_segment_size = 0;
    data_segment_size += strlen(tuple_name) + 1;    // Length of tuple name + '\0' character

    char values_buffer[NUMERIC_LENGTH + 1];    // Length of int/float + byte indicating if its concrete value
    for(size_t i = 0;i < fields_amount;i++) {
        switch(fields[i].type) {
            case TS_INT: {
                if(fields[i].is_actual == TS_YES) {
                    values_buffer[0] = (char)INT_YES;
                    code_int(values_buffer + 1, fields[i].data.int_field);
                    strncpy(buffer + data_segment_size, values_buffer, 5);
                    data_segment_size += NUMERIC_LENGTH + 1;
                } else if(fields[i].is_actual == TS_NO) {
                    values_buffer[0] = (char)INT_NO;
                    strncpy(buffer + data_segment_size, values_buffer, 1);
                    data_segment_size += 1;
                } else {
                    // TODO invalid data return error
                }
                break;
            }
            case TS_FLOAT: {
                if(fields[i].is_actual == TS_YES) {
                    values_buffer[0] = (char)FLOAT_YES;
                    code_float(values_buffer + 1, fields[i].data.float_field);
                    strncpy(buffer + data_segment_size, values_buffer, 5);
                    data_segment_size += NUMERIC_LENGTH + 1;
                } else if(fields[i].is_actual == TS_NO) {
                    values_buffer[0] = (char)FLOAT_NO;
                    strncpy(buffer + data_segment_size, values_buffer, 1);
                    data_segment_size += 1;
                } else {
                    // TODO invalid data return error
                }
                break;
            }
            case TS_STRING: {
                if(fields[i].is_actual == TS_YES) {
                    values_buffer[0] = (char)STRING_YES;
                    strncpy(buffer + data_segment_size, values_buffer, 1);
                    data_segment_size += 1;
                    strncpy(buffer + data_segment_size, fields[i].data.string_field, strlen(fields[i].data.string_field) + 1);   // Adding string to buffer + '\0' character
                    data_segment_size += strlen(fields[i].data.string_field) + 1;   // length of string + '\0' character
                } else if(fields[i].is_actual == TS_NO) {
                    values_buffer[0] = (char)STRING_NO;
                    strncpy(buffer + data_segment_size, values_buffer, 1);
                    data_segment_size += 1;
                } else {
                    // TODO invalid data return error
                }
                break;
            }
            default: {
                // TODO invalid data return error
            }
        }
    }
    values_buffer[0] = (char)END_MESSAGE;
    strncat(buffer + data_segment_size, values_buffer, 1);      // Adding End of Transmission Block character at the end
    data_segment_size += 1;

    return data_segment_size;   // returns total size of data segment stored in buffer
}

bool ts_out(const char* tuple_name, field_t fields[const], const int fields_amount) {

    // iterate over elements and create char[]
    size_t data_segment_size = generate_data_segment(tuple_name, fields, fields_amount);
    if(data_segment_size < 0) {
        // TODO user provided some invalid data when creating tuple fields
    } else if(data_segment_size == 0) {
        // TODO add error when user tries to send empty tuple
    } else {
        // TODO everything went correctly
#ifndef NDEBUG
        printf("DATA SEGMENT SIZE:%lu\n", data_segment_size);
        for(size_t i = 0;i < data_segment_size;i++) {
            printf("%d:%c", (unsigned char)(buffer[i]), buffer[i]);
            printf("\n");
        }
#endif // NDEBUG
    }

    // send via network to server as char[]
    // network module will add ALP, but provide necessary informations for ALP

    return true;
}


bool ts_inp(const char* tuple_name, field_t fields[], const int fields_amount) {
    return true;
}


bool ts_rdp(const char* tuple_name, field_t fields[], const int fields_amount) {
    return true;
}