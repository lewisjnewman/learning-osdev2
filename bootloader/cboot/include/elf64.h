#ifndef ELF64_H
#define ELF64_H

#include <int_types.h>

typedef struct {
    u8 magic[4];                
    u8 version;                                 
    u8 endianess;               
    u8 header_version;          
    u8 os_abi;                  
    u8 _unused[8];
    u16 resc;
    u16 instruction_set;
    u32 elf_version;
    u64 entry_point;
    u64 program_header_table_position;
    u64 section_header_table_position;
    u32 flags;
    u16 header_size;
    u16 program_header_table_entry_size;
    u16 program_header_table_entry_count;
    u16 section_header_table_entry_size;
    u16 section_header_table_entry_count;
    u16 index_section_header_table;
} __attribute__((packed)) ELF64FileHeader;

typedef struct {
    u32 type;
    u32 flags;
    u64 offset;
    u64 vaddr;
    u64 paddr;
    u64 filesz;
    u64 memsz;
    u64 align;
} __attribute__((packed)) ELF64ProgramHeader;

#endif