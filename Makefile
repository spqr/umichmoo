MOO_VERSION=MOO1_2_1
TARGET=debug

CC=msp430-elf-gcc
AS=msp430-elf-as
MKDIR=mkdir -p
OBJDUMP=msp430-elf-objdump
OBJCOPY=msp430-elf-objcopy
MSPFLASHER=MSP430Flasher
MSPDEBUG=mspdebug
CFLAGS=-Wall -DMOO_VERSION=$(MOO_VERSION)
LDFLAGS=
SOURCES=digital_accel_sensor.c digital_accel_moo_interface.c flash.c int_temp_sensor.c moo.c \
rfid.c build_send_to_reader.c build_port1_isr.c timerA1_isr.c sensor.c sensor_read.c

ifeq ($(TARGET),debug)
	CFLAGS+=-ggdb3
endif
ALL_CFLAGS=-ffixed-R4 -ffixed-R5 -mmcu=msp430f2618 -Igcc/ $(CFLAGS)
ALL_LDFLAGS=-Tgcc/msp430f2618.ld $(LDFLAGS)
TARGET_FULL=$(TARGET)-$(MOO_VERSION)
OUTDIR_BASE=build
OUTDIR=$(OUTDIR_BASE)/$(TARGET_FULL)
DEPDIR_BASE=.dep
DEPDIR=$(DEPDIR_BASE)/$(TARGET_FULL)
MAKEDEPEND=$(CC) -MD $(ALL_CFLAGS) -o $(DEPDIR)/$(TARGET_FULL)/$*.d $<

OBJECTS=$(addprefix $(OUTDIR)/, $(notdir $(SOURCES:.c=.o)))
EXECUTABLE=moo-prog
HEX=$(EXECUTABLE).hex

all: $(OUTDIR)/$(HEX)

$(OUTDIR)/$(HEX): $(OUTDIR)/$(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@

$(OUTDIR)/$(EXECUTABLE).elf: $(OBJECTS)
	$(CC) $(ALL_LDFLAGS) $(ALL_CFLAGS) $(OBJECTS) -o $@


$(OUTDIR)/%.o: %.c | $(OUTDIR) $(DEPDIR)
	$(CC) -c -MD -MP -MF '$(DEPDIR)/$*.d' $(ALL_CFLAGS) $< -o $@

clean:
	-$(RM) $(OUTDIR)/*
	-$(RM) $(DEPDIR)/*

nuke:
	-$(RM) -r $(OUTDIR_BASE)/*
	-$(RM) -r $(DEPDIR_BASE)/*

$(OUTDIR):
	$(MKDIR) $(OUTDIR)

$(DEPDIR):
	$(MKDIR) $(DEPDIR)

-include $(addprefix $(DEPDIR)/, $(notdir $(OBJECTS:.o=.d)))

.PHONY: all clean
