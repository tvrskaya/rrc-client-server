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
#include <Utils/RRCUtils.h>
#include <DL-CCCH-Message.h>
#define PORT 8080
#define BUFFER_SIZE 1024

int main(void) {

    uint8_t* buffer;
    ssize_t len;
    RRCConnectionRequest_coder(&buffer, &len);

    struct sockaddr_in servaddr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = inet_addr("172.17.0.1"),
    };

    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (sockfd < 0) {
        printf("Error when opening socket\n");
        exit(1);
    }

    int ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret < 0) {
        printf("Error when connecting socket\n");
        exit(1);
    }

    printf("Len client: %ld\n", len);
    ret = sctp_sendmsg(sockfd, buffer, len, NULL, 0, 0, 0, 0, 0, 0);
    if (ret < 0) {
        printf("Error when sending msg\n");
        exit(1);
    }

    printf("Sent packet\n");

    uint8_t bufferSetup[BUFFER_SIZE] = {0};
    ssize_t valread = sctp_recvmsg(sockfd, bufferSetup, BUFFER_SIZE, NULL, 0, 0, 0);
    if (valread < 0) {
        printf("Fail to get bytes from message(");
        return -1;
    }
    printf("Recv packet\n");

    UL_CCCH_Message_t* msg = 0;
    asn_dec_rval_t rval; //decoder return value
    rval = asn_decode(NULL, ATS_CANONICAL_XER, &asn_DEF_DL_CCCH_Message, (void**)&msg, bufferSetup, valread);
    if (rval.code == RC_FAIL) {
        printf("Fail to decode msg\n");
        exit(1);
    }
    xer_fprint(stdout, &asn_DEF_DL_CCCH_Message, msg);

    close(sockfd);
}