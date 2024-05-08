#include "server/server_networking.h"

#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h> // Definition of INET_ADDRSTRLEN (IP address)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
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

// Header types
#define INP 1
#define RDP 2
#define OUT 3
#define HELLO 4
#define ACK 5

unsigned char packet_buffer[PACKET_BUFFER_LENGTH];
uint8_t device_id = 97;

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

void decode_message(ALP_message* message_decoded, char* message_coded, uint32_t size) {
    // Header
    uint16_t header_int = 0;
    header_int = 0xFF00 & ((unsigned char)message_coded[0] << 8);
    header_int += 0x00FF & ((unsigned char)message_coded[1]);

    message_decoded->operation_type = 0x0007 & (header_int >> 13);
    message_decoded->device_id = 0x007F & (header_int >> 6);
    message_decoded->fields_amount = 0x003F & (header_int);

    // Tuple fields
    memset(message_decoded->tuple, '\0', PACKET_BUFFER_LENGTH - HEADER_LENGTH);
    for(uint32_t i = 2;i < size;i++) {
        message_decoded->tuple[i - 2] = message_coded[i];
        // printf("%d:%c\n", i, message_decoded->tuple[i - 2]);
    }
    //strncpy(message_decoded->tuple, message_coded + HEADER_LENGTH, PACKET_BUFFER_LENGTH - HEADER_LENGTH);
}


