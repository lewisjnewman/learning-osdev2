#ifndef BIOS_MEMORY_MAP_H
#define BIOS_MEMORY_MAP_H

#include <int_types.h>

typedef struct{
    u64 base_address;
    u64 map_size;
    u32 type;
    u32 acpi3;
} __attribute__((__packed__)) bios_memmap;


//types of maps
#define BIOS_MAP_FREE_MEMORY    0x01
#define BIOS_MAP_RESERVED       0x02
#define BIOS_MAP_ACPI_RECLAIM   0x03
#define BIOS_MAP_ACPI_NVS       0x04
#define BIOS_BAD_MEMORY         0x05

//the master boot record places the bios memory map at this address
#define BIOS_MEMORY_MAP_LOCATION 0x0500

#endif