
RM	   := rm -f
RMDIR  := rm -rf
MKDIR  := mkdir -p

.PHONY := all kernel disk clean run

.FORCE:


all: kernel
	$(MAKE) -C bootloader/
	$(MAKE) -C kernel
	$(MKDIR) build
	cp bootloader/mbr.bin build/
	cp bootloader/cboot.bin build/
	cp kernel/target/x86_64/debug/kernel build/kernel.elf
	strip --strip-debug build/kernel.elf

	$(MKDIR) build/rootfs
	$(MKDIR) build/rootfs/sys
	$(MKDIR) build/rootfs/cfg
	$(MKDIR) build/rootfs/bin
	$(MKDIR) build/rootfs/usr
	@echo "This is a test file" > build/rootfs/test.txt
	@echo "Hello World" > build/rootfs/hello.txt
	@dd if=/dev/urandom bs=300 count=1 > build/rootfs/blob1.dat
	@dd if=/dev/urandom bs=300 count=1 > build/rootfs/blob2.dat
	@dd if=/dev/urandom bs=300 count=1 > build/rootfs/bin/blob3.dat
	@dd if=/dev/urandom bs=300 count=1 > build/rootfs/sys/blob4.dat
	@cp build/kernel.elf build/rootfs/sys/kernel.elf

disk.img: all
	dd if=/dev/zero of=disk.img bs=1M count=256
	sfdisk ./disk.img < template.sfdisk
	dd if=./build/mbr.bin of=disk.img conv=notrunc bs=512 count=1
	dd if=./build/cboot.bin of=disk.img conv=notrunc bs=512 seek=2048	


	cd ./build/rootfs; tar --format=ustar -cf ../../rootfs.tar *
	dd if=./rootfs.tar of=./disk.img bs=512 seek=4096

disk: disk.img

clean:
	$(RMDIR) build/
	$(RM) disk.img
	$(MAKE) -C bootloader/ -f Makefile clean;
	$(MAKE) -C kernel/ -f Makefile clean;

run: disk.img
	qemu-system-x86_64 -hda disk.img -m 8G -no-reboot \
	-chardev stdio,id=char0,logfile=serial.log,signal=off -serial chardev:char0