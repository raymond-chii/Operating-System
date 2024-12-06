.text
.align 2
.globl _tas

_tas:
    mov     w8, #1              // Load immediate value 1
loop:
    ldaxrb  w9, [x0]           // Load exclusive byte
    stlxrb  w10, w8, [x0]      // Store exclusive byte
    cbnz    w10, loop          // If store failed, retry
    mov     w0, w9             // Move old value to return register
    ret                        // Return