#include "node/tuple_space.h"

// Fields types in message payload
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

// TODO add checks for null pointers at the beginning of functions

// TODO all int uses should be changed to uint32_t

// It take pointer to any value, user has to ensure it will be 4 byte long value
static inline uint32_t hton(void* value_t) {
    uint32_t host_value_t;
    memcpy(&host_value_t, value_t, NUMERIC_LENGTH);
    return htobe32(host_value_t);
}

static inline uint32_t ntoh(char value_buffer_t[static NUMERIC_LENGTH]) {
    uint32_t network_value_t;
    memcpy(&network_value_t, value_buffer_t, NUMERIC_LENGTH);
    return be32toh(network_value_t);
}

// returns size of data segment in bytes, or -1 on error
static int serialize_data_segment(const char* tuple_name, field_t fields[const], const int fields_amount) {
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
                    uint32_t network_value = hton(&(fields[i].data.int_field));   // Changing host byte order to network order (Big-Endian)
                    memcpy(values_buffer + 1, &network_value, NUMERIC_LENGTH);
                    strncpy(buffer + data_segment_size, values_buffer, 5);
                    data_segment_size += NUMERIC_LENGTH + 1;
                } else if(fields[i].is_actual == TS_NO) {
                    values_buffer[0] = (char)INT_NO;
                    strncpy(buffer + data_segment_size, values_buffer, 1);
                    data_segment_size += 1;
                } else {
                    printf("ERROR: %s:%s (%s:%d)\n","invalid field state", strerror(errno), __FILE__, __LINE__);
                    return -1;
                }
                break;
            }
            case TS_FLOAT: {
                if(fields[i].is_actual == TS_YES) {
                    values_buffer[0] = (char)FLOAT_YES;
                    uint32_t network_value = hton(&(fields[i].data.float_field));   // Float is same size as int so its bits are interpreted same as if it was int when sending to server
                    memcpy(values_buffer + 1, &network_value, NUMERIC_LENGTH);
                    strncpy(buffer + data_segment_size, values_buffer, 5);
                    data_segment_size += NUMERIC_LENGTH + 1;
                } else if(fields[i].is_actual == TS_NO) {
                    values_buffer[0] = (char)FLOAT_NO;
                    strncpy(buffer + data_segment_size, values_buffer, 1);
                    data_segment_size += 1;
                } else {
                    printf("ERROR: %s:%s (%s:%d)\n","invalid field state", strerror(errno), __FILE__, __LINE__);
                    return -1;
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
                    printf("ERROR: %s:%s (%s:%d)\n","invalid field state", strerror(errno), __FILE__, __LINE__);
                    return -1;
                }
                break;
            }
            default: {
                printf("ERROR: %s:%s (%s:%d)\n","invalid field type", strerror(errno), __FILE__, __LINE__);
                return -1;
            }
        }
    }
    values_buffer[0] = (char)END_MESSAGE;
    strncat(buffer + data_segment_size, values_buffer, 1);      // Adding End of Transmission Block character at the end
    data_segment_size += 1;

    return data_segment_size;   // returns total size of data segment stored in buffer
}


