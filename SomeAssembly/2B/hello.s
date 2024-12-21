.global _start
        .align 2

.data
msg:    .ascii      "Hello, world!\n"
        len = . - msg


.text
_start:
    // write(1, msg, len)
    
    MOV     X0, #1
    adrp    X1, msg@PAGE // cite: https://www.reddit.com/r/asm/comments/o2152s/using_adr_in_arm_macos/
    add     X1, X1, msg@PAGEOFF // no adr
    MOV     X2, len
    MOV     X16, #4 // syscall write
    SVC     0

  
