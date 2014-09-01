CC=msp430-elf-gcc
CFLAGS=-ffixed-R4 -ffixed-R5 -c -Wall -mmcu=msp430f2618 -Igcc/msp430_gcc_support_files
LDFLAGS=-Tgcc/msp430_gcc_support_files/msp430f2618.ld
SOURCES=digital_accel_sensor.c digital_accel_moo_interface.c flash.c int_temp_sensor.c moo.c \
rfid.c build_send_to_reader.c build_port1_isr.c build_timerA1_isr.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=moo-prog

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o

.SUFFIXES:
.SUFFIXES: .c .o
