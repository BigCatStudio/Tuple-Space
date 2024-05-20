#include "node/node_networking.h"

char packet_buffer[PACKET_BUFFER_LENGTH];
uint8_t device_id = 113;

networking_stack net;   // struct with all necessary fields for network communication with tuple space server 


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


bool setup_networking() {
    net.node_net_config = NULL;
    net.server_net_config = NULL;
    net.server_info_size = sizeof(net.server_net_info);

    // Setting variables for UDP connection type
    memset(&net.node_config, 0, sizeof(struct addrinfo)); // Setting node_config bytes to 0
    net.node_config.ai_family = PF_INET;
    net.node_config.ai_socktype = SOCK_DGRAM;
    net.node_config.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, CLIENT_PORT, &(net.node_config), &(net.node_net_config)) != 0) {     // Setting addrinfo struct to proper values
        printf("ERROR: %s:%s (%s:%d)\n","getaddrinfo not working", strerror(errno), __FILE__, __LINE__);
        return false;
    } else {
        printf("Address info of client set\n");
    }

    if ((net.socket_info = socket(net.node_net_config->ai_family, net.node_net_config->ai_socktype, net.node_net_config->ai_protocol)) == -1) { // Defining socket
        printf("ERROR: %s:%s (%s:%d)\n", "socket not working", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net.node_net_config);
        return false;
    } else {
        printf("Socket created\n");
    }

    if(bind(net.socket_info, net.node_net_config->ai_addr, net.node_net_config->ai_addrlen) != 0) {   // Binding socket
        printf("ERROR: %s:%s (%s:%d)\n", "binding not working", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net.node_net_config);
        close(net.socket_info);
        return false;
    } else {
        printf("Bind succuess\n");
    }

    memset(&net.server_config, 0, sizeof(struct addrinfo));
    net.server_config.ai_family = PF_INET;
    net.server_config.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(SERVER_ADDRESS, SERVER_PORT, &net.server_config, &net.server_net_config ) != 0) { 
        printf("ERROR: %s:%s (%s:%d)\n","getaddrinfo not working", strerror(errno), __FILE__, __LINE__);
        freeaddrinfo(net.node_net_config);
        close(net.socket_info);
        return false;
    } else {
        printf("Address info of server set\n");
    }

    // TODO send hello to server

    return true;
}

void clean_networking() {
    // Clearing space and freeing resources
    freeaddrinfo(net.node_net_config);
    freeaddrinfo(net.server_net_config);
    close(net.socket_info);
}

bool send_message(uint8_t operation_type, uint8_t fields_amount, char* buffer, uint32_t size) {
    char header[3];
    memset(header, 0, sizeof(header));
    memset(packet_buffer, 0, PACKET_BUFFER_LENGTH);

    if(!create_header(header, operation_type, fields_amount)) {
        printf("ERROR: %s:%s (%s:%d)\n","provided values are invalid", strerror(errno), __FILE__, __LINE__);    // TODO checking values should be earlier in function call's stack
        return false;
    } else {
        packet_buffer[0] = header[0];
        packet_buffer[1] = header[1];
    }

    if((operation_type == INP) || (operation_type == RDP) || (operation_type == OUT)) {
        if(buffer != NULL) {
            for(size_t i = 0;i < size;i++) {
                packet_buffer[i + HEADER_LENGTH] = buffer[i];
            }
        }
    }

    net.pos = sendto(net.socket_info, packet_buffer, size + HEADER_LENGTH, 0, net.server_net_config->ai_addr, net.server_net_config->ai_addrlen);
    if(net.pos < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","sending failed", strerror(errno), __FILE__, __LINE__);
        return false;
    } else if(net.pos > 0) {
        printf("Succesfully sent %d bytes\n", net.pos);
    }

    return true;
    // TODO Wait for packet with tuple from server 
}

bool receive_message(char* buffer) {
    size_t pos = recvfrom(net.socket_info, packet_buffer, PACKET_BUFFER_LENGTH, 0, (struct sockaddr*) &(net.server_net_info), &(net.server_info_size));
    if(pos < 0) {
        printf("ERROR: %s:%s (%s:%d)\n","failed receiving packet", strerror(errno), __FILE__, __LINE__);
        return false;
    }

    ALP_message message;
    decode_message(&message, packet_buffer, pos);

    printf("\nMessage Received from Server %s:%d\n\tSIZE:%lu\n\tHEADER:\n\t\toperation type:%d\n\t\tdevice ID:%d\n\t\tfields_amount:%d\n", 
                    inet_ntoa(net.server_net_info.sin_addr), \
                    ntohs(net.server_net_info.sin_port), \
                    pos, \
                    message.operation_type, \
                    message.device_id, \
                    message.fields_amount
    );

    memcpy(buffer, message.tuple, pos - 2);

    return true;
}
