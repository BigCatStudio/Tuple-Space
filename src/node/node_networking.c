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

#include "node/node_networking.h"


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



static void decode_message(ALP_message* message_decoded, char* message_coded, uint32_t size) {
    // Header
    uint16_t header_int = 0;
    header_int = 0xFF00 & ((unsigned char)message_coded[0] << 8);
    header_int += 0x00FF & ((unsigned char)message_coded[1]);

    message_decoded->operation_type = 0x0007 & (header_int >> 13);
    message_decoded->device_id = 0x007F & (header_int >> 6);
    message_decoded->fields_amount = 0x003F & (header_int);

    // Tuple fields
    memset(message_decoded->tuple, 0, sizeof(message_decoded->tuple));
    for(uint32_t i = 2;i < size;i++) {
        message_decoded->tuple[i - 2] = message_coded[i];
    }
}

static bool create_header(char* header, uint8_t operation_type, uint8_t fields_amount) {
    // TODO make all header types in enum
    // TODO change it to check if operation_type is in enum of header types
    if(operation_type < INP || operation_type > ACK) {
        return false;
    } 

    if(fields_amount > FIELDS_MAX || fields_amount < 0) {
        return false;
    }

    uint16_t header_int = 0x0000;

    header_int = 0xFFFF & (operation_type << 13);   // Bit multiplication and moving them to be next to each other
    header_int += 0xFFFF & (device_id << 6);
    header_int += 0xFFFF & (fields_amount);

    header[0] = (header_int >> 8) & 0xFF;
    header[1] = (header_int) & 0xFF;

    return true;
}


bool setup_networking(networking_stack* net) {
    net->node_net_config = NULL;
    net->server_net_config = NULL;
    net->server_info_size = sizeof(net->server_net_info);

    // Setting variables for UDP connection type
    memset(&net->node_config, 0, sizeof(struct addrinfo)); // Setting node_config bytes to 0
    net->node_config.ai_family = PF_INET;
    net->node_config.ai_socktype = SOCK_DGRAM;
    net->node_config.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, CLIENT_PORT, &(net->node_config), &(net->node_net_config)) != 0) {     // Setting addrinfo struct to proper values
        printf("getaddrinfo not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        exit(-1);
    } else {
        printf("Address info of server set.\n");
    }

    if ((net->socket_info = socket(net->node_net_config->ai_family, net->node_net_config->ai_socktype, net->node_net_config->ai_protocol)) == -1) { // Defining socket
        printf("socket not working\n");
        printf("ERROR: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net->node_net_config);
        exit(-1);
    } else {
        printf("Socket created.\n");
    }

    if(bind(net->socket_info, net->node_net_config->ai_addr, net->node_net_config->ai_addrlen) != 0) {   // Binding socket
        printf("Bind error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net->node_net_config);
        close(net->socket_info);
        exit(-1);
    } else {
        printf("Bind succuess\n");
    }

    memset(&net->server_config, 0, sizeof(struct addrinfo));
    net->server_config.ai_family = PF_INET;
    net->server_config.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(SERVER_ADDRESS, SERVER_PORT, &net->server_config, &net->server_net_config ) != 0) { 
        printf("getaddrinfo, error: %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net->node_net_config);
        close(net->socket_info);
        exit(-1);
    } else {
        printf("Host info set\n");
    }

    return true;
}


void send_message(uint8_t operation_type, uint8_t fields_amount, char* tuple, uint32_t size) {
    char header[3];
    memset(header, 0, sizeof(header)); // Clearing header
    memset(packet_buffer, 0, sizeof(packet_buffer)); // Clearing buffer

    if(!create_header(header, operation_type, fields_amount)) {
        printf("Provided values are invalid\n");    // TODO checking values should be earlier in function call's stack
        // TODO add exiting from function with error
    } else {
        packet_buffer[0] = header[0];
        packet_buffer[1] = header[1];
    }

    if((operation_type == INP) || (operation_type == RDP) || (operation_type == OUT)) {
        if(tuple != NULL) {
            for(int i = 0;i < size;i++) {
                packet_buffer[i + HEADER_LENGTH] = tuple[i];
            }
        }
    }

    // TODO Sending packet
    // Udp.beginPacket(Server_IP, server_port);
    // Udp.write(packet_buffer, size + HEADER_LENGTH);
    // Udp.endPacket();
}



int main(int argc, char **argv) {
    // ---------------- Connecting ------------------
    networking_stack net;
    setup_networking(&net);


    // snprintf(packet_buffer, PACKET_BUFFER_LENGTH, NHELLO, CLIENT_PORT);
    strncpy(packet_buffer, "Test Message", strlen("Test Message") + 1);
    net.pos = sendto(net.socket_info, packet_buffer, strlen(packet_buffer), 0, net.server_net_config->ai_addr, net.server_net_config->ai_addrlen);
    if(net.pos < 0) {
        // printf("ERROR: %s\n", strerror(errno), __FILE__, __LINE__);
    } else if(net.pos > 0) {
        printf("Succesfully sent %d bytes\n", net.pos);
    }


    // Exchanging informations with client
    srand(time(NULL));
    for(;;) {
        // Receiveing from client
        if((net.pos = recvfrom(net.socket_info, packet_buffer, PACKET_BUFFER_LENGTH, 0, (struct sockaddr*) &(net.server_net_info), &(net.server_info_size))) < 0){
            printf("ERROR: %s\n", strerror(errno));
            freeaddrinfo(net.node_net_config);
            freeaddrinfo(net.server_net_config);
            close(net.socket_info);
            exit(-1);
        }  else {
            printf("Succesfully received %d byte message\n", net.pos);
        }
        packet_buffer[net.pos] = '\0';


        // printf("Message Received from %s:%d\n       HEADER:    %s\n       MESSAGE:   %s\n\n", inet_ntoa(server_net_info.sin_addr), ntohs(server_net_info.sin_port), header, message);
        if((net.pos = sendto(net.socket_info, packet_buffer, strlen(packet_buffer), 0, (const struct sockaddr *) &(net.server_net_info), net.server_info_size)) < 0) {
            // TODO Do something if server did not send hello
        }
        
    }

    // Clearing space and freeing resources
    freeaddrinfo(net.node_net_config);
    freeaddrinfo(net.server_net_config);
    close(net.socket_info);

    return 0;
}