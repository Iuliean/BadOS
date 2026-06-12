.intel_syntax noprefix

.global cpu_info
cpu_info:
    mov eax, edi
    mov ecx, esi

    cpuid

    mov rsi, 0x0
    or  rsi, rax
    shl rbx, 32
    or  rsi, rbx
    mov rax, rsi

    shl rdx, 32
    or rdx, rcx

    ret

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

    ret
