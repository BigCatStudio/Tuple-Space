#ifndef TUPLE_SPACE_STORAGE_H
#define TUPLE_SPACE_STORAGE_H


#define INT_YES 1
#define INT_NO 2
#define FLOAT_YES 3
#define FLOAT_NO 4
#define STRING_YES 5
#define STRING_NO 6

// TODO think how to store tuples in storage
// use serialize_data_segment and deserialize_data_segment from tuple_space.c
// or store as char* buffer
// TODO what data structure should be used for storing tuples and which will be most efficient 
// for finding matching tuple

// IDEA
// Use head node as starting point
// Head node will have pointers to heads of list of tuples with same fields_amount

typedef struct {
    node* next  // Next tuple of same fields amount
    // TODO char* for whole tuple or deserialized data -> which faster?
} node;

typedef struct {
    size_t fields_amount;
    node* next;
} tuples_list;

typedef struct {
    tuples_list** list;    // Contains list of nodes where each node is head of list with same fields amount
} tuples_storage;





void server_inp();

void server_out();

void server_rdp();


#endif  // TUPLE_SPACE_STORAGE_H