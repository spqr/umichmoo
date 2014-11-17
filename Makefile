include config.mk
include build.mk
SOURCES := flash.c moo.c rfid.c build_send_to_reader.c build_port1_isr.c timerA1_isr.c sensor.c sensor_read.c
ifeq ($(TARGET),debug)
	CFLAGS+=-ggdb3
endif
TARGET_FULL=$(TARGET)-$(MOO_VERSION)
OUTDIR_BASE=build
OUTDIR=$(OUTDIR_BASE)/$(TARGET_FULL)
DEPDIR_BASE=.dep
DEPDIR=$(DEPDIR_BASE)/$(TARGET_FULL)

define include_sensors
SOURCES_SAVE := $$(SOURCES)
SOURCES :=
d := sensors/$(strip $(1))
include $(addsuffix /Rules.mk, $$(d))
SENSOR_SOURCES += $$(foreach file, $$(SOURCES), $$(d)/$$(file))
SOURCES := $$(SOURCES_SAVE)
endef 

### Include sensor code ###
SENSOR_SOURCES :=
$(foreach sensor, $(SENSORS), $(eval $(call include_sensors, $(sensor))))
SENSOR_OBJECTS := $(SENSOR_SOURCES:%.c=$(OUTDIR)/%.o)
### End sensor code ###

OBJECTS=$(addprefix $(OUTDIR)/, $(notdir $(SOURCES:.c=.o)))
EXECUTABLE=moo-prog
HEX=$(EXECUTABLE).hex

all: $(OUTDIR)/$(HEX)

$(OUTDIR)/$(HEX): $(OUTDIR)/$(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@

$(OUTDIR)/$(EXECUTABLE).elf: $(OBJECTS) $(SENSOR_OBJECTS)
	$(CC) $(ALL_LDFLAGS) $(ALL_CFLAGS) $(OBJECTS) -o $@


$(SENSOR_OBJECTS) $(OBJECTS): $(OUTDIR)/%.o : %.c
	@$(MKDIR) $(dir $@)
	@$(MKDIR) $(dir $(DEPDIR)/$*.d)
	$(CC) -c -MD -MP -MF '$(DEPDIR)/$*.d' $(ALL_CFLAGS) $< -o $@

clean:
	-$(RM) -r $(OUTDIR)/*
	-$(RM) -r $(DEPDIR)/*

nuke:
	-$(RM) -r $(OUTDIR_BASE)/*
	-$(RM) -r $(DEPDIR_BASE)/*

-include $(addprefix $(DEPDIR)/, $(notdir $(OBJECTS:.o=.d)))

.PHONY: all clean
