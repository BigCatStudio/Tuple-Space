#include "server/tuple_space_storage.h"

int main() {
    tuple_space ts;
    ts.lists = NULL;
    
    add_tuple(&ts, "To jest tuple 1", 2, 16);
    add_tuple(&ts, "To jest tuple 2", 5, 16);
    add_tuple(&ts, "To jest tuple 3", 1, 16);
    add_tuple(&ts, "To jest tuple 4", 2, 16);
    add_tuple(&ts, "To jest tuple 5", 5, 16);
    add_tuple(&ts, "To jest tuple 6", 2, 16);
    add_tuple(&ts, "To jest tuple 7", 1, 16);
    add_tuple(&ts, "To jest tuple 8", 2, 16);
    add_tuple(&ts, "To jest tuple 9", 2, 16);
    add_tuple(&ts, "To jest tuple 10", 2, 17);

    display_tuple_space(&ts);

    remove_tuple(&ts, "To jest tuple 8", 2, 16);
    remove_tuple(&ts, "To jest tuple 1", 2, 16);
    remove_tuple(&ts, "To jest tuple 10", 2, 17);

    display_tuple_space(&ts);

    return 0;
}
