#ifndef BOOT_STRUCT_H
#define BOOT_STRUCT_H

#include <int_types.h>
#include <bios_memory_map.h>

// Struct that is passed to kmain to give information to the kernel about the setup

typedef struct {
    BiosMemoryRegion* bios_memory_map_address;
    u64 kernel_paddr;       // kernel physical memory address
    u64 kernel_size;        // the size of the kernel
} BootInfo;

#endif