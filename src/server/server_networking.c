#include "server/server_networking.h"

char packet_buffer[PACKET_BUFFER_LENGTH];
uint8_t device_id = 97;

// TODO change size data type to size_t

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
    //printf("Decoded message\n");
    for(uint32_t i = 2;i < size;i++) {
        message_decoded->tuple[i - 2] = message_coded[i];
        //printf("%d:%c\n", i, message_decoded->tuple[i - 2]);
    }
    //printf("\n");
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

bool create_header(char* header, uint8_t operation_type, uint8_t fields_amount) {
    // TODO make operation_type global enup and check if provided value is in enum
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
    
    // Creating tuple space structure
    tuple_space ts;
    ts.lists = NULL;

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

                size_t pos = recvfrom(net.socket, packet_buffer, PACKET_BUFFER_LENGTH, 0, (struct sockaddr*) &(net.client_info), &(net.c_len));

                if(pos < 0) {
                    printf("ERROR: %s\n", strerror(errno));
                    freeaddrinfo(net.r);
                    close(net.socket);
                    exit(-1);
                }
                packet_buffer[pos] = '\0';

                ALP_message message;
                decode_message(&message, packet_buffer, pos);

                
                printf("\nMessage Received from Client %s:%d\n\tSIZE:%lu\n\tHEADER:\n\t\toperation type:%d\n\t\tdevice ID:%d\n\t\tfields_amount:%d\n\tMESSAGE:", 
                    inet_ntoa(net.client_info.sin_addr), \
                    ntohs(net.client_info.sin_port), \
                    pos, \
                    message.operation_type, \
                    message.device_id, \
                    message.fields_amount
                );

                for(size_t i = 0;i < pos;i++) {
                    printf("%c", packet_buffer[i]);
                }
                printf("\n");


                if(message.operation_type == INP) {
                    if(add_tuple(&ts, message.tuple, message.fields_amount, pos - HEADER_LENGTH)) {
                        // send_message(&net, ACK, message.fields_amount, message.tuple, size);
                    } else {
                        // send_message(&net, ACK, 0, NULL, 0);
                    }
                } else if(message.operation_type == RDP) {
                    char* tuple = get_tuple(&ts, message.tuple, message.fields_amount, pos - HEADER_LENGTH);
                    printf("Checking Tuple Read!\n");
                    if(tuple != NULL) {
                        printf("Tuple Found!\n");
                        // send_message(&net, ACK, message.fields_amount, message.tuple, size);
                    } else {
                        printf("Tuple Not Found!\n");
                        // send_message(&net, ACK, 0, NULL, 0);
                    }
                } else if(message.operation_type == OUT) {
                    char* tuple = get_tuple(&ts, message.tuple, message.fields_amount, pos - HEADER_LENGTH);
                    if(tuple != NULL) {      // Sending ACK only if tuple was correctly added
                        if(!remove_tuple(&ts, message.tuple, message.fields_amount, pos - HEADER_LENGTH)) {
                            // TODO log error
                            // Tuple found eariler but could not be deleted from tuple space
                        }
                        // send_message(&net, ACK, 0, NULL, 0);
                    }
                } else if(message.operation_type == HELLO) {
                    // send_message(&net, ACK, 0, NULL, 0);
                }

                display_tuple_space(&ts);   // TODO remove from prod
            }
        }
    }
    // Clearing space and freeing resources
    clear_tuple_space(&ts);
    freeaddrinfo(net.r);
    close(net.socket);

    return 0;
}