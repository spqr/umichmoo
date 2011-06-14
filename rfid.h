/* See license.txt for license information. */

#ifndef RFID_H
#define RFID_H

#include "mymoo.h"

// the bit count will be different from the spec, because we don't adjust it for
// processing frame-syncs/rtcal/trcals. however, the cmd buffer will contain pure packet
// data.
#if ENABLE_SLOTS
#define NUM_QUERY_BITS          21
#define NUM_READ_BITS           53  // 60 bits actually received, but need to break off early for computation
#define MAX_NUM_READ_BITS       60
#elif ENABLE_SESSIONS
#define NUM_QUERY_BITS          21
#define NUM_READ_BITS           53  // 60 bits actually received, but need to break off early for computation
#define MAX_NUM_READ_BITS       60
#else
#define NUM_QUERY_BITS          24
#define NUM_READ_BITS           55   // 60 bits actually received, but need to break off early for computation
#define MAX_NUM_READ_BITS       60
#endif
#define MAX_NUM_QUERY_BITS      25
#define NUM_QUERYADJ_BITS       9
#define NUM_QUERYREP_BITS       5
#define MAX_NUM_QUERYADJ_BITS   9
#define NUM_ACK_BITS            20
#define NUM_REQRN_BITS          41
#define NUM_NAK_BITS            10

volatile short state;
volatile unsigned char command;
unsigned short rn16;
unsigned int epc;
unsigned short divideRatio;
unsigned short linkFrequency;
unsigned char subcarrierNum;
unsigned char TRext;
unsigned char delimiterNotFound;
unsigned short ackReplyCRC, queryReplyCRC, readReplyCRC;
unsigned short Q;
unsigned short slot_counter;
unsigned short shift;
unsigned int read_counter;
unsigned int sensor_counter;
unsigned char timeToSample;

unsigned short inInventoryRound;
unsigned char last_handle_b0, last_handle_b1;

#define BUFFER_SIZE 16                         // max of 16 bytes rec. from reader
#define MAX_BITS (BUFFER_SIZE * 8)
#define POLY5 0x48
volatile unsigned char cmd[BUFFER_SIZE+1];          // stored cmd from reader
//volatile unsigned char reply[BUFFER_SIZE+1]= { 0x30, 0x35, 0xaa, 0xab, 0x55,0xff,0xaa,0xab,0x55,0xff,0xaa,0xab,0x55,0xff,0x00, 0x00};

volatile unsigned char queryReply[];
volatile unsigned char ackReply[];
volatile unsigned char tid[];
volatile unsigned char usermem[];
volatile unsigned char readReply[];

unsigned char RN16[23];

void sendToReader(volatile unsigned char *data, unsigned char numOfBits);
unsigned short crc16_ccitt(volatile unsigned char *data, unsigned short n);
#if 0
unsigned char crc5(volatile unsigned char *buf, unsigned short numOfBits);
#endif

/* Handlers for RFID commands */
/* XXX make these inline where appropriate, but only after restructuring
 * code so that the functions to be inlined are called from only one compilation
 * unit */
void handle_query (volatile short nextState);
void handle_queryrep (volatile short nextState);
void handle_queryadjust (volatile short nextState);
void handle_select (volatile short nextState);
void handle_ack (volatile short nextState);
void handle_request_rn (volatile short nextState);
void handle_read (volatile short nextState);
void handle_nak (volatile short nextState);
void do_nothing ();

#endif // RFID_H
