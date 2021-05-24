
RM	   := rm -f
RMDIR  := rm -rf
MKDIR  := mkdir -p

.PHONY := all disk clean run

all:
	$(MAKE) -C bootloader/
	$(MKDIR) build
	cp bootloader/mbr.bin build/
	cp bootloader/cboot.bin build/
	
	$(MKDIR) build/rootfs
	@echo "This is a test file" > build/rootfs/test.txt

disk.img: all
	dd if=/dev/zero of=disk.img bs=1M count=256
	sfdisk ./disk.img < template.sfdisk
	dd if=./build/mbr.bin of=disk.img conv=notrunc bs=512 count=1
	dd if=./build/cboot.bin of=disk.img conv=notrunc bs=512 seek=2048	

	virt-make-fs build/rootfs/ --size=132103680 --type=ext2 rootfs.img
	dd if=./rootfs.img of=./disk.img bs=512 seek=4096

disk: disk.img

clean:
	$(RMDIR) build/
	$(RM) disk.img
	$(MAKE) -C bootloader/ -f Makefile clean;

run: disk.img
	qemu-system-x86_64 -hda disk.img