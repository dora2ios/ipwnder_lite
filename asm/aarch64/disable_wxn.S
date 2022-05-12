.text

.align 2

.globl _main
_main:
// x0 =  0xA0000
MOV  X1, #0x180000000  // x1 = 0x180000000
ADD  X2, X1, X0        // x2 = 0x1800A0000
ADD  X1, X1,  #0x625   // x1 = 0x180000625
STR  X1, [X2, #0x600]  // *(x2 + 0x600) = x1
DMB  SY

MOV  X1, #0xC
MRS  X0, CurrentEL
CMP  X0, X1
BEQ  _SCTLR_EL3

_SCTLR_EL1:
MOV  X0, #0x100D
MSR  SCTLR_EL1, X0
DSB  SY
ISB
B    _RET

_SCTLR_EL3:
MOV  X0, #0x100D
MSR  SCTLR_EL3, X0
DSB  SY
ISB

_RET:
RET
