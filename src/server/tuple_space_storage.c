#include "server/tuple_space_storage.h"


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

    // for(size_t i = 0;i < size;i++) {
    //     // current->tuple[i] = tuple[i];  
    //      // TODO why memcpy does not work?
    // }
    memcpy(current->tuple, tuple, size);

    return current;
}

// Compares given C-string with tuples in tuple_space based on provided template
static bool compare_tuples(const char* const tuple, const char* const tuple_template, size_t* size) {
    size_t current_position = 0;
    size_t index = 0;

    // Comparing tuples names
    if(strcmp(tuple, tuple_template) != 0) {
        return false;
    }
    current_position += strlen(tuple) + 1;    // Length of tuple name + '\0'
    index = current_position;

    // TODO change below while to one macro which will take as arguments YES, NO and WIDTH values

    // Comparing tuples fields
    while(tuple_template[current_position] != END_MESSAGE) {
        switch(tuple[index]) {
            case INT_YES: {
                if(tuple_template[current_position] == INT_YES) {
                    if(memcmp(tuple + index + 1, tuple_template + current_position + 1, INT_WIDTH) != 0) {
                        return false;
                    }
                    index += INT_WIDTH + 1;
                    current_position += INT_WIDTH + 1;
                } else if(tuple_template[current_position] == INT_NO) {
                    index += INT_WIDTH + 1;
                    current_position += 1;
                } else {
                    return false;
                }
                break;
            }
            case FLOAT_YES: {
                if(tuple_template[current_position] == FLOAT_YES) {
                    if(memcmp(tuple + index + 1, tuple_template + current_position + 1, FLOAT_WIDTH) != 0) {
                        return false;
                    }
                    index += FLOAT_WIDTH + 1;
                    current_position += FLOAT_WIDTH + 1;
                } else if(tuple_template[current_position] == FLOAT_NO) {
                    index += FLOAT_WIDTH + 1;
                    current_position += 1;
                } else {
                    return false;
                }
                break;
            }
            case STRING_YES: {
                if(tuple_template[current_position] == STRING_YES) {
                    if(memcmp(tuple + index + 1, tuple_template + current_position + 1, strlen(tuple + index + 1)) != 0) {
                        return false;
                    }
                    index += strlen(tuple + index + 1) + 1 + 1;
                    current_position += strlen(tuple_template + current_position + 1) + 1 + 1;
                } else if(tuple_template[current_position] == STRING_NO) {
                    index += strlen(tuple + index + 1) + 1 + 1;
                    current_position += 1;
                } else {
                    return false;
                }
                break;
            }
            default: {
                printf("ERROR: %s:%s (%s:%d)\n","unknown type of data in template", strerror(errno), __FILE__, __LINE__);
                return false;
                break;
            }
        }
    }
    
    if(size != NULL) {
        *size = ++index;  // Size of found tuple for get_tuple function
    }
    
    return true;
}

static void check_empty_list(tuple_space* ts, size_t index) {
    if(ts->lists[index].head == NULL) { // Check if list is empty
        if(ts->lists_amount == 1) {    // only one list
            ts->lists[index].fields_amount = 0;
        } else if(index == ts->lists_amount - 1) { // last list
            ts->lists[index].fields_amount = 0;
        } else {    // middle list
            ts->lists[index] = ts->lists[ts->lists_amount - 1]; // middle list
            ts->lists[ts->lists_amount - 1].head = NULL;
            ts->lists[ts->lists_amount - 1].fields_amount = 0;
        }

        ts->lists_amount--;

        if(ts->lists_amount < ts->capacity - ALLOCATION_AMOUNT) {   // Freeing memory if there is too big capacity for lists amount
            tuple_list* new_lists = realloc(ts->lists, sizeof(tuple_list) * (ts->capacity - ALLOCATION_AMOUNT));   // Deallocating unused memory
            if(new_lists == NULL) {
                // Memory allocation failed
                // TODO handle error
                printf("ERROR: %s:%s (%s:%d)\n","Memory allocation failed", strerror(errno), __FILE__, __LINE__);
            }

            ts->lists = new_lists;  // TODO check if all content has been correctly copied to new location
            ts->capacity -= ALLOCATION_AMOUNT;
        }
    }
}

// Returns pointer to tuple in specified list
char* get_tuple(tuple_space* ts, char* tuple, const uint8_t fields_amount, size_t* size) {
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
            printf("ERROR: %s:%s (%s:%d)\n","Memory allocation failed", strerror(errno), __FILE__, __LINE__);
            return false;
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
                printf("ERROR: %s:%s (%s:%d)\n","Memory allocation failed", strerror(errno), __FILE__, __LINE__);
                return false;
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

    if(compare_tuples(current->tuple, tuple, NULL)) {   // Checking if head matches tuple
        ts->lists[index].head = current->next;  // NULL or next tuple
        free(current->tuple);
        free(current);
        
        check_empty_list(ts, index);

        return true;
    }

    while(current->next != NULL) {  // Checking rest of tuples
        if(compare_tuples(current->next->tuple, tuple, NULL)) {
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

void clear_tuple_space(tuple_space* ts) {
    if(ts == NULL) {
        return;
    }

    for(size_t i = 0;i < ts->lists_amount;i++) {
        node* current = ts->lists[i].head;
        node* delete_current;

        while(current != NULL) {
            delete_current = current;
            current = current->next;
            free(delete_current->tuple);
            free(delete_current);
        }
    }
    free(ts->lists);
}

// For debugging purpouses, displayes whole tuple_space
void display_tuple_space(tuple_space* ts) {
    printf("\nCapacity:%lu\n", ts->capacity);
    printf("Lists amount:%lu\n\n", ts->lists_amount);

    for(size_t i = 0;i < ts->lists_amount;i++) {
        printf("\tList[%lu] | ", i);
        printf("Fields amount:%lu\n", ts->lists[i].fields_amount);

        node* current = ts->lists[i].head;

        if(current == NULL) {
            printf("\tList is empty\n");
        } else {
            do {
                printf("\t");
                for(size_t i = 0;i < current->size;i++) {
                    // printf("%lu:%c\n", i, current->tuple[i]);
                    printf("%c", current->tuple[i]);
                }
                printf("\n");
                current = current->next;
            } while(current != NULL);
        }

        printf("\n");
    }
}