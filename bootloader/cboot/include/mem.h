#ifndef MEM_H
#define MEM_H

#include <int_types.h>

extern void memset(void* dst, u8 val, usize size);

// returns 0 if they are equal - 1 if they are not
// (this is not a posix standards compliant implementation but oh well it will do)
extern int memcmp(const void* s1, const void* s2, usize size);

extern void memcpy(void* dst, const void* src, usize size);

// returns 0 if they are equal - 1 if they are not
// (this is not a posix standards compliant implementation but oh well it will do)
extern int strncmp(const char* s1, const char* s2, usize size);

extern u64 strncpy(char* dst, const char* src, usize size);

#endif