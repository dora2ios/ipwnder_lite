#include <stdint.h>
#include <stdbool.h>

typedef void (*arm_write_ttb_t)(uint32_t val);
typedef void (*arm_flush_tlbs_t)(void);
typedef int (*prepare_and_jump_t)(uint32_t boot, uint32_t ptr, uint32_t arg);
typedef uint32_t (*platform_get_boot_trampoline_t)(void);
typedef void (*bcopy_t)(uint32_t src, uint32_t dest, uint32_t n);

__attribute__((noinline)) arm_write_ttb_t arm_write_ttb         = 0x3f00044c;
__attribute__((noinline)) arm_flush_tlbs_t arm_flush_tlbs       = 0x3f000698;
__attribute__((noinline)) prepare_and_jump_t prepare_and_jump   = 0x3f005f81;
__attribute__((noinline)) platform_get_boot_trampoline_t platform_get_boot_trampoline = 0x3f006e85;
__attribute__((noinline)) bcopy_t my_bcopy                      = 0x3f009ac0;

uint32_t jumpto(void) // 0x10079801
{
    uint32_t ttbr0      = 0x1007c000;
    *(uint32_t*)ttbr0   = 0x3f000c1e;
    
    arm_write_ttb(ttbr0);
    arm_flush_tlbs();
    
    my_bcopy(0x10010000, 0x10000000, 0x50000);
    
    return prepare_and_jump(0, platform_get_boot_trampoline(), 0x10000000);
}