// returns size of data segment in bytes, or -1 on error
static int deserialize_data_segment(const char* tuple_name, field_t fields[const]) {
    if(strncmp(tuple_name, buffer, strlen(tuple_name)) != 0) {
        printf("ERROR: %s:%s (%s:%d)\n","returned tuple does not match name", strerror(errno), __FILE__, __LINE__);
        return -1;
    }

    size_t data_segment_size = 0;   // Checks current position in buffer
    data_segment_size += strlen(tuple_name) + 1;

    size_t index = 0;
    while(buffer[data_segment_size] != (char)END_MESSAGE) {
        switch(buffer[data_segment_size]) {
            case (char)INT_YES: {
                fields[index].is_actual = TS_YES;
                fields[index].type = TS_INT;
                data_segment_size++;
                uint32_t host_value = ntoh(&(buffer[data_segment_size]));
                memcpy(&fields[index].data.int_field, &(host_value), NUMERIC_LENGTH);
                data_segment_size += NUMERIC_LENGTH;
                break;
            }
            case (char)FLOAT_YES: {
                fields[index].is_actual = TS_YES;
                fields[index].type = TS_FLOAT;
                data_segment_size++;
                uint32_t host_value = ntoh(&(buffer[data_segment_size]));
                memcpy(&fields[index].data.float_field, &(host_value), NUMERIC_LENGTH);
                data_segment_size += NUMERIC_LENGTH;
                break;
            }
            case (char)STRING_YES: {
                if(fields[index].is_actual == TS_YES) {
                    free(fields[index].data.string_field);  // freeing memory alocated for old string
                } else {
                    fields[index].is_actual = TS_YES;
                }

                fields[index].type = TS_STRING;
                data_segment_size++;
                if(!init_string(&(fields[index].data.string_field), &(buffer[data_segment_size]), strlen(&(buffer[data_segment_size])) + 1)) {
                    return -1;
                }
                data_segment_size += strlen(fields[index].data.string_field) + 1;   // string length + '\0'
                break;
            }
            default: {
                printf("ERROR: %s:%s (%s:%d)\n","invalid field type", strerror(errno), __FILE__, __LINE__);
                return -1;      
            }
        }
        index++;
    }
    data_segment_size++;    // Last byte for END of TRANSMISSION BLOCK

    return data_segment_size;
}


/********************** API DEFINITION **********************/

// Allocates memory for string based on string_source of given length (length includes string and '\0' character)
bool init_string(char** const string_field, const char* const string_source, const size_t length) {
    *string_field = malloc(sizeof(char) * length);
    if(!(*string_field)) {
        printf("ERROR: %s:%s (%s:%d)\n","memory allocation failed", strerror(errno), __FILE__, __LINE__);
        return false;
    }

    strncpy(*string_field, string_source, length - 1);
    (*string_field)[length - 1] = (char)END_STRING;
    return true;
}

// Deallocates current string and allocates memory for new string
bool change_string(char** const string_field, const char* const string_source, const size_t length) {
    *string_field = realloc(*string_field, length);   // It leave current block or deallocate old and allocate new
    if(!(*string_field)) {
        printf("ERROR: %s:%s (%s:%d)\n","memory reallocation failed", strerror(errno), __FILE__, __LINE__);
        return false;
    }

    strncpy(*string_field, string_source, length);
    return true;
}

// Should always be called at the end of using tuples. It deallocates any strings
void destroy_tuple(field_t* tuples, const size_t fields_amount) {
    for(size_t i = 0;i < fields_amount; i++) {
        if(tuples[i].type == TS_STRING && tuples[i].is_actual == TS_YES) {      // Checking if field is string and actually contains string
            free(tuples[i].data.string_field);
        }
    }
}


bool ts_out(const char* tuple_name, field_t fields[const], const size_t fields_amount) {
    int data_segment_size = serialize_data_segment(tuple_name, fields, fields_amount);

    if(data_segment_size < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","data serialization failed", strerror(errno), __FILE__, __LINE__);
        return false;
    } else if(data_segment_size == 0) {
        printf("ERROR: %s:%s (%s:%d)\n","tried to send empty tuple", strerror(errno), __FILE__, __LINE__);
        return false;
    } else {
#ifndef NDEBUG
        printf("DATA SEGMENT SIZE:%d\n", data_segment_size);
        for(size_t i = 0;i < data_segment_size;i++) {
            printf("%d:%c\n", (unsigned char)(buffer[i]), buffer[i]);
        }
#endif // NDEBUG
    }

    send_message(OUT, fields_amount, buffer, data_segment_size);

    // TODO Wait for ACK
    if(!receive_message(buffer)) {
        return false;
    } 

    return true;
}


