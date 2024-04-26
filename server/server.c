#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define _GNU_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <UL-CCCH-Message.h>
#include <UL-DCCH-Message.h>

#include <Utils/RRCUtils.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    uint8_t buffer[BUFFER_SIZE] = {0};

    // create a socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == 0) {
        perror("socket failed!");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // setup the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind fails");
        exit(EXIT_FAILURE);
    }

    // start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listening failed");
        exit(EXIT_FAILURE);
    }

    // print a message showing that the server is listening on a port
    printf("Server listening on port %d\n", PORT);
    while(1) {
        // accept incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // accepting the connection
        printf("Connection accepted\n");

        // read data from the client and print it
        ssize_t valread = sctp_recvmsg(new_socket, buffer, BUFFER_SIZE, NULL, 0, 0, 0);
        if (valread < 0) {
            printf("Fail to get bytes from message(\n");
            exit(EXIT_FAILURE);
        }

        //при использовании функции ловлю сегфолт, так и не понял, почему
        //RRCConnectionRequest_decoder(&buffer, &valread);
        //поэтому декодируем так
        UL_CCCH_Message_t* msg = 0;
        asn_dec_rval_t rval; //decoder return value
        rval = asn_decode(NULL, ATS_CANONICAL_XER, &asn_DEF_UL_CCCH_Message, (void**)&msg, buffer, valread);
        if (rval.code == RC_FAIL) {
            printf("Fail to decode msg\n");
            exit(EXIT_FAILURE);
        }
        xer_fprint(stdout, &asn_DEF_UL_CCCH_Message, msg);

        //После получения ConnectionRequest кодируем ответ ConnectionSetup
        uint8_t* bufferSetup;
        ssize_t lenSetup;
        RRCConnectionSetup_coder(&bufferSetup, &lenSetup);

        int ret = sctp_sendmsg(new_socket, bufferSetup, lenSetup, NULL, 0, 0, 0, 0, 0, 0);
        if (ret < 0) {
            printf("Error when sending msg\n");
            exit(EXIT_FAILURE);
        }
        uint8_t bufferComplete[BUFFER_SIZE] = {0};
        valread = sctp_recvmsg(new_socket, bufferComplete, BUFFER_SIZE, NULL, 0, 0, 0);
        if (valread < 0) {
            printf("Fail to get bytes from message(\n");
            exit(EXIT_FAILURE);
        }

        printf("%ld\n", valread);
        UL_DCCH_Message_t* msgComplete = 0;
        rval = asn_decode(NULL, ATS_CANONICAL_XER, &asn_DEF_UL_DCCH_Message, (void**)&msgComplete, bufferComplete, valread);
        if (rval.code == RC_FAIL) {
            printf("Fail to decode msg\n");
            exit(EXIT_FAILURE);
        }
        xer_fprint(stdout, &asn_DEF_UL_DCCH_Message, msgComplete);
        printf("Connection established!!!!!!!\n");
    }

    // close the socket
    close(server_fd);
    return 0;
}