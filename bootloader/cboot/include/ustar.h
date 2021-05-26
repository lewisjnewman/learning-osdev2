#ifndef TAR_H
#define TAR_H

#include <int_types.h>
#include <gpt.h>

extern void ustar_list_files(GPTEntry gpte);
extern usize ustar_get_filesize(GPTEntry gpte, char* path, u64* filesize);
extern usize ustar_get_num_sectors(GPTEntry gpte, char* path, u64* sectors);
extern u64 ustar_load_file(GPTEntry gpte, char* path, u8* dst_buf);

#endif