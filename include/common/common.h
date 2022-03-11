#ifndef COMMON_H
#define COMMON_H

#define MEMC                0x6d656d63

int exec_payload(io_client_t client, unsigned char* data, size_t sz);

void send_payload_no_error(io_client_t client, unsigned char* payload, size_t payloadLen);

#endif
