CC=msp430-elf-gcc
AS=msp430-elf-as
MKDIR=mkdir -p
OBJDUMP=msp430-elf-objdump
OBJCOPY=msp430-elf-objcopy
MSPFLASHER=MSP430Flasher
MSPDEBUG=mspdebug
CFLAGS=-Wall -ggdb3 -DMOO_VERSION=MOO1_2_1
ALL_CFLAGS=-ffixed-R4 -ffixed-R5 -mmcu=msp430f2618 -Igcc/ $(CFLAGS)
LDFLAGS=
ALL_LDFLAGS=-Tgcc/msp430f2618.ld -nostartfiles $(LDFLAGS)
SOURCES=digital_accel_sensor.c digital_accel_moo_interface.c flash.c int_temp_sensor.c moo.c \
rfid.c build_send_to_reader.c build_port1_isr.c build_timerA1_isr.c sensor.c sensor_read.c
OUTDIR=build

OBJECTS=$(addprefix $(OUTDIR)/, $(notdir $(SOURCES:.c=.o)))
EXECUTABLE=moo-prog
HEX=$(EXECUTABLE).hex

all: $(OUTDIR)/$(HEX)

$(OUTDIR)/$(HEX): $(OUTDIR)/$(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@

$(OUTDIR)/$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(ALL_LDFLAGS) $(ALL_CFLAGS) $(OBJECTS) -o $@

$(OUTDIR)/%.o: %.c | $(OUTDIR)
	$(CC) -c $(ALL_CFLAGS) $< -o $@

clean:
	-$(RM) $(OUTDIR)/*

$(OUTDIR):
	$(MKDIR) $(OUTDIR)

.PHONY: all clean
