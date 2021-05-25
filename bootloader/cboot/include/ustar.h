#ifndef TAR_H
#define TAR_H

#include <int_types.h>
#include <gpt.h>

typedef struct {
    u8* databuffer;
    u64 filesize;
} File;

extern void ustar_list_files(GPTEntry gpte);

#endif