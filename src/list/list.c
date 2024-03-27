#include "list/list.h"

// typedef struct {
//     int is_actual; /* does the data member contains data */
//     int type; /* what is the type of the data member */
//     union {
//         int int_field;
//         float float_field;
//         char* string_field;
//     } data;
// } field_t;

// typedef struct Head_Node {
//     char* tuple_name;
//     size_t size;    // Without topic, only amount fields
//     Node* next_field;
//     struct Head_Node* next_tuple;
// } Head_Node;

// typedef struct Tuples_List {
//     size_t tuples_amount;
//     Head_Node* head;
// } Tuples_List;

void initiate_tuples_list(Tuples_List* tuples_list) {
    if(tuples_list) {
        tuples_list->tuples_amount = 0;
        tuples_list->head = (void*)0;   // Universal way to set pointer to NULL value
    }
    // TODO think if anything is needed
}

bool remove(Tuples_List* tuples_list, const size_t index) {
    return true;
}

//     char* tuple_name;
//     size_t size;    // Without topic, only amount fields
//     Node* next_field;
//     struct Head_Node* next_tuple;

bool add(Tuples_List* tuples_list, char* const tuple_name, const field_t* const tuple, const size_t size) {
    // Check if tuples list exists
    if(!tuples_list) {
        // TODO log error
        return false;
    }
    
    Head_Node* new_tuple;
    Head_Node* current_tuple = tuples_list->head;

    // Allocating new tuple
    new_tuple = malloc(sizeof(Head_Node));
    if(!new_tuple) {
        // TODO log error
        return false;
    }

    new_tuple->tuple_name = malloc(strlen(tuple_name) + 1);
    if(!new_tuple->tuple_name) {
        // TODO log error
        free(new_tuple);
        new_tuple = (void*)0;   // Nulling head
        return false;
    }
    new_tuple->tuple_name[strlen(tuple_name)] = '\0';   // Null terminated string

    new_tuple->fields = malloc(sizeof(field_t) * size);
    if(!new_tuple->fields) {
        // TODO log error
        free(new_tuple->tuple_name);
        free(new_tuple);
        new_tuple = (void*)0;   // Nulling head)
        return false;
    }
    memcpy(new_tuple->fields, tuple, sizeof(field_t) * size);
    
    new_tuple->size = size;
    new_tuple->next_tuple = (void*)0;

    tuples_list->tuples_amount++;

    // Adding new tuple to list
    if(!current_tuple) {  // No elements
        tuples_list->head = new_tuple;
    } else {    // Head exists
        while(current_tuple->next_tuple) {
            current_tuple = current_tuple->next_tuple;
        }
        current_tuple->next_tuple = new_tuple;
    }

    return true;
}

field_t* get(Tuples_List* tuples_list, const size_t index) {

}

int find(Tuples_List* tuples_list, char* const tuple_name, const field_t* const tuple, const size_t size) {

}
