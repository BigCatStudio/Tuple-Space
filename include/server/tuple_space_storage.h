#ifndef TUPLE_SPACE_STORAGE_H
#define TUPLE_SPACE_STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define INT_YES 1
#define INT_NO 2
#define FLOAT_YES 3
#define FLOAT_NO 4
#define STRING_YES 5
#define STRING_NO 6

#define ALLOCATION_AMOUNT 10    // How many new lists of tuples should be added when memory max is reached

// TODO think how to store tuples in storage
// use serialize_data_segment and deserialize_data_segment from tuple_space.c
// or store as char* buffer
// TODO what data structure should be used for storing tuples and which will be most efficient 
// for finding matching tuple

// IDEA
// Use head node as starting point
// Head node will have pointers to heads of list of tuples with same fields_amount

typedef struct node {
    struct node* next;  // Next tuple of same fields amount
    char* tuple;
} node;

typedef struct {
    size_t fields_amount;
    node* head;
} tuple_list;

typedef struct {
    size_t capacity;        // How many lists can hold current tuple_space
    size_t lists_amount;    // Current amount of valid lists 
    tuple_list* lists;      // Contains list of nodes where each node is head of list with same fields amount
} tuple_space;


// Returns index if found and -1 if not found
int32_t find_list(tuple_space* ts, const size_t fields_amount) {
    for(size_t i = 0;i < ts->lists_amount;i++) {
        if(ts->lists[i].fields_amount == fields_amount) {
            return i;
        }
    }

    return -1;
}

// Adds tuple to tuple_space
bool add_element(tuple_space* ts, const char* restrict const tuple, const uint8_t fields_amount, const size_t size) {
    // Check if it is used for the first time
    if(ts->lists == NULL) {
        ts->lists = malloc(sizeof(tuple_list) * ALLOCATION_AMOUNT);     // Arbitrarily allocating memory for 10 lists of different fields amount to prevent to much allocating
        if(ts->lists == NULL) {
            // Memory allocation failed
            // TODO handle error
        }
        
        ts->capacity = ALLOCATION_AMOUNT;
        ts->lists_amount = 0;

        // Setting head of list to default values
        for(size_t i = 0;i < ALLOCATION_AMOUNT;i++) {
            ts->lists[i].fields_amount = 0;
            ts->lists[i].head = NULL;
        }
    }

    // Checking if given fields_amount has list
    int32_t index = find_list(ts, fields_amount);
    if(index == -1) {   // Creating new list
        if(ts->capacity == ts->lists_amount) {   // There is not memory for new list
            tuple_list* new_lists = realloc(ts->lists, ts->capacity + ALLOCATION_AMOUNT);   // Allocating new memory increased by arbitrarly value
            if(new_lists == NULL) {
                // Memory allocation failed
                // TODO handle error
            }

            ts->lists = new_lists;  // TODO check if all content has been correctly copied to new location
            ts->capacity += ALLOCATION_AMOUNT;
        }

        ts->lists[ts->lists_amount].head = malloc(sizeof(node));    // Allocating memory for list head
        if(ts->lists[ts->lists_amount].head == NULL) {
            // Memory allocation failed
            // TODO handle error
        }

        node* head = ts->lists[ts->lists_amount].head;
        head->next = NULL;
        head->tuple = malloc(size * sizeof(char)); // Allocating memory for tuple in char* format
        if(head->tuple == NULL) {
            // Memory allocation failed
            // TODO handle error
        }

        for(size_t i = 0;i < size;i++) {
            head->tuple[i] = tuple[i];  
            // memcpy(head->tuple, tuple, size); // TODO why memcpy does not work?
        }



        ts->lists[ts->lists_amount].fields_amount = fields_amount;
        ts->lists_amount++;

    } else {    // Adding tuple to existing list
        node* head = ts->lists[index].head;
        if(head == NULL) {
            // TODO handle some error
        }

        while(head->next != NULL) {
            head = head->next;  // TODO WHAT???
        }
    }

    return true;
}


bool remove_element() {

    return true;
}

bool find_element() {

    return true;
}

bool get_element() {

    return true;
}




void server_inp();

void server_out();

void server_rdp();


#endif  // TUPLE_SPACE_STORAGE_H