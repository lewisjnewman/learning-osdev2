#include <mem.h>


void memset(void* dst, u8 val, usize size){
    u8* dst8 = (u8*)dst;

    // align to an 8 byte boundary
    while((u64)dst8 % 8 != 0 && (u64)dst8 < (u64)dst + size) {*dst8++ = val;}

    u64* dst64 = (u64*)dst8;

    // do 64bit copies
    while((u64)dst64+8 < (u64)dst+size){
        *dst64++ = val;
    }

    dst8 = (u8*)dst64;
    
    //do the final byte copies
    while((u64)dst8 < (u64)dst+size){
        *dst8++ = val;
    }
}

// returns 0 if they are equal - 1 if they are not
// (this is not a posix standards compliant implementation but oh well it will do)
int memcmp(const void* s1, const void* s2, usize size){
    u8* b1 = (u8*)s1;
    u8* b2 = (u8*)s2;

    for(usize i = 0; i < size; i++){
        if (b1[i] != b2[i]){
            return 1;
        }
    }
    return 0;
}

void memcpy(void* dst, const void* src, usize size){
    u8* dst8 = (u8*)dst;
    u8* src8 = (u8*)src;

    // align to an 8 byte boundary
    while((u64)dst8 % 8 != 0 && (u64)dst8 < (u64)dst + size) {*dst8++ = *src8++;}

    u64* dst64 = (u64*)dst8;
    u64* src64 = (u64*)src8;

    // do 64bit copies
    while((u64)dst64+8 < (u64)dst+size){
        *dst64++ = *src64++;
    }

    dst8 = (u8*)dst64;
    src8 = (u8*)src64;

    //do the final byte copies
    while((u64)dst8 < (u64)dst+size){
        *dst8++ = *src8++;
    }
}


int strncmp(const char* s1, const char* s2, usize size){
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

u64 strncpy(char* dst, const char* src, usize sz){
    u64 counter=0;
    while(*src != '\0' && counter < sz){
        *dst++ = *src++;
        counter++;
    }
    return counter;
}