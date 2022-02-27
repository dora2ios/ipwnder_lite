.text

.pool

// used: x0, x19-x22, x30

.global _main
_main:
msr     daifset, #0x3
isb

_patch:
ldr     x10, [x0]
orr     x10, x10, #0x60000000000000
and     x10, x10, #0xffffffffffffff7f
str     x10, [x0]
dsb     sy
tlbi    alle3
dsb     sy
isb

adr     x8, INSN_NOP
ldr     w9, [x8]
str     w9, [x19]
str     w9, [x20]
str     w9, [x21]
str     w9, [x22]

and     x10, x10, #0xff9fffffffffffff
orr     x10, x10, #0x80
str     x10, [x0]
dsb     sy
tlbi    alle3
dsb     sy
isb

_end:
msr     daifclr, #0x3
isb
ret

INSN_NOP:
.long 0xd503201f
