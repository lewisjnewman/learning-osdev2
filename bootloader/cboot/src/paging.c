#include <paging.h>
#include <mem.h>
#include <vga.h>

void load_newpagetable(void* pt){
    u64 temp = (u64)pt;

    // set the cr3 register (also flushes the translation lookaside buffer)
    __asm__ __volatile__("movq %0, %%cr3"::"b"(temp));
}


/*
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

// level 3 page table used for mapping the physical address space into the upper area of memory
PageTableEntry p3table_physical[512];

// sets up the page tables for mapping 0xFFFF800000000000 to 0x0000000000000000 using 1GB huge pages
void setup_physical_map(u64 total_memsize){
    // The p4table has already been setup at 0x1000
    PageTableEntry* p4table = (PageTableEntry*)0x1000;

    // we are going to setup a p3 table for the upper memory addresses just after the p2 table
    PageTableEntry* p3table = (PageTableEntry*)0x6000;

    // clear the entire page table
    memset(p3table, 0, PAGETABLE_SIZE);

    // work out the total size of physical memory in gigabytes
    usize num_gigs = (total_memsize+0x3fffffff)/(0x40000000);

    // what virtual addresses we are going to start mapping from
    VirtAddr virtaddr;
    virtaddr.raw = 0xFFFF800000000000;

    for(usize i = 0; i < num_gigs; i++){
        // Create a new page table entry
        PageTableEntry new_entry;
        new_entry.raw = 0;

        // Set the page table entry address and mark it as writable, present and as a huge page (1GB)
        SET_PTE_ADDRESS(new_entry, i*0x40000000);
        new_entry.writable = 1;
        new_entry.present = 1;
        new_entry.huge_page = 1;

        // save the new entry in the level 3 page table
        p3table[virtaddr.l3_index] = new_entry;

        // increment virtaddr by 1GB to point to the next region we want to map
        virtaddr.raw += 0x40000000;
    }

    PageTableEntry p3table_entry;
    p3table_entry.raw = 0;
    SET_PTE_ADDRESS(p3table_entry, (u64)p3table);
    p3table_entry.present = 1;
    p3table_entry.writable = 1;

    p4table[virtaddr.l4_index] = p3table_entry;

    load_newpagetable(p4table);

    //test to see if this works
    int neq = memcmp((void*)0x0000000000000000, (void*)0xFFFF800000000000, 1048576);
    if(neq){
        puts("Page Table mapping is suspiciously incorrect!\n");
        __asm__ __volatile__("hlt");
    } else {
        puts("Physical Page Table mapping is correct\n");
    }
}

void print_pagetable(PageTableEntry* table, usize level){
    for(usize i = 0; i < NUM_PAGE_TABLE_ENTRIES; i++){
        PageTableEntry pte = table[i];

        if(!pte.present){
            continue;
        }
        //else

        puts("PT");
        putd(level);
        putc('[');
        putd(i);
        putc(']');
        puts(" = ");
        putx64(pte.raw);
        puts("; addr = ");
        putx64(GET_PTE_ADDRESS(pte));
        putc('\n');

        if(level==4){
            PageTableEntry* subtable = (PageTableEntry*)GET_PTE_ADDRESS(pte);
            print_pagetable(subtable, level-1);
        } else if((level==2 || level==3) && !pte.huge_page){
            PageTableEntry* subtable = (PageTableEntry*)GET_PTE_ADDRESS(pte);
            print_pagetable(subtable, level-1);
        }
    }
}

VirtAddr virtual_to_physical_address(u64 addr){
    // we can rely on the fact that the entire physical memory region is going to be 
    // mapped to the upper memory region starting at 0xFFFF800000000000
    VirtAddr va;
    va.raw = addr | 0xFFFF800000000000;
    return va;
}

void* virtual_to_physical_pointer(void* ptr){
    return (void*)virtual_to_physical_address((u64)ptr).raw;
}
