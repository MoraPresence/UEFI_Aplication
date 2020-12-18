use16
	org 1000h
	jmp start

PML4_addr equ 200000h
PDPE_addr equ 201000h
PDE_addr  equ 202000h

start:
	cli
	
	lgdt [tmp_gdtr]

	mov eax,cr0
	or al,1
	mov cr0,eax

	jmp  8:PROTECTED_MODE_ENTRY_POINT;

	use32
	PROTECTED_MODE_ENTRY_POINT:


	mov ax,16
	mov es,ax
	mov ds,ax

    ;mov eax, cr4
    ;bts eax, 5   ; PAE = 1
    ;mov cr4, eax

	mov dword [es:PDE_addr], 010000011b    ; PS or Present or Write
    mov dword [es:PDE_addr+4], 0
    mov dword [es:PDPE_addr], PDE_addr or 3    ; Present or Write
    mov dword [es:PDPE_addr+4], 0
    mov dword [es:PML4_addr], PDPE_addr or 3   ; Present or Write
    mov dword [es:PML4_addr+4], 0

    mov eax, PML4_addr
    mov cr3, eax ; base address tables in cr3

    mov ecx, 0xC0000080 ; EFER
    rdmsr
    bts eax,8  ; EFER.LME = 1
    wrmsr

    mov eax, cr0
    bts eax, 31 ; PG = 1
    mov cr0, eax
	
    jmp LM_CODE_START
	
use64
LM_CODE_START:
	mov rdi,0x80000000
	mov rcx,0x13880
	greenLineLoop:
	mov word[rdi],0xe144
	inc rdi
	inc rdi
	loop greenLineLoop
	jmp $

	 
align 8
	tmp_gdt:
	zero_seg      db  00h,  00h, 00h, 00h, 00h,       00h,       00h, 00h
	code_seg      db 0FFh, 0FFh, 00h, 00h, 00h, 10011010b, 11001111b, 00h  ; сегмент кода 32
	data_seg   	  db 0FFh, 0FFh, 00h, 00h, 00h, 10010010b, 11001111b, 00h  ; сегмент данных
	code64_seg    db  00h,  00h, 00h, 00h, 00h,       9Ah,       20h, 00h  ; сегмент кода 64
	data64_seg    db  00h,  00h, 00h, 00h, 00h,       92h,       00h, 00h  ; сегмент данных 64

tmp_gdtr:
		 .size    dw (8 * 5  - 1)
         .address dd tmp_gdt
		 