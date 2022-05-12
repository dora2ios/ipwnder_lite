.text

.align 2

.global _main
_main:
  ldr   w1, [x0]            // x0 = gDemotionRegister
  and   w1, w1, #0xfffffffe
  str   w1, [x0]
  dsb   sy
  isb
  ret

