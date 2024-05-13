#include "server/tuple_space_storage.h"

int main() {
    tuple_space ts;
    ts.lists = NULL;
    
    add_tuple(&ts, "To jest tuple 1", 1, 16);
    add_tuple(&ts, "To jest tuple 2", 2, 16);
    add_tuple(&ts, "To jest tuple 3", 3, 16);
    add_tuple(&ts, "To jest tuple 4", 1, 16);
    add_tuple(&ts, "To jest tuple 5", 2, 16);
    add_tuple(&ts, "To jest tuple 6", 4, 16);
    add_tuple(&ts, "To jest tuple 7", 4, 16);
    add_tuple(&ts, "To jest tuple 8", 5, 16);
    add_tuple(&ts, "To jest tuple 9", 2, 16);
    add_tuple(&ts, "To jest tuple 10", 2, 17);
    add_tuple(&ts, "To jest tuple 11", 6, 17);
    add_tuple(&ts, "To jest tuple 12", 7, 17);
    add_tuple(&ts, "To jest tuple 13", 8, 17);
    add_tuple(&ts, "To jest tuple 14", 9, 17);
    add_tuple(&ts, "To jest tuple 15", 10, 17);
    add_tuple(&ts, "To jest tuple 16", 11, 17);
    add_tuple(&ts, "To jest tuple 17", 12, 17);

    display_tuple_space(&ts);

    remove_tuple(&ts, "To jest tuple 8", 5, 16);
    remove_tuple(&ts, "To jest tuple 1", 1, 16);
    remove_tuple(&ts, "To jest tuple 10", 2, 17);
    remove_tuple(&ts, "To jest tuple 13", 8, 17);
    remove_tuple(&ts, "To jest tuple 14", 9, 17);

    display_tuple_space(&ts);


    return 0;
}