bool ts_inp(const char* tuple_name, field_t fields[], const size_t fields_amount) {
    // Creating char buffer with template
    int template_segment_size = serialize_data_segment(tuple_name, fields, fields_amount);
    
    if(template_segment_size < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","data serialization failed", strerror(errno), __FILE__, __LINE__);
        return false;
    } else if(template_segment_size == 0) {
        printf("ERROR: %s:%s (%s:%d)\n","tried to send empty template", strerror(errno), __FILE__, __LINE__);
        return false;
    } else {
#ifndef NDEBUG
        printf("TEMPLATE SEGMENT SIZE:%d\n", template_segment_size);
        for(size_t i = 0;i < template_segment_size;i++) {
            printf("%lu -> %d:%c", i, (unsigned char)(buffer[i]), buffer[i]);
            printf("\n");
        }
#endif // NDEBUG
    }

    send_message(INP, fields_amount, buffer, template_segment_size);
    // TODO wait for response tuple

    if(!receive_message(buffer)) {
        return false;
    } 

    // decode 
    int data_segment_size = deserialize_data_segment(tuple_name, fields);
    if(data_segment_size < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","data deserialization failed", strerror(errno), __FILE__, __LINE__);
        return false;
    } else if(data_segment_size == 0) {
        printf("ERROR: %s:%s (%s:%d)\n","received empty tuple", strerror(errno), __FILE__, __LINE__);
        return false;
    } else {
#ifndef NDEBUG
        printf("\nDATA SEGMENT SIZE:%d\n", data_segment_size);
        for(size_t i = 0;i < fields_amount;i++) {
            switch(fields[i].type) {
                case TS_INT: {
                    printf("%d\n", fields[i].data.int_field);
                    break;
                }
                case TS_FLOAT: {
                    printf("%g\n", fields[i].data.float_field);
                    break;
                }
                case TS_STRING: {
                    printf("%s\n", fields[i].data.string_field);
                    break;
                }
                default: {
                    
                }
            }
        }
#endif // NDEBUG
    }

    return true;
}


bool ts_rdp(const char* tuple_name, field_t fields[], const size_t fields_amount) {
    size_t template_segment_size = serialize_data_segment(tuple_name, fields, fields_amount);

    if(template_segment_size < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","data serialization failed", strerror(errno), __FILE__, __LINE__);
        return false;
    } else if(template_segment_size == 0) {
        printf("ERROR: %s:%s (%s:%d)\n","tried to send empty template", strerror(errno), __FILE__, __LINE__);
        return false;
    } else {
#ifndef NDEBUG
        printf("TEMPLATE SEGMENT SIZE:%lu\n", template_segment_size);
        for(size_t i = 0;i < template_segment_size;i++) {
            printf("%lu -> %d:%c", i, (unsigned char)(buffer[i]), buffer[i]);
            printf("\n");
        }
#endif // NDEBUG
    }

    send_message(RDP, fields_amount, buffer, template_segment_size);

    // TODO receive response tuple
    if(!receive_message(buffer)) {
        return false;
    } 

    // Get buffer of char from network
    size_t data_segment_size = deserialize_data_segment(tuple_name, fields);
    
    if(data_segment_size < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","data deserialization failed", strerror(errno), __FILE__, __LINE__);
        return false;
    } else if(data_segment_size == 0) {
        printf("ERROR: %s:%s (%s:%d)\n","received empty tuple", strerror(errno), __FILE__, __LINE__);
        return false;
    } else {
#ifndef NDEBUG
        printf("\nDATA SEGMENT SIZE:%lu\n", data_segment_size);
        for(size_t i = 0;i < fields_amount;i++) {
            switch(fields[i].type) {
                case TS_INT: {
                    printf("%d\n", fields[i].data.int_field);
                    break;
                }
                case TS_FLOAT: {
                    printf("%g\n", fields[i].data.float_field);
                    break;
                }
                case TS_STRING: {
                    printf("%s\n", fields[i].data.string_field);
                    break;
                }
                default: {
                    
                }
            }
        }
#endif // NDEBUG
    }

    return true;
}
