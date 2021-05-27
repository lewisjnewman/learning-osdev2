BITS 16
org 0x7C00

    cli

    ; setup all the segment registers
    xor ax, ax
    mov fs, ax
    mov es, ax
    mov gs, ax
    mov es, ax

    ; setup the stack just below where we
    mov ss, ax
    mov sp, 0x7BF0

    jmp 0:start


start:
    sti

    ; save the drive number for later
    mov [drive_num], dl
    
    ; set the video mode
    xor ah, ah
    mov al, 0x03
    int 0x10

    ; disable the cursor
    mov ah, 0x01
    mov ch, 0x3f
    int 0x10

    mov si, str_hi
    call puts


    ; load the second stage of the bootloader at the address 0x8000
    mov ah, 0x42
    mov dl, [drive_num]
    mov si, dap
    int 0x13
    jc error

    ; get the bios memory map and save it somewhere - we can't do this later once we leave real mode
    call bios_mem_map

    ; enable the a20 address line so memory access above 1MB is correct
    mov ax, 0x2401
    int 0x15
    jc error

    cli

    ; enter 32bit protected mode with a 32bit gdt and longjmp to cboot to set the code descriptor
    lgdt [gdt_pointer]
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    ;reset all the segment registers
    mov eax, DATA_SEG
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    ;jump to the cboot program
    jmp CODE_SEG:end

    cli 
    hlt
    

;loads data from the bios memory map function into the start of the 30kb memory space starting at 0x0500
bios_mem_map:
    pusha

    mov di, 0x0500
    xor ebx, ebx

.loop:

    mov eax, 0x0000E820
    mov ecx, 24
    mov edx, 0x534D4150

    int 0x15
    jc error
    or ebx, ebx
    jz .end
    add di, 24
    mov DWORD [di+16], 1
    jmp .loop
.end:

    mov ax, 0x0000
    mov es, ax

    popa
    ret


error:
    mov si, str_err
    call puts
    cli
    hlt


;function that expects si to be a pointer to a null terminated string
puts:
    pusha
    mov ah, 0x0e    ;write character at cursor position

.loop:
    lodsb           ;load byte pointed to by ds:si into al
    or al,al        ;check for null byte
    jz .end         ;if there is a null byte then goto end
    int 0x10        ;bios interrupt 0x10 - video stuff
    jmp .loop
.end:
    ;get current cursor position
    mov ah, 0x03
    mov bh, 0x00
    int 0x10

    ;adjust cursor position to newline
    inc dh
    mov dl, 0

    ;set cursor position
    mov ah, 0x02
    int 0x10

    popa
    ret    

;   section which stores variables

    str_err: db "boot err", 0
    str_hi: db "boot hi", 0

    drive_num: db 0

; disk address packaet
dap:
.size    db 0x10         ; size of the disk address packate
         db 0            ; unused field
.sectors dw 128          ; number of sectors to read
.offset  dw 0x0000       ; the offset inside the segment of where to read
.segment dw 0x1000       ; the segment where to start writing the data to
.start   dq 2048         ; start with sector 2048

gdt_start:
    dq 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:


gdt_pointer:
    dw gdt_end - gdt_start
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

BITS 32
end:
    jmp 0x10000

times 510 - ($-$$) db 0
dw 0xAA55