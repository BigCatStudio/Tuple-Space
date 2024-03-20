#include "node/tuple_space.h"
#include <stdlib.h>

int main(int argc, char* argv[argc + 1]) {

    // out - adding tuple to space
    // inp - retrieving tuple from space
    // rdp - reading tuple from space

    int nice_power;
    double temp, pi;
    field_t my_tuple[2]; /* an array of fields (name not included) */
    field_t my_template[1];
    /* make a tuple */
    my_tuple[0].is_actual = TS_YES;
    my_tuple[0].type = TS_INT;
    my_tuple[0].data.int_field = 128;
    my_tuple[1].is_actual = TS_YES;
    my_tuple[1].type = TS_FLOAT;
    my_tuple[1].data.float_field = 3.14;
    /* add a tuple to the tuple space */
    ts_out("nice_constants", my_tuple, 2); /* ("nice_constants",128,3.14) */

    
    // /* make a template */
    // my_template[0].is_actual = NO;
    // my_template[0].type = FLOAT_FIELD; /* need to specify the type */
    // /* retrieve and remove a tuple with temperature */
    // /* some other process must have produced a tuple matching the template */
    // ts_inp("temperature", my_template, 1); /* ("temperature",?float) */
    // temp = my_template[0].data.float_field;

    // /* transform a previously used tuple into a template */
    // my_tuple[0].is_actual = NO;
    // my_tuple[1].is_actual = NO;
    // ts_rdp("nice_constants", my_tuple, 2); /* ("nice_constants",?int,?float) */
    // nice_power = my_tuple[0].data.int_field; /* 128 – from the tuple space */
    // pi = my_tuple[1].data.float_field; /* 3.14 – from the tuple space */

    return EXIT_SUCCESS;
}