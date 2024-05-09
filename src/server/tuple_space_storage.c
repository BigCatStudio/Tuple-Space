#include "server/tuple_space_storage.h"

int main() {
    
    return 0;
}


// typedef struct {
//     struct node* next;  // Next tuple of same fields amount
//     // TODO char* for whole tuple or deserialized data -> which faster?
// } node;

// typedef struct {
//     size_t fields_amount;
//     node* next;
// } tuples_list;

// typedef struct {
//     tuples_list** list;    // Contains list of nodes where each node is head of list with same fields amount
// } tuples_storage;