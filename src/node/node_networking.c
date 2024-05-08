// #include "node/node_networking.h"


// Headers files
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h> // Definition of INET_ADDRSTRLEN (IP address)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> // To get local time
#include <sys/time.h>   // To add prototype of gettimeofday(...) function

#define CLIENT_ADDRESS "127.0.0.1"
#define CLIENT_PORT "9999"     // Server port that it listens

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT "9000"

#define BUFFER_SIZE 1024   // Max size of data in single packet

// Header types
#define NHELLO "NHELLO"
#define CLIENT "CLIENT"
#define SERVER "SERVER"


int main(int argc, char **argv) {
    // ---------------- Connecting ------------------
    struct addrinfo node_config, *node_net_config = NULL;  // h, r
    struct sockaddr_in server_net_info; // c
    int socket_info, pos;   // s, pos
    socklen_t server_info_size = sizeof(server_net_info);      // c_len

    // Setting variables for UDP connection type
    memset(&node_config, 0, sizeof(struct addrinfo)); // Setting node_config bytes to 0
    node_config.ai_family = PF_INET;
    node_config.ai_socktype = SOCK_DGRAM;
    node_config.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, CLIENT_PORT, &node_config, &node_net_config) != 0) {     // Setting addrinfo struct to proper values
        printf("getaddrinfo not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    } else {
        printf("Address info of server set.\n");
    }

    if ((socket_info = socket(node_net_config->ai_family, node_net_config->ai_socktype, node_net_config->ai_protocol)) == -1) { // Defining socket
        printf("socket not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(node_net_config);
        exit(-1);
    } else {
        printf("Socket created.\n");
    }

    if(bind(socket_info, node_net_config->ai_addr, node_net_config->ai_addrlen) != 0) {   // Binding socket
        printf("Bind error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(node_net_config);
        close(socket_info);
        exit(-1);
    } else {
        printf("Bind succuess\n");
    }

    char message_received[BUFFER_SIZE];  // Variable for server response messages
    char message_to_send[BUFFER_SIZE];  // Variable for server response messages
    // unsigned char header[7];        // variable to store type of message



    struct addrinfo server_config, *server_net_config = NULL;   // client_hello, res

    memset(&server_config, 0, sizeof(struct addrinfo));
    server_config.ai_family = PF_INET;
    server_config.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(SERVER_ADDRESS, SERVER_PORT, &server_config, &server_net_config ) != 0) { 
        printf("getaddrinfo, error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(node_net_config);
        close(socket_info);
        exit(-1);
    } else {
        printf("Host info set\n");
    }

    // snprintf(message_to_send, BUFFER_SIZE, NHELLO, CLIENT_PORT);
    strncpy(message_to_send, "Test Message", 13);
    pos = sendto(socket_info, message_to_send, strlen(message_to_send), 0, server_net_config->ai_addr, server_net_config->ai_addrlen);
    if(pos < 0) {
        // printf("ERROR: %s\n", strerror(errno), __FILE__, __LINE__);
    } else if(pos > 0) {
        printf("Succesfully sent %d bytes\n", pos);
    }


    // Exchanging informations with client
    srand(time(NULL));
    for(;;) {
        // Receiveing from client
        if((pos = recvfrom(socket_info, message_received, BUFFER_SIZE, 0, (struct sockaddr*) &server_net_info, &server_info_size )) < 0){
            printf("ERROR: %s\n", strerror(errno));
            freeaddrinfo(node_net_config);
            freeaddrinfo(server_net_config);
            close(socket_info);
            exit(-1);
        }  else {
            printf("Succesfully received %d byte message\n", pos);
        } 
        message_received[pos] = '\0';


        // printf("Message Received from %s:%d\n       HEADER:    %s\n       MESSAGE:   %s\n\n", inet_ntoa(server_net_info.sin_addr), ntohs(server_net_info.sin_port), header, message);
        // Sending response to client
        // snprintf(message_to_send, BUFFER_SIZE, "%s%s%s", SERVER, numbers_as_words[rand() % 10], message);
        if((pos = sendto(socket_info, message_to_send, strlen(message_to_send), 0, (const struct sockaddr *) &server_net_info, server_info_size)) < 0) {
            // printf("ERROR: %s\n", strerror(errno), __FILE__, __LINE__);
            // TODO Do something if server did not send hello
        }
    }

    // Clearing space and freeing resources
    freeaddrinfo(node_net_config);
    freeaddrinfo(server_net_config);
    close(socket_info);

    return 0;
}