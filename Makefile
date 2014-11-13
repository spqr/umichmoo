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

include $(addprefix sensors/$(SENSORS)/, $(addsuffix .mk, $(SENSORS)))
SENSOR_SOURCES := $(foreach sensor,$(SENSORS), sensors/$(sensor)/$(SOURCES_$(sensor)))
SENSOR_OBJECTS := $(foreach sensor,$(SENSORS), $(OUTDIR)/sensors/$(sensor)/$(SOURCES_$(sensor):.c=.o))

OBJECTS=$(addprefix $(OUTDIR)/, $(notdir $(SOURCES:.c=.o)))
EXECUTABLE=moo-prog
HEX=$(EXECUTABLE).hex


define depend_on_dir
$(1): | $(dir $(1))
	ifndef $(dir $(1))_DIRECTORY_RULE_IS_DEFINED
		$(dir $(1)):
    			mkdir -p $$@

		$(dir $(1))_DIRECTORY_RULE_IS_DEFINED := 1
	endif
endef

$(foreach file,$($(SENSOR_OBJECTS) $(OBJECTS)),$(eval $(call depend_on_dir,$(file))))

all: $(OUTDIR)/$(HEX)

$(OUTDIR)/$(HEX): $(OUTDIR)/$(EXECUTABLE).elf
	$(OBJCOPY) -O ihex $< $@

$(OUTDIR)/$(EXECUTABLE).elf: $(OBJECTS) $(SENSOR_OBJECTS)
	$(CC) $(ALL_LDFLAGS) $(ALL_CFLAGS) $(OBJECTS) -o $@


$(SENSOR_OBJECTS) $(OBJECTS): $(OUTDIR)/%.o : %.c | $(OUTDIR) $(DEPDIR)
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
