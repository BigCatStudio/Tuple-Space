# Tuple-Space
In IoT, communication between devices is crutial for system to work properly. This project implements middleware for managing network communication in IoT environment using tupples as messages. Tuple is simple message format where you can have many fields of 3 types: integer number, floating point number and text. Also it is important to specify first element of tuple, which is its name.

Tuple has structure:
| name | field_type | field_value | field_type | field value | ... |


User can create tuple and send it via provided simple interface which usage is shown in files in tests/ folder.

Running sending node:
gcc -Wall -o user src/node/node_networking.c src/node/tuple_space.c tests/user.c -I ./include

Running receiving node:
gcc -Wall -o read_test src/node/node_networking.c src/node/tuple_space.c tests/read_test.c -I ./include

Running tuple space server:
gcc -Wall -o server src/server/tuple_space_storage.c src/server/server_networking.c -I ./include

