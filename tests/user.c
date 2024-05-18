#include "node/tuple_space.h"
#include <stdlib.h>

int main(int argc, char* argv[argc + 1]) {

    // out - adding tuple to space
    // inp - retrieving tuple from space
    // rdp - reading tuple from space

    // Has to be called always beforte any operations on tuples
    setup_networking();
    
    field_t my_tuple[3]; /* an array of fields (name not included) */
    field_t my_template[3];
    /* make a tuple */
    my_tuple[0].is_actual = TS_YES;
    my_tuple[0].type = TS_INT;
    my_tuple[0].data.int_field = 123456789;
    my_tuple[1].is_actual = TS_YES;
    my_tuple[1].type = TS_STRING;
    init_string(&(my_tuple[1].data.string_field), "Test message", strlen("Test message") + 1);
    my_tuple[2].is_actual = TS_YES;
    my_tuple[2].type = TS_FLOAT;
    my_tuple[2].data.float_field = 3.14;
    /* add a tuple to the tuple space */
    bool status = ts_inp("nice_constants", my_tuple, 3); /* ("nice_constants",128,3.14) */
    if(!status) {
        fprintf(stderr, "Error while doing inp operation\n");
    }

    destroy_tuple(my_tuple, 3); // Always call this function if you will not use tuple any more

    clean_networking();     // Always has to be called at the end of the program to properly clean network structures
    
    // /* make a template */
    // my_template[0].is_actual = TS_NO;
    // my_template[0].type = TS_FLOAT; /* need to specify the type */
    // my_template[1].is_actual = TS_NO;
    // my_template[1].type = TS_STRING; /* need to specify the type */
    // my_template[2].is_actual = TS_NO;
    // my_template[2].type = TS_INT; /* need to specify the type */
    // /* retrieve and remove a tuple with temperature */
    // /* some other process must have produced a tuple matching the template */
    // ts_inp("temperature", my_template, 3); /* ("temperature",?float) */
    // /* temp = my_template[0].data.float_field; */

    // /* transform a previously used tuple into a template */
    // my_tuple[0].is_actual = NO;
    // my_tuple[1].is_actual = NO;
    // ts_rdp("nice_constants", my_tuple, 2); /* ("nice_constants",?int,?float) */
    // nice_power = my_tuple[0].data.int_field; /* 128 – from the tuple space */
    // pi = my_tuple[1].data.float_field; /* 3.14 – from the tuple space */

    return EXIT_SUCCESS;
}