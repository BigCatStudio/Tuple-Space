#ifndef SERVER_NETWORKING_H
#define SERVER_NETWORKING_H


#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h> // Definition of INET_ADDRSTRLEN (IP address)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h> // To get local time
#include <sys/time.h>   // To add prototype of gettimeofday(...) function
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>


#define SERVER_PORT "9000"     // Server port that it listens
#define PACKET_BUFFER_LENGTH 1024
#define HEADER_LENGTH 2
#define TIME_SIZE 84
#define FIELDS_MAX 63

// Header types
#define INP 1
#define RDP 2
#define OUT 3
#define HELLO 4
#define ACK 5


typedef struct {    // Contains server networing details: socket, addresses etc
    struct addrinfo h;
    struct addrinfo* r;
    struct sockaddr_in client_info;
    int socket;
    socklen_t c_len;
} networking_stack;

typedef struct {
    uint8_t operation_type : 3;
    uint8_t device_id : 7;
    uint8_t fields_amount : 6;
    char tuple[PACKET_BUFFER_LENGTH - HEADER_LENGTH];
} ALP_message;




#endif  // SERVER_NETWORKING_H