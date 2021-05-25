#include <mem.h>

void memset(void* dst, u8 val, usize size){
    u8* dst_it = (u8*)dst;

    for(usize i = 0; i < size; i++){
        dst_it[i] = val;
    }
}

// returns 0 if they are equal - 1 if they are not
// (this is not a posix standards compliant implementation but oh well it will do)
int memcmp(void* s1, void* s2, usize size){
    u8* b1 = (u8*)s1;
    u8* b2 = (u8*)s2;

    for(usize i = 0; i < size; i++){
        if (b1[i] != b2[i]){
            return 1;
        }
    }
    return 0;
}

void memcpy(void* dst, void* src, usize size){
    u8* dst_it = (u8*)dst;
    u8* src_it = (u8*)src;

    for(usize i = 0; i < size; i++){
        dst_it[i] = src_it[i];
    }
}


int strncmp(char* s1, char* s2, usize size){
    while ( size && *s1 && ( *s1 == *s2 ) )
    {
        ++s1;
        ++s2;
        --size;
    }
    if ( size == 0 )
    {
        return 0;
    }
    else
    {
        return ( *(u8*)s1 - *(u8*)s2 );
    }
}