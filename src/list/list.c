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

    if((index >= tuples_list->tuples_amount) || (index < 0)) {
        // TODO log error
        return false;
    }

    Head_Node* current_tuple = tuples_list->head;
    if(index == 0) {    // Deleting first element in list
        free(tuples_list->head->tuple_name);
        free(tuples_list->head->fields);
        if(tuples_list->head->next_tuple) { // There are many elements in list
            tuples_list->head = tuples_list->head->next_tuple;
        } else {        // There is only head in list
            tuples_list->head = (void*)0;
        }
        free(current_tuple);
    } else {
        for(size_t i = 0;i < index - 1;i++) {
            current_tuple = current_tuple->next_tuple;
        }

        Head_Node* tuple_to_remove = current_tuple->next_tuple;

        if(tuple_to_remove->next_tuple) {   // Tuple for removal is bewteen tuples
            current_tuple->next_tuple = tuple_to_remove->next_tuple;
        } else {    // Tuple for removal is last
            current_tuple->next_tuple = (void*)0;
        }
        free(tuple_to_remove->tuple_name);
        free(tuple_to_remove->fields);
        free(tuple_to_remove);
    }

    tuples_list->tuples_amount--;

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

// int main(int argc, char* argv[argc + 1]) {

//     return 0;
// }