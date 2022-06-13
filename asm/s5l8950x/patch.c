#include <stdint.h>
#include <stdbool.h>

typedef void (*arm_write_ttb_t)(uint32_t val);
typedef void (*arm_flush_tlbs_t)(void);
typedef void (*bcopy_t)(uint32_t src, uint32_t dest, uint32_t n);
typedef void (*real_payload_t)(void);
typedef void (*checkm8_payload_t)(void);

__attribute__((noinline)) arm_write_ttb_t arm_write_ttb = 0x3f00044c;
__attribute__((noinline)) arm_flush_tlbs_t arm_flush_tlbs = 0x3f000698;
__attribute__((noinline)) bcopy_t my_bcopy = 0x3f009ac0;

__attribute__((noinline)) real_payload_t real_payload = 0x10079891;
__attribute__((noinline)) checkm8_payload_t checkm8_payload = 0x10000000;

__attribute__((noinline)) uint32_t src = 0x41414141;
__attribute__((noinline)) uint32_t size = 0x42424242;

int patchROM(void);

__attribute__((noinline)) void jump(void) // 0x00000291
{
    my_bcopy(0x1000012C, 0x10079800, 0x90); // relocate jumpto()
    my_bcopy(src, 0x10079890, size); // relocate real_payload()
    
    return real_payload(); // -----------------------------------+
}                          //                                    |
                           //                                    |
__attribute__((noinline)) int payload(void) // real_payload() <--+
{
    checkm8_payload(); // sub_10000000()
    patchROM();
    
    return 0;
}

__attribute__((noinline)) int patchROM(void)
{
    uint32_t patchAddr = 0;
    
    my_bcopy(0x000, 0x10000000, 0x012C);
    my_bcopy(0x200, 0x10000200, 0x0090);
    my_bcopy(0x300, 0x10000300, 0xFD00);
    
    // patch loadaddr, imagesize
    patchAddr = 0x100009d8;
    *(uint32_t*)patchAddr = 0xbf004a3d;
    patchAddr = 0x100009f0;
    *(uint32_t*)patchAddr = 0xbf004837;
    patchAddr = 0x10000a02;
    *(uint32_t*)patchAddr = 0xbf004a33;
    patchAddr = 0x10000a14;
    *(uint32_t*)patchAddr = 0xbf00482e;
    patchAddr = 0x10000a48;
    *(uint32_t*)patchAddr = 0xbf004821;
    patchAddr = 0x10000a68;
    *(uint32_t*)patchAddr = 0xbf004819;
    patchAddr = 0x10000a82;
    *(uint32_t*)patchAddr = 0xbf004a13;
    patchAddr = 0x10000ab8;
    *(uint32_t*)patchAddr = 0x00050000;
    patchAddr = 0x10000ad0;
    *(uint32_t*)patchAddr = 0x10010000;
    
    // security_init
    // security_clear_mem_in_chunks
    patchAddr = 0x10006b18;
    *(uint32_t*)patchAddr = 0xbf00480d;
    patchAddr = 0x10006b1c;
    *(uint16_t*)patchAddr = 0x490d;
    
    patchAddr = 0x10006b50;
    *(uint32_t*)patchAddr = 0x10010000;
    patchAddr = 0x10006b54;
    *(uint32_t*)patchAddr = 0x00050000;
    
    // jump shellcode
    patchAddr = 0x1000086e;
    *(uint16_t*)patchAddr = 0xbf00; // nop
    patchAddr = 0x10000870;
    *(uint32_t*)patchAddr = 0x47004800; // ldr r0, =jumpto
    patchAddr = 0x10000874;
    *(uint32_t*)patchAddr = 0x10079801; // bx r0
    patchAddr = 0x10000878;
    *(uint32_t*)patchAddr = 0xbf00bf00; // nop
    
    // sigcheck patch
    // verify_shsh
    patchAddr = 0x1000499c;
    *(uint32_t*)patchAddr = 0x60182000;
    
    // shsh
    patchAddr = 0x100049e0; // SDOM
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004a02; // PROD
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004a4c; // CHIP
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004a6e; // TYPE
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004a94; // SEPO
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004ab4; // CEPO
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004ad4; // BORD
    *(uint32_t*)patchAddr = 0x20002000;
    patchAddr = 0x10004afa; // ECID
    *(uint32_t*)patchAddr = 0x20002000;
    
    // write ttbr0
    uint32_t ttbr0      = 0x1007c000;
    *(uint32_t*)ttbr0   = 0x10000c1e;
    
    arm_write_ttb(ttbr0);
    arm_flush_tlbs();
    
    return 0;
}
