
.PHONY = all clean

CC    := clang
LD    := ld
ASM   := nasm

RM          := rm 
RMDIR       := rm -r
MKDIR		:= mkdir -p

OBJ         := cboot/obj
INCLUDE     := cboot/include
SRC         := cboot/src
ASMSRC      := cboot/asm
CSRCS       := $(wildcard $(SRC)/*.c)
ASMSRCS     := $(wildcard $(ASMSRC)/*.asm)
COBJS       := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(CSRCS))
ASMOBJS     := $(patsubst $(ASMSRC)/%.asm,$(OBJ)/%.o,$(ASMSRCS))
OBJS        := $(COBJS) $(ASMOBJS)
CBOOTEXE    := cboot.bin
CFLAGS      := -I$(INCLUDE) -ffreestanding -nostdlib -Wall -Wextra -Werror -march=x86-64 --target=x86_64-pc-none-elf
LDFLAGS     := -Tcboot/link.ld

all: mbr.bin | $(CBOOTEXE)

clean:
	-$(RMDIR) $(OBJ)
	-$(RM) cboot.bin
	-$(RM) mbr.bin

mbr.bin: mbr/mbr.asm
	$(ASM) $< -f bin -o $@

$(CBOOTEXE): $(OBJS)
	@echo \$(OBJS) $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/%.o: $(ASMSRC)/%.asm | $(OBJ)
	$(ASM) -f elf64 $< -o $@

$(OBJ):
	$(MKDIR) $@

