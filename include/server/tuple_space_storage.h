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


// Returns index if found and -1 if not found
static int32_t find_list(tuple_space* ts, const size_t fields_amount) {
    for(size_t i = 0;i < ts->lists_amount;i++) {
        if(ts->lists[i].fields_amount == fields_amount) {
            return i;
        }
    }

    return -1;
}

// TODO resize method for chaning size of tuple_space

// Allocates memory for new tuple and its contents
static node* allocate_new_tuple(const char* const tuple, const size_t size) {
    node* current = malloc(sizeof(node));    // Allocating memory for list head
    if(current == NULL) {
        // Memory allocation failed
        // TODO handle error
        return NULL;
    }

    current->next = NULL;
    current->size = size;
    current->tuple = malloc(size * sizeof(char)); // Allocating memory for tuple in char* format
    if(current->tuple == NULL) {
        // Memory allocation failed
        // TODO handle error
        return NULL;
    }

    for(size_t i = 0;i < size;i++) {
        // current->tuple[i] = tuple[i];  
        memcpy(current->tuple, tuple, size); // TODO why memcpy does not work?
    }

    return current;
}

// Compares given C-string with tuples in tuple_space based on provided template
static bool compare_tuples(const char* const tuple_1, const char* const tuple_2, size_t size) {
    if(strncmp(tuple_1, tuple_2, size) == 0) {
        return true;
    }
    return false;



}

// Returns pointer to tuple in specified list
char* get_tuple(tuple_space* ts, char* tuple, const uint8_t fields_amount, size_t size) {
    size_t index = find_list(ts, fields_amount);
    if(index == -1) {
        return NULL;
    }

    node* current = ts->lists[index].head;
    if(current == NULL) {
        return NULL;
    }

    do {
        if(compare_tuples(current->tuple, tuple, size)) {
            return current->tuple;
        }
        current = current->next;
    } while(current != NULL);

    return NULL;
}


// Adds tuple to tuple_space
bool add_tuple(tuple_space* ts, const char* restrict const tuple, const uint8_t fields_amount, const size_t size) {
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
            tuple_list* new_lists = realloc(ts->lists, sizeof(tuple_list) * (ts->capacity + ALLOCATION_AMOUNT));   // Allocating new memory increased by arbitrarly value
            if(new_lists == NULL) {
                // Memory allocation failed
                // TODO handle error
            }

            ts->lists = new_lists;  // TODO check if all content has been correctly copied to new location
            ts->capacity += ALLOCATION_AMOUNT;
        }

        ts->lists[ts->lists_amount].head = allocate_new_tuple(tuple, size);
        ts->lists[ts->lists_amount].fields_amount = fields_amount;
        ts->lists_amount++;

    } else {    // Adding tuple to existing list
        node* current = ts->lists[index].head; // First tuple in list

        if(current == NULL) {   // There are not elements in list
            ts->lists[index].head = allocate_new_tuple(tuple, size);
        } else {    // Head exists in list
            while(current->next != NULL) {
                current = current->next;
            }

            current->next = allocate_new_tuple(tuple, size);
        }
    }

    return true;
}

// Returns true if tuple was removed and false if tuple was not found
bool remove_tuple(tuple_space* ts, const char* const tuple, const uint8_t fields_amount, const size_t size) {
    size_t index = find_list(ts, fields_amount);

    if(index == -1) {
        return false;
    }

    node* current = ts->lists[index].head;

    if(current == NULL) {
        return false;
    }

    if(compare_tuples(current->tuple, tuple, size)) {   // Checking if head matches tuple
        ts->lists[index].head = current->next;  // NULL or next tuple
        free(current->tuple);
        free(current);
        return true;
    }

    // TODO check if there are any tuples in list, if not remove list from tuple_space

    while(current->next != NULL) {  // Checking rest of tuples if they exist
        if(compare_tuples(current->next->tuple, tuple, size)) {
            node* next = current->next->next;   // NULL or next tuple
            free(current->next->tuple);
            free(current->next);
            current->next = next;
            return true;
        }
        current = current->next;
    }

    return false;
}

// For debugging purpouses, displayes whole tuple_space
void display_tuple_space(tuple_space* ts) {
    printf("\nCapacity:%lu\n", ts->capacity);
    printf("Lists amount:%lu\n\n", ts->lists_amount);

    for(size_t i = 0;i < ts->lists_amount;i++) {
        printf("\tList[%lu]\n", i);
        printf("\tFields amount:%lu\n", ts->lists[i].fields_amount);

        node* current = ts->lists[i].head;

        if(current == NULL) {
            printf("\tList is empty\n");
        } else {
            do {
                printf("\tSize:%lu\n", current->size);
                printf("\t");
                for(size_t i = 0;i < current->size;i++) {
                    // printf("%lu:%c\n", i, current->tuple[i]);
                    printf("%c", current->tuple[i]);
                }

                current = current->next;
            } while(current != NULL);
        }

        printf("\n");
    }
}


#endif  // TUPLE_SPACE_STORAGE_H