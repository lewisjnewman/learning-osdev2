#ifndef ATA_H
#define ATA_H

//sectors are usually 512 bytes
#define SECTOR_SIZE 512

#include <int_types.h>

extern void ATA_PIO_read_sectors(void* dst, u32 start, u8 num);
extern void ATA_PIO_write_sectors(void* src, u32 start, u8 num);

#endif