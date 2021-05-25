#include <ustar.h>
#include <ata.h>
#include <vga.h>
#include <mem.h>

u64 oct2bin(unsigned char *str, u64 size) {
    u64 result = 0;
    for(usize i = 0; i < size; i++){
        result *= 8;
        result += str[i] - '0';
    }
    return result;
}

//USTAR FILE TYPES
#define NORMAL_FILE '0'
#define HARD_LINK '1'
#define SYM_LINK '2'
#define CHAR_DEVICE '3'
#define BLOCK_DEVICE '4'
#define DIRECTORY '5'
#define NAMED_PIPE '6'

typedef struct{
    char filename[100];
    u64 filemode;
    u64 owner_uid;
    u64 owner_gid;
    char filesize[12];
    char last_modified[12];
    u64 checksum;
    u8 type;
    char linkname[100];
    char signature[6];
    char version[2];
    char owner_username[32];
    char owner_groupname[32];
    u64 dev_major;
    u64 dev_minor;
    char filename_prefix[155];
} __attribute__((packed)) USTAR_MetadataSector;

void ustar_list_files(GPTEntry gpte) {
    char buf[512];
    memset(buf, 0, 512);
    USTAR_MetadataSector* mdata = (USTAR_MetadataSector*)buf;

    u64 seeker = 0;

    while(1){
        // this should be a tar metadata sector
        ATA_PIO_read_sectors(buf, seeker+gpte.starting_lba, 1);

        int result = memcmp(buf+257, "ustar", 5);
        if (result != 0){
            break;
        }

        u32 filesize = oct2bin((u8*)buf + 0x7c, 11);
        u32 num_sectors = ((filesize + 511) / 512);

        putc('/');
        puts(mdata->filename);

        switch (mdata->type)
        {
        case '\0':
        case NORMAL_FILE:
            puts(" :file\n");
            seeker += 1;
            seeker += num_sectors;
            break;
        case HARD_LINK:
            puts(" :link\n");
            break;
        case SYM_LINK:
            puts(" :symlink\n");
            break;
        case CHAR_DEVICE:
            puts(" :character device\n");
            break;
        case BLOCK_DEVICE:
            puts(" :block device\n");
            break;
        case DIRECTORY:
            puts(" :directory\n");
            seeker += 1;
            break;
        case NAMED_PIPE:
            puts(" :named pipe\n");
            break;
        default:
            puts(" :unknown type: ");
            putx8(mdata->type);
            putc('\n');
            break;
        }

    }
}