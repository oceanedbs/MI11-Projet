/* Fichier de démarrage ASM */

.section .vectors, "ax"
.code 32


  ldr   pc, =_start       @ reset vector
  b     .                 @ Undefined instructions
  b     .                 @ software interrupt
  b     .                 @ prefetch abort
  b     .                 @ data abort
  b     .                 @ reserved
  b     scheduler		  @ irq
  b     .                 @ fiq

.text
.code 32
.align 4

.global _start
.func   _start

_start:

  /* Clear bss */
  ldr   r0,=__bss
  ldr   r1,=__ebss
  mov   r2, #0
  b     2f
1:str   r2, [r0], #4

2:cmp   r0, r1
  blo   1b

  /* Switch to SYS mode */
  mrs   r0, cpsr
  orr   r0, r0, #0x1f
  msr   cpsr_c, r0

  /* Setup stack */
  ldr   sp, =__sys_tos

  bl	main
  b     .
.endfunc

.end
