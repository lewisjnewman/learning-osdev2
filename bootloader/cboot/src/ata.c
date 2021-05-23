#include <ports.h>
#include <ata.h>

//PIO status
#define PIO_STATUS_BSY 0x80
#define PIO_STATUS_RDY 0x40
#define PIO_STATUS_DRQ 0x08
#define PIO_STATUS_DF 0x20
#define PIO_STATUS_ERR 0x01


void ATA_PIO_wait_BSY(){
    while(io_in8(0x1F7) & PIO_STATUS_BSY);
}

void ATA_PIO_wait_DRQ(){
    while(!(io_in8(0x1F7) & PIO_STATUS_RDY));
}

void ATA_PIO_read_sectors(void* dst, uint32_t first_sector, uint8_t num_sectors){

    //wait for the drive to finish whatever it is doing
    ATA_PIO_wait_BSY();

	io_out8(0x1F6, 0xE0 | ((first_sector >>24) & 0xF));
	io_out8(0x1F2, num_sectors);
	io_out8(0x1F3, (uint8_t) first_sector);
	io_out8(0x1F4, (uint8_t)(first_sector >> 8));
	io_out8(0x1F5, (uint8_t)(first_sector >> 16)); 
	io_out8(0x1F7, 0x20); //Send the read command

    uint16_t* dest = (uint16_t*)dst;

	for (int j =0;j<num_sectors;j++)
	{
		ATA_PIO_wait_BSY();
		ATA_PIO_wait_DRQ();
		for(int i=0;i<256;i++){
			dest[i] = io_in16(0x1F0);
        }
        dest += 256;
	}
}

void ATA_PIO_write_sectors(void* src, uint32_t first_sector, uint8_t num_sectors){
	
    ATA_PIO_wait_BSY();
	
	io_out8(0x1F6, 0xE0 | ((first_sector >>24) & 0xF));
	io_out8(0x1F2, num_sectors);
	io_out8(0x1F3, (uint8_t) first_sector);
	io_out8(0x1F4, (uint8_t)(first_sector >> 8));
	io_out8(0x1F5, (uint8_t)(first_sector >> 16)); 
	io_out8(0x1F7, 0x30); //Send the write command

    uint32_t* data = (uint32_t*)src;

	for (int j =0;j<num_sectors;j++)
	{
		ATA_PIO_wait_BSY();
		ATA_PIO_wait_DRQ();
		for(int i=0;i<256;i++)
		{
			io_out32(0x1F0, *data++);
		}
	}
}