// Setting socket, addresses etc 
void setup_networking(networking_stack* net) {
    net->r = NULL;
    net->c_len = sizeof(net->client_info);

    // Setting variables for UDP connection type
    memset(&(net->h), 0, sizeof(struct addrinfo)); // Setting h bytes to 0
    net->h.ai_family = PF_INET;
    net->h.ai_socktype = SOCK_DGRAM;
    net->h.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, SERVER_PORT, &(net->h), &(net->r)) != 0) {     // Setting addrinfo struct to proper values
        printf("getaddrinfo not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    } else {
        printf("Address info of server set.\n");
    }

    if ((net->socket = socket(net->r->ai_family, net->r->ai_socktype, net->r->ai_protocol)) == -1) { // Defining socket
        printf("socket not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net->r);
        exit(-1);
    } else {
        printf("Socket created.\n");
    }

    if (bind(net->socket, net->r->ai_addr, net->r->ai_addrlen) != 0) {      // Binding socket
        printf("bind not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net->r);
        close(net->socket);
        exit(-1);
    } else {
        printf("Bind success\n\n");
    }
}

uint8_t create_header(char* header, uint8_t operation_type, uint8_t fields_amount) {
    if(operation_type < 1 || operation_type > 5) {
        return 0;
    } 

    if(fields_amount > 63 || fields_amount < 0) {
        return 0;
    } 

    // operation_type  3 bits
    // device_id       7 bits
    // fields_amount   6 bits
    // |xxx|xxxxxxx|xxxxxx| = uint16_t

    uint16_t header_int = 0x0000;

    header_int = 0xFFFF & (operation_type << 13);   // Bit multiplication and moving them to be next to each other
    header_int += 0xFFFF & (device_id << 6);
    header_int += 0xFFFF & (fields_amount);

    header[0] = (header_int >> 8) & 0xFF;
    header[1] = (header_int) & 0xFF;
    
    return 1;
}

void send_message(networking_stack* net, uint8_t operation_type, uint8_t fields_amount, char* tuple, uint32_t size) {
    char header[3];

    memset(packet_buffer, 0, sizeof(packet_buffer)); // Clearing buffer
    memset(header, 0, sizeof(header)); // Clearing buffer

    if(!create_header(header, operation_type, fields_amount)) {
        // TODO add logic if header fails
        printf("Header was not created");
    } else {
        packet_buffer[0] = header[0];
        packet_buffer[1] = header[1];
        // strncpy(packet_buffer, header, HEADER_LENGTH);
    }

    if(tuple != NULL) {
        for(int i = 0;i < size;i++) {
            packet_buffer[i + 2] = tuple[i];
        }
        //strncat(packet_buffer, tuple, size);
    }
    
    int pos = 0;

    if((pos = sendto(net->socket, packet_buffer, size + HEADER_LENGTH, 0, (const struct sockaddr *) &(net->client_info), net->c_len)) < 0) {
        printf("ERROR: %s\n", strerror(errno));     
        // TODO maybe something more when send failed?
    } else if(pos > 0) {
        printf("Succesfully sent %d bytes as an ACK\n", pos);
    }
}

int main(int argc, char **argv) {
    networking_stack net;
    setup_networking(&net);
    // initialize_meta_tuple_space();

    // Defineing variables to make sure client won't stop on blocking function
    fd_set readfds;     
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 5000;
    int new_data;

    for(;;) {
        FD_ZERO(&readfds);      // Setting variable to 0 to keep track of new data coming
        FD_SET(net.socket, &readfds);    // Assigning tracking variable to selected socket

        if((new_data = select(net.socket + 1, &readfds, NULL, NULL, &tv)) < 0) {  // If error occurs
            printf("ERROR: %s\n", strerror(errno));
            // TODO add some logic
        } else if(new_data > 0) {      // If data came to selected socket
            if(FD_ISSET(net.socket, &readfds)) {
                memset(packet_buffer, 0, sizeof(packet_buffer)); // Clearing buffer

                uint32_t pos = recvfrom(net.socket, packet_buffer, PACKET_BUFFER_LENGTH, 0, (struct sockaddr*) &(net.client_info), &(net.c_len));

                if(pos < 0) {
                    printf("ERROR: %s\n", strerror(errno));
                    freeaddrinfo(net.r);
                    close(net.socket);
                    exit(-1);
                }
                packet_buffer[pos] = '\0';

                // Header
                // ALP_message message;
                // decode_message(&message, packet_buffer, pos);

                printf("\nMessage Received from Client %s :%d\n\t Size:%d\nMessage:", 
                    inet_ntoa(net.client_info.sin_addr), \
                    ntohs(net.client_info.sin_port), \
                    pos
                );

                for(size_t i = 0;i < pos;i++) {
                    printf("%c", packet_buffer[i]);
                }
                printf("\n");

                // for(size_t i = 0;i < pos;i++) {
                //     printf("%c", packet_buffer[i]);
                // }

                /*
                printf("\nMessage Received from Client %s :%d\n\tHEADER:\n\t\toperation type:%d\n\t\tdevice ID:%d\n\t\tfields_amount:%d\n\tMESSAGE:", 
                    inet_ntoa(net.client_info.sin_addr), \
                    ntohs(net.client_info.sin_port), \
                    message.operation_type, \
                    message.device_id, \
                    message.fields_amount
                ); */

                // for(int i = 0;i < pos - 2;i++) {
                //     printf("%c", message.tuple[i]);
                // }
                // printf("\n");

                // if(message.operation_type == INP) {
                //     uint32_t size = server_inp(message.fields_amount, message.tuple);
                //     if(size > 0) {
                //         send_message(&net, ACK, message.fields_amount, message.tuple, size);
                //     } else {
                //         send_message(&net, ACK, 0, NULL, 0);
                //     }
                // } else if(message.operation_type == RDP) {
                //     uint32_t size = server_rdp(message.fields_amount, message.tuple);
                //     if(size > 0) {
                //         send_message(&net, ACK, message.fields_amount, message.tuple, size);
                //     } else {
                //         send_message(&net, ACK, 0, NULL, 0);
                //     }
                // } else if(message.operation_type == OUT) {
                //     if(server_out(message.fields_amount, message.tuple, pos - 2)) {      // Sending ACK only if tuple was correctly added
                //         send_message(&net, ACK, 0, NULL, 0);
                //     }
                // } else if(message.operation_type == HELLO) {
                //     send_message(&net, ACK, 0, NULL, 0);
                // }
            }
        }
    }
    // Clearing space and freeing resources
    freeaddrinfo(net.r);
    close(net.socket);

    return 0;
}