#include <RRCConnectionRequest.h>
#include <stdio.h>
#include <sys/types.h>

void RRCConnectionRequest_coder(uint8_t **buffer, ssize_t *len);
void RRCConnectionRequest_decoder(uint8_t **buffer, ssize_t *len);
void RRCConnectionSetup_coder(uint8_t **buffer, ssize_t *len);