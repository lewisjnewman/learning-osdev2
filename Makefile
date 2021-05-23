
RM	   := rm -f
RMDIR  := rm -rf
MKDIR  := mkdir -p

.PHONY := all disk clean run

all:
	$(MAKE) -C bootloader/
	$(MKDIR) build
	cp bootloader/mbr.bin build/
	cp bootloader/cboot.bin build/

disk.img: all
	dd if=/dev/zero of=disk.img bs=1M count=256
	sfdisk ./disk.img < template.sfdisk
	dd if=./build/mbr.bin of=disk.img conv=notrunc bs=512 count=1
	dd if=./build/cboot.bin of=disk.img conv=notrunc bs=512 seek=34	

# 	TODO - create a filesystem and place the kernel inside it and copy it onto the disk as well
	

disk: disk.img

clean:
	$(RMDIR) build/
	$(RM) disk.img
	$(MAKE) -C bootloader/ -f Makefile clean;

run: disk.img
	qemu-system-x86_64 -hda disk.img