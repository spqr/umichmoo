/* See license.txt for license information. */

#ifndef MYMOO_H
#define MYMOO_H

////////////////////////////////////////////////////////////////////////////////
// Step 1: pick an application
// simple hardcoded query-ack
#define SIMPLE_QUERY_ACK              0
// return sampled sensor data as epc. best for range.
#define SENSOR_DATA_IN_ID             1
// support read commands. returns one word of counter data
#define SIMPLE_READ_COMMAND           0
// return sampled sensor data in a read command. returns three words of accel
// data
#define SENSOR_DATA_IN_READ_COMMAND   0
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Step 1A: pick a sensor type
// If you're not using the SENSOR_DATA_IN_ID or SENSOR_DATA_IN_READ_COMMAND
// apps, then this is a don't care.
// Choices:
// use "0C" - static data - for test purposes only
#define SENSOR_NULL                   0
// use "0D" accel sensor sampled with 10-bit ADC, full RC settling time
#define SENSOR_ACCEL                  1
// use "0B" accel sensor sampled with 10-bit ADC, partial RC settling ("quick")
// for more info, see wiki: Code Examples
#define SENSOR_ACCEL_QUICK            2
// use "0F" built-in temperature sensor sampled with a 10-bit ADC
#define SENSOR_INTERNAL_TEMP          3
// use "0E" external temperature sensor sampled with a 10-bit ADC
#define SENSOR_EXTERNAL_TEMP          4
// use "0A" comm statistics
#define SENSOR_COMM_STATS             5

// Choose Active Sensor:
#define ACTIVE_SENSOR                 SENSOR_ACCEL_QUICK
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Step 2: pick a reader and moo hardware
// make sure this syncs with project target
#define MOO1_1                        0x11
#define MOO_VERSION                   MOO1_1
#define IMPINJ_READER                 1
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Step 3: pick protocol features
// The spec actually requires all these features, but as a practical matter
// supporting things like slotting and sessions requires extra power and thus
// limits range. Another factor is if you are running out of room on flash --
// e.g., you're using the flash-limited free IAR kickstart compiler -- you
// probably want to leave these features out unless you really need them.
//
// You only need ENABLE_SLOTS when you're working with more than one Moo. The
// code will run fine without ENABLE_SLOTS if you're using one Moo with
// multiple non-Moo rfid tags.
//
// ENABLE_SESSIONS is new code that hasn't been tested in a multiple reader
// environment. Also note a workaround I use in handle_queryrep to deal with
// what appears to be unexpected session values in the reader I'm using.
//
// Known issue: ENABLE_SLOTS and ENABLE_SESSIONS won't work together;
// it's probably just a matter of finding the right reply timing in handle_query
#define ENABLE_SLOTS 			0
#define ENABLE_SESSIONS			0
#define ENABLE_HANDLE_CHECKING          0 // not implemented yet ...
#if ENABLE_SESSIONS
#include "sessions.h"
#endif
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Step 4: set EPC and TID identifiers (optional)
#define MOO_ID 0x00, 0x08
#define EPC   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, \
    MOO_VERSION, MOO_ID
#define TID_DESIGNER_ID_AND_MODEL_NUMBER  0xFF, 0xF0, 0x01
////////////////////////////////////////////////////////////////////////////////

// Step 5: pick either Miller-2 or Miller-4 encoding
#define MILLER_2_ENCODING 0 // not tested ... use ayor
#define MILLER_4_ENCODING 1

#if SIMPLE_QUERY_ACK
#define ENABLE_READS                  0
#define READ_SENSOR                   0
#pragma message ("compiling simple query-ack application")
#endif
#if SENSOR_DATA_IN_ID
#define ENABLE_READS                  0
#define READ_SENSOR                   1
#pragma message ("compiling sensor data in id application")
#endif
#if SIMPLE_READ_COMMAND
#define ENABLE_READS                  1
#define READ_SENSOR                   0
#pragma message ("compiling simple read command application")
#endif
#if SENSOR_DATA_IN_READ_COMMAND
#define ENABLE_READS                  1
#define READ_SENSOR                   1
#pragma message ("compiling sensor data in read command application")
#endif

#if READ_SENSOR
  #if (ACTIVE_SENSOR == SENSOR_ACCEL_QUICK)
    #include "quick_accel_sensor.h"
  #elif (ACTIVE_SENSOR == SENSOR_ACCEL)
    #include "accel_sensor.h"
  #elif (ACTIVE_SENSOR == SENSOR_INTERNAL_TEMP)
    #include "int_temp_sensor.h"
  #elif (ACTIVE_SENSOR == SENSOR_EXTERNAL_TEMP)
	#error "SENSOR_EXTERNAL_TEMP not yet implemented"
  #elif (ACTIVE_SENSOR == SENSOR_NULL)
    #include "null_sensor.h"
  #elif (ACTIVE_SENSOR == SENSOR_COMM_STATS)
	#error "SENSOR_COMM_STATS not yet implemented"
  #endif
#endif

#endif // MYMOO_H
