.intel_syntax noprefix

.global is_pml5_supported
is_pml5_supported:

    push rbx
    mov eax,0x7

    cpuid
    and ecx, 0x10
    mov eax, ecx
    shl eax, 16
    pop rbx

    ret

.global load_gdt_from
load_gdt_from:
    push rbp
    mov rbp, rsp
    sub rsp, 0x8

    lgdt [rdi]

reload_cs:
    push 0b0000000000001000
    lea rax, [reload_segments]
    push rax
    retfq

reload_segments:
    mov ax, 0b0000000000010000
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rbp
    ret
