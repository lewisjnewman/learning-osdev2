#ifndef GPT_H
#define GPT_H

#include <int_types.h>

typedef struct{
    u8 signature[8];
    u32 revision;
    u32 header_size;
    u32 crc32_of_header;
    u32 _reserved;
    u64 lba_of_header;
    u64 lba_of_alternate;
    u64 first_usable_block;
    u64 last_usable_block;
    u8 guid[16];
    u64 partition_array_lba;
    u32 number_of_entries;
    u32 sizeof_partition_entry;
    u32 crc32_of_partition_array;
} __attribute__((packed)) GPTHeader;

typedef struct{
    u8 partition_type_guid[16];
    u8 unique_partition_guid[16];
    u64 starting_lba;
    u64 ending_lba;
    u64 attribute;
    u16 partition_name[32]; // This could however vary
} __attribute__((packed)) GPTEntry;

#endif