.text

.syntax    unified

.pool
.set loadaddr,                      0xBAD00001
.set aes_crypto_cmd,                0xBAD00002
.set platform_get_boot_trampoline,  0xBAD00004
.set prepare_and_jump,              0xBAD00005

.set VERIFY_SHSH,                   0xBAD00003


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
bne.n   _jump_back

push    {r4-r7,lr}
add     r7, sp, #0xC
sub     sp, sp, #0x10

mov     r4, r0
ldr     r5, =loadaddr

movw    r1, #0xFFFF
ldrh    r2, [r4,#2]
cmp     r1, r2
bne.n   _request_done

ldrd    r0, r1, [r5]

_check_img3:
movw    r2, #0x6733
movt    r2, #0x496d             @ r2 = '3GMI'
cmp     r0, r2
bne.n   _request_done

_check_ibss:
movw    r2, #0x7373
movt    r2, #0x6962             @ r2 = 'ssbi'
ldr     r0, [r5, #0x10]
cmp     r0, r2
bne.n   _request_done

movw    r0, #0x5045
movt    r0, #0x5459             @ r0 = 'EPYT'
ldr     r1, =loadaddr
ldr     r2, [r1, #4]            @ header->fullSize
bl      _find
cmp     r0, #0
beq.n   _request_done
mov     r1, r0
movw    r2, #0x7373
movt    r2, #0x6962             @ r2 = 'ssbi'
ldr     r0, [r1, #0xc]
cmp     r0, r2
bne.n   _request_done

_decrypt_image:                 @ always encrypted images
movw    r0, #0x5441
movt    r0, #0x4441             @ r0 = 'ATAD'
ldr     r1, =loadaddr
ldr     r2, [r1, #4]            @ header->fullSize
bl      _find
cmp     r0, #0
beq.n   _request_done
mov     r6, r0

movw    r0, #0x4147
movt    r0, #0x4b42             @ r0 = 'GABK'
ldr     r1, =loadaddr
ldr     r2, [r1, #4]            @ header->fullSize
bl      _find
cmp     r0, #0
beq.n   _request_done
mov     r5, r0

movs    r0, #0x11               @ AES_DECRYPT
movs    r4, #0x14
adds    r1, r5, r4
adds    r2, r5, r4
movs    r3, #0x30               @ KBAG_KEY_IV_SIZE
mov     r4, #0x20000000
add     r4, r4, #0x200          @ AES_TYPE_GID
str     r4, [sp]
movs    r4, #0
str     r4, [sp, #4]
movs    r4, #0
str     r4, [sp, #8]
ldr     r4, =aes_crypto_cmd
blx     r4

movs    r0, #0x11               @ AES_DECRYPT
movs    r4, #0xc
adds    r1, r6, r4
adds    r2, r6, r4
ldr     r3, [r6, #0x8]
mov     r4, #0x20000000         @ AES_MODE_256
str     r4, [sp]
movs    r4, #0x24
adds    r4, r5
str     r4, [sp, #4]
movs    r4, #0x14
adds    r4, r5
str     r4, [sp, #8]
ldr     r4, =aes_crypto_cmd
blx     r4

_copy_image:
ldr     r0, =loadaddr
movs    r1, #0xc
adds    r1, r6
ldr     r2, [r6, #8]            @ header->fullSize
mov     r5, r2
bl      _memcpy

_check_ibssImage:
ldr     r0, =loadaddr
ldr     r1, [r0, #0x200]
movw    r2, #0x4269
movt    r2, #0x5353             @ r2 = 'iBSS' (check str: 'iBSS for n42ap, ...')
cmp     r1, r2
bne.n   _request_done

_patch:
ldr     r0, =loadaddr
ldr     r1, =VERIFY_SHSH
adds    r1, r1, r0
movw    r2, #0x2000
movt    r2, #0x6018             @ INSN2_MOV_R0_0__STR_R0_R3
str     r2, [r1]

_boot_image:
ldr     r3, =platform_get_boot_trampoline
blx     r3
mov     r1, r0
movs    r0, #0
ldr     r2, =loadaddr
ldr     r3, =prepare_and_jump
blx     r3

_loop:
b.n     _loop                   @ never return

_request_done:
movs    r0, #0
add     sp, sp, #0x10
pop     {r4-r7,pc}

_memcpy:

_memcpy_loop:
ldrb    r3, [r1]
strb    r3, [r0]
adds    r0, #1
adds    r1, #1
subs    r2, #1
cmp     r2, #0
bne.n   _memcpy_loop

bx      lr

_find:
push    {r4, lr}

_find_loop:
ldr     r4, [r1]
cmp     r4, r0
beq.n   _find_ret

_find_loop_continue:
adds    r1, #2
subs    r2, #2
cmp     r2, #0
bne.n   _find_loop

movs    r1, #0                  @ not found

_find_ret:
mov     r0, r1
pop     {r4, pc}

.align 2
