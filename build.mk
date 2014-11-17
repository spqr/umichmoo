CC          := msp430-elf-gcc
AS          := msp430-elf-as
MKDIR       := mkdir -p
OBJDUMP     := msp430-elf-objdump
OBJCOPY     := msp430-elf-objcopy
MSPFLASHER  := MSP430Flasher
MSPDEBUG    := mspdebug
CFLAGS      := -Wall -DMOO_VERSION=$(MOO_VERSION)
LDFLAGS     :=
ALL_CFLAGS  := -I. -std=gnu11 -ffixed-R4 -ffixed-R5 -mmcu=msp430f2618 -Igcc/ $(CFLAGS)
ALL_LDFLAGS := -Tgcc/msp430f2618.ld $(LDFLAGS)
