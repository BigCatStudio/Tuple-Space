#ifndef TUPLE_SPACE_STORAGE_H
#define TUPLE_SPACE_STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// Headers in message payload
#define INT_YES 1
#define INT_NO 2
#define FLOAT_YES 3
#define FLOAT_NO 4
#define STRING_YES 5
#define STRING_NO 6
#define END_STRING 0
#define END_MESSAGE 23  // Number of ASCII character for End of Transmission Block 

#define INT_WIDTH 4
#define FLOAT_WIDTH 4

#define ALLOCATION_AMOUNT 10    // How many new lists of tuples should be added when memory max is reached

// TODO think how to store tuples in storage
// use serialize_data_segment and deserialize_data_segment from tuple_space.c
// or store as char* buffer
// TODO what data structure should be used for storing tuples and which will be most efficient 
// for finding matching tuple

// IDEA
// Use head node as starting point
// Head node will have pointers to heads of list of tuples with same fields_amount

// Contains pointer *next to next tuple in list and tuple
typedef struct node {
    struct node* next;  // Next tuple of same fields amount
    char* tuple;
    size_t size;
} node;

// Contains pointer *head to the beginning of list and amount of fields for list
typedef struct {
    size_t fields_amount;
    node* head;
} tuple_list;

// Contains pointers to lists, and capacity of how many lists it can currently contain in memory
typedef struct {
    size_t capacity;        // How many lists can hold current tuple_space
    size_t lists_amount;    // Current amount of valid lists 
    tuple_list* lists;      // Contains list of nodes where each node is head of list with same fields amount
} tuple_space;


// Dynamic allocation:
// tuple_list* lists
// node* head, node* next
// char* tuple


// TODO add checking for types returned by all functions defined here




// Returns pointer to tuple in specified list
char* get_tuple(tuple_space* ts, char* tuple, const uint8_t fields_amount, size_t size);

// Adds tuple to tuple_space
bool add_tuple(tuple_space* ts, const char* restrict const tuple, const uint8_t fields_amount, const size_t size);

// Returns true if tuple was removed and false if tuple was not found
bool remove_tuple(tuple_space* ts, const char* const tuple, const uint8_t fields_amount, const size_t size);

// Frees allocated memory for tuple space
void clear_tuple_space(tuple_space* ts);

// For debugging purpouses, displayes whole tuple_space
void display_tuple_space(tuple_space* ts);


#endif  // TUPLE_SPACE_STORAGE_H