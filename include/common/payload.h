#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>

#define VROM_PAGE_OFFSET (0x400)
#define SRAM_PAGE_OFFSET (0x600)
#define PAGESIZE         (0x2000000)

#define MAX_BLOCK_SIZE   (0x50)
#define MAX_ROP_SIZE     (0x200)
#define MAX_PAYLOAD_SIZE (0x800)

typedef struct callback_t{
    uint64_t arg0;
    uint64_t arg1;
    struct callback_t *next;
} callback_t;

int payload_gen(io_client_t client, checkra1n_payload_t* payload, bool eclipsa);

#endif
