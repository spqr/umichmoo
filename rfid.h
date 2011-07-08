/* See license.txt for license information. */

#ifndef RFID_H
#define RFID_H

// the bit count will be different from the spec, because we don't adjust it for
// processing frame-syncs/rtcal/trcals. however, the cmd buffer will contain
// pure packet data.
#if ENABLE_SLOTS
#define NUM_QUERY_BITS          21
#define NUM_READ_BITS           53  // 60 bits actually received, but need to
                                    // break off early for computation
#define MAX_NUM_READ_BITS       60
#elif ENABLE_SESSIONS
#define NUM_QUERY_BITS          21
#define NUM_READ_BITS           53  // 60 bits actually received, but need to
                                    // break off early for computation
#define MAX_NUM_READ_BITS       60
#else
#define NUM_QUERY_BITS          24
#define NUM_READ_BITS           55  // 60 bits actually received, but need to
                                    // break off early for computation
#define MAX_NUM_READ_BITS       60
#endif
#define MAX_NUM_QUERY_BITS      25
#define NUM_QUERYADJ_BITS       9
#define NUM_QUERYREP_BITS       5
#define MAX_NUM_QUERYADJ_BITS   9
#define NUM_ACK_BITS            20
#define NUM_REQRN_BITS          41
#define NUM_NAK_BITS            10

extern volatile short state;
extern volatile unsigned char command;
extern unsigned short rn16;
extern unsigned int epc;
extern unsigned short divideRatio;
extern unsigned short linkFrequency;
extern unsigned char subcarrierNum;
extern unsigned char TRext;
extern unsigned char delimiterNotFound;
extern unsigned short ackReplyCRC, queryReplyCRC, readReplyCRC;
extern unsigned short Q;
extern unsigned short slot_counter;
extern unsigned short shift;
extern unsigned int read_counter;
extern unsigned int sensor_counter;
extern unsigned char timeToSample;

extern unsigned short inInventoryRound;
extern unsigned char last_handle_b0, last_handle_b1;

/* XXX.  If BUFFER_SIZE is 16 instead of 32, we don't seem to parse READ
 * commands correctly in at least {SIMPLE,SENSOR_DATA_IN}_READ_COMMAND modes.
 * What is the maximum length in bytes of the READ command? */
#define CMD_BUFFER_SIZE 32 // max of 32 bytes rec. from reader
#define MAX_BITS (CMD_BUFFER_SIZE * 8)
#define POLY5 0x48
extern volatile unsigned char cmd[CMD_BUFFER_SIZE+1]; // stored cmd from reader

extern volatile unsigned char queryReply[];
extern volatile unsigned char ackReply[];
extern volatile unsigned char tid[];
extern volatile unsigned char usermem[];
extern volatile unsigned char readReply[];

extern unsigned char RN16[23];

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
