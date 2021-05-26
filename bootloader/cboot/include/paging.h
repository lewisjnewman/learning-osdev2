#ifndef PAGING_H
#define PAGING_H

#include <int_types.h>


typedef union {
    u64 raw;
    struct{
        u64 present:1;
        u64 writable:1;
        u64 user_accessible:1;
        u64 write_through_caching:1;
        u64 disable_cache:1;
        u64 accessed:1;
        u64 dirty:1;
        u64 huge_page:1;
        u64 _available1:3;
        u64 addr:42;
        u64 _available2:10;
        u64 no_execute:1;
    } __attribute__((packed)); 
} PageTableEntry;


#define SET_PTE_ADDRESS(pte, n) (pte.addr = (n>>11))
#define GET_PTE_ADDRESS(pte) (pte.addr<<11)

typedef union {
    u64 raw;
    struct{
        u64 offset:12;
        u64 l1_index:9;
        u64 l2_index:9;
        u64 l3_index:9;
        u64 l4_index:9;
        u64 sign_extension:16;
    } __attribute__((packed));
} VirtAddr;

// There are 512 page table entries in a page table
#define NUM_PAGE_TABLE_ENTRIES 512
#define PAGETABLE_SIZE (sizeof(PageTableEntry) * NUM_PAGE_TABLE_ENTRIES)

/*
BIT     NAME                    MEANING
0	    present	                the page is currently in memory
1	    writable	            it's allowed to write to this page
2	    user accessible	        if not set, only kernel mode code can access this page
3	    write through caching	writes go directly to memory
4	    disable cache	        no cache is used for this page
5	    accessed	            the CPU sets this bit when this page is used
6	    dirty	                the CPU sets this bit when a write to this page occurs
7	    huge page/null	        must be 0 in P1 and P4, creates a 1GiB page in P3, creates a 2MiB page in P2
8	    global	                page isn't flushed from caches on address space switch (PGE bit of CR4 register must be set)
9-11	available	            can be used freely by the OS
12-51	physical address	    the page aligned 52bit physical address of the frame or the next page table
52-62	available	            can be used freely by the OS
63	    no execute	            forbid executing code on this page (the NXE bit in the EFER register must be set)
*/

extern void load_newpagetable(void* pt);

extern void setup_physical_map(u64 total_memsize);

/* Memory Map Design
0x0000000000000000  - bottom of the lower half of memory
...                 - memory used for userspace processes
0x00007FFFFFFFFFFF  - top of the lower half of memory
...
...
...
0xFFFF800000000000  - bottom of the upper half of memory (physical memory map)
0xFFFF900000000000  - kernel exe map (code, rodata, data, bss sections)
0xFFFFA00000000000  - kernel heap map
0xFFFFB00000000000  - kernel stack area
...
0xFFFFFFFF80000000  - top-2GB 
0xFFFFFFFFFFFFFFFF  - top of the upper half of memory
*/

extern VirtAddr virtual_to_physical_address(u64 addr);
extern void* virtual_to_physical_pointer(void* ptr);

#endif