.text

.pool
.set LOAD_ADDRESS,              0xBAD00001
.set IMG_MAGIC,                 0xBAD00002
.set TAG_DATA,                  0xBAD00003
.set EXCEPTION_VECTOR,          0xBAD00004
.set TAG_KBAG,                  0xBAD00005
.set AES_TYPE_GID,              0xBAD00006
.set AES_CRYPTO_CMD,            0xBAD00007
.set AES_MODE_256,              0xBAD00008
.set GET_BOOT_TRAMPOLINE,       0xBAD0000b
.set JUMPTO,                    0xBAD0000c

.set VERIFY_SHSH,               0xBAD00009
.set INSN2_MOV_R0_0__STR_R0_R3, 0xBAD0000a

.code 16
.global _main
_main:

_jump_back:
bkpt    #1
bkpt    #1
bkpt    #1
bkpt    #1

ldrh    r2, [r0]
movw    r3, #0x2A1
cmp     r2, r3
bne     _jump_back

push    {r4-r7,lr}
add     r7, sp, #0xC
sub     sp, sp, #0x10

mov     r4, r0
ldr     r5, =LOAD_ADDRESS

movw    r1, #0xFFFF
ldrh    r2, [r4,#2]
cmp     r1, r2
bne     _request_done

ldrd    r0, r1, [r5]

ldr     r2, =IMG_MAGIC
cmp     r0, r2
bne     _request_done

_decrypt_image:
ldr     r0, =TAG_DATA
ldr     r1, =LOAD_ADDRESS
ldr     r2, [r1, #4]            @ header->fullSize
bl      _find
cmp     r0, #0
beq     _request_done
mov     r6, r0

mov     r1, #0xc
add     r1, r6
ldr     r2, [r1]
ldr     r1, =EXCEPTION_VECTOR
cmp     r1, r2
beq     _copy_image

ldr     r0, =TAG_KBAG
ldr     r1, =LOAD_ADDRESS
ldr     r2, [r1, #4]            @ header->fullSize
bl      _find
cmp     r0, #0
beq     _request_done
mov     r5, r0

mov     r0, #0x11               @ AES_DECRYPT
mov     r4, #0x14
add     r1, r5, r4
add     r2, r5, r4
mov     r3, #0x30               @ KBAG_KEY_IV_SIZE
ldr     r4, =AES_TYPE_GID
str     r4, [sp]
mov     r4, #0
str     r4, [sp, #4]
mov     r4, #0
str     r4, [sp, #8]
ldr     r4, =AES_CRYPTO_CMD
blx     r4

mov     r0, #0x11               @ AES_DECRYPT
mov     r4, #0xc
add     r1, r6, r4
add     r2, r6, r4
ldr     r3, [r6, #0x8]
ldr     r4, =AES_MODE_256
str     r4, [sp]
mov     r4, #0x24
add     r4, r5
str     r4, [sp, #4]
mov     r4, #0x14
add     r4, r5
str     r4, [sp, #8]
ldr     r4, =AES_CRYPTO_CMD
blx     r4

_copy_image:
ldr     r0, =LOAD_ADDRESS
mov     r1, #0xc
add     r1, r6
ldr     r2, [r6, #8]            @ header->fullSize
mov     r5, r2
bl      _memcpy

_patch:
ldr     r0, =LOAD_ADDRESS
ldr     r1, =VERIFY_SHSH
add     r1, r1, r0
ldr     r2, =INSN2_MOV_R0_0__STR_R0_R3
str     r2, [r1]

_boot_image:
ldr     r3, =GET_BOOT_TRAMPOLINE
blx     r3
mov     r1, r0
mov     r0, #0
ldr     r2, =LOAD_ADDRESS
ldr     r3, =JUMPTO
blx     r3

_request_done:
mov     r0, #0
add     sp, sp, #0x10
pop     {r4-r7,pc}

_memcpy:

_memcpy_loop:
ldrb    r3, [r1]
strb    r3, [r0]
add     r0, #1
add     r1, #1
sub     r2, #1
cmp     r2, #0
bne     _memcpy_loop

BX      lr

_find:
push    {r4, lr}

_find_loop:
ldr     r4, [r1]
cmp     r4, r0
beq     _find_ret

_find_loop_continue:
add     r1, #2
sub     r2, #2
cmp     r2, #0
bne     _find_loop

mov     r1, #0                  @ not found

_find_ret:
mov     r0, r1
pop     {r4, pc}

.align 2
