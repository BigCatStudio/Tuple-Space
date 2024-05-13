#include "server/tuple_space_storage.h"

int main() {
    tuple_space ts;
    ts.lists = NULL;
    
    // add_tuple(&ts, "To jest tuple 1", 2, 16);
    // add_tuple(&ts, "To jest tuple 2", 5, 16);
    // add_tuple(&ts, "To jest tuple 3", 1, 16);
    // add_tuple(&ts, "To jest tuple 4", 2, 16);
    // add_tuple(&ts, "To jest tuple 5", 5, 16);
    // add_tuple(&ts, "To jest tuple 6", 2, 16);
    // add_tuple(&ts, "To jest tuple 7", 1, 16);
    // add_tuple(&ts, "To jest tuple 8", 2, 16);
    // add_tuple(&ts, "To jest tuple 9", 2, 16);
    // add_tuple(&ts, "To jest tuple 10", 2, 17);

    // display_tuple_space(&ts);

    // remove_tuple(&ts, "To jest tuple 8", 2, 16);
    // remove_tuple(&ts, "To jest tuple 1", 2, 16);
    // remove_tuple(&ts, "To jest tuple 10", 2, 17);

    // display_tuple_space(&ts);

    // printf("Tuple:%s\n", get_tuple(&ts, "To jest tuple 2", 5, 16));


    char string[10] = "123456789";
    string[1] = '\0';
    add_tuple(&ts, string, 1, 10);
    // add_tuple(&ts, "To jest tuple 2", 2, 16);
    // add_tuple(&ts, "To jest tuple 3", 3, 16);
    // add_tuple(&ts, "To jest tuple 4", 4, 16);
    // add_tuple(&ts, "To jest tuple 5", 5, 16);
    // add_tuple(&ts, "To jest tuple 6", 6, 16);
    // add_tuple(&ts, "To jest tuple 7", 7, 16);
    // add_tuple(&ts, "To jest tuple 8", 8, 16);
    // add_tuple(&ts, "To jest tuple 9", 9, 16);
    // add_tuple(&ts, "To jest tuple 10", 10, 17);
    // add_tuple(&ts, "To jest tuple 11", 11, 17);
    // add_tuple(&ts, "To jest tuple 12", 12, 17);
    // add_tuple(&ts, "To jest tuple 13", 13, 17);
    // add_tuple(&ts, "To jest tuple 14", 14, 17);


    display_tuple_space(&ts);

    return 0;
}
