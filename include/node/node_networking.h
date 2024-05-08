#ifndef NODE_NETWORKING_H
#define NODE_NETWORKING_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> // To get local time
#include <errno.h>
#include <stdbool.h>

#include <netdb.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h> // Definition of INET_ADDRSTRLEN (IP address)
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>   // To add prototype of gettimeofday(...) function

#define CLIENT_ADDRESS "127.0.0.1"
#define CLIENT_PORT "9999"     // Server port that it listens

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT "9000"

#define PACKET_BUFFER_LENGTH 1024   // Max size of data in single packet
#define FIELDS_MAX 63



// Header types
#define INP 1
#define RDP 2
#define OUT 3
#define HELLO 4
#define ACK 5

#define HEADER_LENGTH 2


typedef struct {
    uint8_t operation_type : 3;
    uint8_t device_id : 7;
    uint8_t fields_amount : 6;
    char tuple[PACKET_BUFFER_LENGTH];
} ALP_message;

typedef struct {    // Contains server networing details: socket, addresses etc
    struct addrinfo node_config;  // h
    struct addrinfo *node_net_config; // r
    struct sockaddr_in server_net_info; // c
    int socket_info, pos;   // s, pos
    socklen_t server_info_size;      // c_len
    struct addrinfo server_config; 
    struct addrinfo *server_net_config;   // client_hello, res
} networking_stack;

bool setup_networking();

void clean_networking();

void send_message(uint8_t operation_type, uint8_t fields_amount, char* tuple, uint32_t size);


#endif  // NODE_NETWORKING_H