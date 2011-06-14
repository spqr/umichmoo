/* See license.txt for license information. */

//******************************************************************************
//  Moo 1.1
//  Device: MSP430-2618
//
//  Version HW1.1_SW1.1
//
//  Pinout:
//             P1.1 = Data out signal
//             P1.2 = Data in signal
//             P1.3 = Data in enable
//
//             P2.0 = Supervisor In
//             P4.0 = Supervisor In
//
//	       Tested against Impinj v 3.0.2 reader, Miller-4 encodings
//	       Alien reader code is out of date
//
//	This is a partial implementation of the RFID EPCGlobal C1G2 protocol
//	for the Moo 1.1 hardware platform.
//	
//	What's missing: 
//        - SECURED and KILLED states.
//        - No support for WRITE, KILL, LOCK, ACCESS, BLOCKWRITE and BLOCKERASE commands.
//        - Commands with EBVs always assume that the field is 8 bits long.
//        - SELECTS don't support truncation.
//        - READs ignore membank, wordptr, and wordcount fields. (What READs do
//          return is dependent on what application you have configured in step 1.)
//        - I sometimes get erroneous-looking session values in QUERYREP commands.
//          For the time being, I just parse the command as if the session value
//          is the same as my previous_session value.
//        - QUERYs use a pretty aggressive slotting algorithm to preserve power.
//          See the comments in that function for details.
//        - Session timeouts work differently than what's in table 6.15 of the spec.
//          Here's what we do:
//            SL comes up as not asserted, and persists as long as it's in ram retention
//            mode or better.
//            All inventory flags come up as 'A'. S0's inventory flag persists as
//            long as it's in ram retention mode or better. S1's inventory flag persists
//            as long as it's in an inventory round and in ram retention mode or better;
//            otherwise it is reset to 'A' with every reset at the top of the while
//            loop. S2's and S3's inventory flag is reset to 'A' with every reset at
//            the top of the while loop.
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Step 1: pick an application
// simple hardcoded query-ack
#define SIMPLE_QUERY_ACK              0
// return sampled sensor data as epc. best for range.
#define SENSOR_DATA_IN_ID             1
// support read commands. returns one word of counter data
#define SIMPLE_READ_COMMAND           0
// return sampled sensor data in a read command. returns three words of accel data
#define SENSOR_DATA_IN_READ_COMMAND   0
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Step 1A: pick a sensor type
// If you're not using the SENSOR_DATA_IN_ID or SENSOR_DATA_IN_READ_COMMAND apps,
// then this is a don't care.
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
// e.g., you're using the flash-limited free IAR kickstart compiler -- you probably
// want to leave these features out unless you really need them.
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
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Step 4: set EPC and TID identifiers (optional)
#define MOO_ID 0x00, 0x08
#define EPC   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, MOO_VERSION, MOO_ID
#define TID_DESIGNER_ID_AND_MODEL_NUMBER  0xFF, 0xF0, 0x01
////////////////////////////////////////////////////////////////////////////////

#if(MOO_VERSION == MOO1_1)
  #include <msp430x26x.h>
  #include "pins.h"
  #define USE_2618  1
#else
  #error "Moo version not supported"
#endif

#if SIMPLE_QUERY_ACK
#define ENABLE_READS                  0
#define READ_SENSOR                   0
#warning "compiling simple query-ack application"
#endif
#if SENSOR_DATA_IN_ID
#define ENABLE_READS                  0
#define READ_SENSOR                   1
#warning "compiling sensor data in id application"
#endif
#if SIMPLE_READ_COMMAND
#define ENABLE_READS                  1
#define READ_SENSOR                   0
#warning "compiling simple read command application"
#endif
#if SENSOR_DATA_IN_READ_COMMAND
#define ENABLE_READS                  1
#define READ_SENSOR                   1
#warning "compiling sensor data in read command application"
#endif

// pick only one
#define MILLER_2_ENCODING             0             // not tested ... use ayor
#define MILLER_4_ENCODING             1

// as per mapping in monitor code
#define moo_debug_1                  DEBUG_1_5   // P1.5
#define moo_debug_2                  DEBUG_2_1   // P2.1
#define moo_debug_3                  CLK_A       // P3.0
#define moo_debug_4                  TX_A        // P3.4
#define moo_debug_5                  RX_A        // P3.5
#define MONITOR_DEBUG_ON                 0

// ------------------------------------------------------------------------

#define SEND_CLOCK  \
  BCSCTL1 = XT2OFF + RSEL3 + RSEL0 ; \
    DCOCTL = DCO2 + DCO1 ;
  //BCSCTL1 = XT2OFF + RSEL3 + RSEL1 ; \
  //DCOCTL = 0;
#define RECEIVE_CLOCK \
  BCSCTL1 = XT2OFF + RSEL3 + RSEL1 + RSEL0; \
  DCOCTL = 0; \
  BCSCTL2 = 0; // Rext = ON

#define BUFFER_SIZE 16                         // max of 16 bytes rec. from reader
#define MAX_BITS (BUFFER_SIZE * 8)
#define POLY5 0x48
volatile unsigned char cmd[BUFFER_SIZE+1];          // stored cmd from reader
//volatile unsigned char reply[BUFFER_SIZE+1]= { 0x30, 0x35, 0xaa, 0xab, 0x55,0xff,0xaa,0xab,0x55,0xff,0xaa,0xab,0x55,0xff,0x00, 0x00};
volatile unsigned char* destorig = &cmd[0];         // pointer to beginning of cmd

// #pragma data_alignment=2 is important in sendResponse() when the words are copied into arrays.
// Sometimes the compiler puts reply[0] on an
// odd address, which cannot be copied as a word and thus screws everything up.
#pragma data_alignment=2

volatile unsigned char queryReply[]= { 0x00, 0x03, 0x00, 0x00};

// ackReply:  First two bytes are the preamble.  Last two bytes are the crc.
volatile unsigned char ackReply[] = { 0x30, 0x00, EPC, 0x00, 0x00};

// first 8 bits are the EPCGlobal identifier, followed by a 12-bit tag designer identifer (made up), followed by a 12-bit model number
volatile unsigned char tid[] = { 0xE2, TID_DESIGNER_ID_AND_MODEL_NUMBER };

// just a one byte placeholder for now
volatile unsigned char usermem[] = { 0x00 };

volatile unsigned char readReply[] = { 
                                    // header - 1 bit - 0 if successful, 1 if error code follows
                                    // memory words - hardcoded to 16 bits of 0xffff for now
                                    // rn - 16 bits - hardcoded to 0xf00f for now
                                    // crc-16 - 16 bits - precomputed as 0x06 0x72
                                    // filler - 15 bits of nothing (don't send)
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19};

unsigned char RN16[23];

// compiler uses working register 4 as a global variable
// Pointer to &cmd[bits]
volatile __no_init __regvar unsigned char* dest @ 4;

// compiler uses working register 5 as a global variable
// count of bits received from reader
volatile __no_init __regvar unsigned short bits @ 5;
unsigned short TRcal=0;

#define STATE_READY               0
#define STATE_ARBITRATE           1
#define STATE_REPLY               2
#define STATE_ACKNOWLEDGED        3
#define STATE_OPEN                4
#define STATE_SECURED             5
#define STATE_KILLED              6
#define STATE_READ_SENSOR         7

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
unsigned short Q = 0, slot_counter = 0, shift = 0;
unsigned int read_counter = 0;
unsigned int sensor_counter = 0;
unsigned char timeToSample = 0;

unsigned short inInventoryRound = 0;
unsigned char last_handle_b0, last_handle_b1;

#if ENABLE_SESSIONS
// selected and session inventory flags
#define S0_INDEX		0x00
#define S1_INDEX		0x01
#define S2_INDEX		0x02
#define S3_INDEX		0x03 

#define SL_ASSERTED		1
#define SL_NOT_ASSERTED		0
#define SESSION_STATE_A		0
#define SESSION_STATE_B		1

unsigned char SL;
unsigned char previous_session = 0x00;
unsigned char session_table[] = { SESSION_STATE_A, SESSION_STATE_A, SESSION_STATE_A, SESSION_STATE_A };
void initialize_sessions();
void handle_session_timeout();
inline int bitCompare(unsigned char *startingByte1, unsigned short startingBit1, unsigned char *startingByte2, unsigned short startingBit2, unsigned short len);
#endif

void sendToReader(volatile unsigned char *data, unsigned char numOfBits);
unsigned short crc16_ccitt(volatile unsigned char *data, unsigned short n);
#if 0
unsigned char crc5(volatile unsigned char *buf, unsigned short numOfBits);
#endif
void setup_to_receive();
void sleep();
unsigned short is_power_good();
#if ENABLE_SLOTS
void lfsr();
inline void loadRN16(), mixupRN16();
#endif
void crc16_ccitt_readReply(unsigned int);
int i;
static inline void handle_query(volatile short nextState);
static inline void handle_queryrep(volatile short nextState);
static inline void handle_queryadjust(volatile short nextState);
static inline void handle_select(volatile short nextState);
static inline void handle_ack(volatile short nextState);
static inline void handle_request_rn(volatile short nextState);
static inline void handle_read(volatile short nextState);
static inline void handle_nak(volatile short nextState);
static inline void do_nothing();

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

int main(void)
{
  //**********************************Timer setup**********************************
  WDTCTL = WDTPW + WDTHOLD;            // Stop Watchdog Timer 
  
  P1SEL = 0;
  P2SEL = 0;
  
  P1IE = 0;
  P1IFG = 0;
  P2IFG = 0;

  DRIVE_ALL_PINS // set pin directions correctly and outputs to low.
  
  // Check power on bootup, decide to receive or sleep.
  if(!is_power_good())
    sleep();
  
  RECEIVE_CLOCK;
 
  /*
  // already set.
#if MONITOR_DEBUG_ON
    // for monitor - pin direction
  P1DIR |= moo_debug_1;
  P2DIR |= moo_debug_2;
  P3DIR |= moo_debug_3;
  P3DIR |= moo_debug_4;
  P3DIR |= moo_debug_5;
#endif
  */

#if DEBUG_PINS_ENABLED
#if USE_2618
  DEBUG_PIN5_LOW;
#endif
#endif

#if ENABLE_SLOTS
  // setup int epc
  epc = ackReply[2]<<8;
  epc |= ackReply[3];
  
  // calculate RN16_1 table
  for (Q = 0; Q < 16; Q++)
  {
    rn16 = epc^Q;
    lfsr();
    
    if (Q > 8)
    {
      RN16[(Q<<1)-9] = __swap_bytes(rn16);
      RN16[(Q<<1)-8] = rn16;
    }
    else
    {
      RN16[Q] = rn16;
    }
  }
#endif
  
  TACTL = 0;

//  P1IES &= ~BIT2; // initial state is POS edge to find start of CW
//  P1IFG = 0x00;       // clear interrupt flag after changing edge trigger

  asm("MOV #0000h, R9");
  // dest = destorig;
  
#if READ_SENSOR
  init_sensor();
#endif

#if !(ENABLE_SLOTS)
  queryReplyCRC = crc16_ccitt(&queryReply[0],2);
  queryReply[3] = (unsigned char)queryReplyCRC;
  queryReply[2] = (unsigned char)__swap_bytes(queryReplyCRC);
#endif
  
#if SENSOR_DATA_IN_ID
  // this branch is for sensor data in the id
  ackReply[2] = SENSOR_DATA_TYPE_ID;
  state = STATE_READ_SENSOR;
  timeToSample++;
#else
  ackReplyCRC = crc16_ccitt(&ackReply[0], 14);
  ackReply[15] = (unsigned char)ackReplyCRC;
  ackReply[14] = (unsigned char)__swap_bytes(ackReplyCRC);
#endif

#if ENABLE_SESSIONS
  initialize_sessions();
#endif
  
  //state = STATE_ARBITRATE;
  state = STATE_READY;

#if MONITOR_DEBUG_ON  
  // for monitor - set STATE READY debug line - 00000 - 0
  P1OUT |= moo_debug_1;
  P1OUT &= ~moo_debug_1;
  P2OUT &= ~moo_debug_2;
#endif
  
  setup_to_receive();
  
  while (1)
  {   
    
    // TIMEOUT!  reset timer
    if (TAR > 0x256 || delimiterNotFound)   // was 0x1000
    {  
      if(!is_power_good()) {
        sleep();
      }

#if MONITOR_DEBUG_ON      
      // for monitor - set TAR OVERFLOW debug line - 00111 - 7
      if (!delimiterNotFound)
      {
        P1OUT |= moo_debug_1;
        P1OUT &= ~moo_debug_1;
        P2OUT &= ~moo_debug_2;
        P3OUT = 0x31;
      }
#endif
      
#if SENSOR_DATA_IN_ID
    // this branch is for sensor data in the id
      if ( timeToSample++ == 10 ) {
        state = STATE_READ_SENSOR;
        timeToSample = 0;
      }
#elif SENSOR_DATA_IN_READ_COMMAND
      if ( timeToSample++ == 10 ) {
        state = STATE_READ_SENSOR;
        timeToSample = 0;
      }
#else
#if !(ENABLE_READS)
    if(!is_power_good()) 
        sleep();
#endif
    inInventoryRound = 0;
    state = STATE_READY;
    
#if MONITOR_DEBUG_ON
    // for monitor - set STATE READY debug line - 00000 - 0
    P1OUT |= moo_debug_1;
    P1OUT &= ~moo_debug_1;
    P2OUT &= ~moo_debug_2;
#endif
    
#endif

#if ENABLE_SESSIONS
    handle_session_timeout();
#endif
    
#if ENABLE_SLOTS
    if (shift < 4)
        shift += 1;
    else
        shift = 0;
#endif
       
      setup_to_receive();
    }
    
    switch (state)
    {
      case STATE_READY:
      {
        inInventoryRound = 0;
        //////////////////////////////////////////////////////////////////////
        // process the QUERY command
        //////////////////////////////////////////////////////////////////////
        if ( bits == NUM_QUERY_BITS  && ( ( cmd[0] & 0xF0 ) == 0x80 ) )
        {
#if MONITOR_DEBUG_ON
            // for monitor - set QUERY PKT debug line - 01001 - 9
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x01;
#endif
          
          //DEBUG_PIN5_HIGH;
          handle_query(STATE_REPLY);
          
#if MONITOR_DEBUG_ON
          // for monitor - set STATE REPLY debug line - 00010 - 2
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x10;
#endif
          
          setup_to_receive();
        }
        //////////////////////////////////////////////////////////////////////
        // process the SELECT command
        //////////////////////////////////////////////////////////////////////
        // @ short distance has slight impact on performance
        else if ( bits >= 44  && ( ( cmd[0] & 0xF0 ) == 0xA0 ) )
        {
          
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY_ADJ debug line - 01101 - 13
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x30;
#endif
          //DEBUG_PIN5_HIGH;
          handle_select(STATE_READY);
          //DEBUG_PIN5_LOW;
          delimiterNotFound = 1;
        } // select command
        //////////////////////////////////////////////////////////////////////
        // got >= 22 bits, and it's not the beginning of a select. just reset.
        //////////////////////////////////////////////////////////////////////
        else if ( bits >= MAX_NUM_QUERY_BITS && ( ( cmd[0] & 0xF0 ) != 0xA0 ) )
        { 
          do_nothing();
          state = STATE_READY;
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set debug line - 01110 - 14
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
        }   
        break;
      }
      case STATE_ARBITRATE:		
      {     
        //////////////////////////////////////////////////////////////////////
        // process the QUERY command
        //////////////////////////////////////////////////////////////////////
        if ( bits == NUM_QUERY_BITS  && ( ( cmd[0] & 0xF0 ) == 0x80 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY PKT debug line - 01001 - 9
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x01;
#endif
          
          //DEBUG_PIN5_HIGH;
          handle_query(STATE_REPLY);
          
#if MONITOR_DEBUG_ON
          // for monitor - set STATE REPLY debug line - 00010 - 2
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x10;
#endif
          
          setup_to_receive();
        }
        //////////////////////////////////////////////////////////////////////
        // got >= 22 bits, and it's not the beginning of a select. just reset.
        //////////////////////////////////////////////////////////////////////
        //else if ( bits >= NUM_QUERY_BITS )
        else if ( bits >= MAX_NUM_QUERY_BITS && ( ( cmd[0] & 0xF0 ) != 0xA0 ) )
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_READY;
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
          
          //DEBUG_PIN5_LOW;
        }
        // this state handles query, queryrep, queryadjust, and select commands.
        //////////////////////////////////////////////////////////////////////
        // process the QUERYREP command
        //////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_QUERYREP_BITS && ( ( cmd[0] & 0x06 ) == 0x00 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY_REP debug line - 01100 - 12
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x20;
#endif
          //DEBUG_PIN5_HIGH;
          handle_queryrep(STATE_REPLY);
          //DEBUG_PIN5_LOW;
          //setup_to_receive();
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
        } // queryrep command
        //////////////////////////////////////////////////////////////////////
        // process the QUERYADJUST command
        //////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_QUERYADJ_BITS  && ( ( cmd[0] & 0xF8 ) == 0x48 ) )
        {
          // at short distance, you get better performance (~52 t/s) if you
          // do setup_to_receive() rather than dnf =1. not sure that this holds
          // true at distance though - need to recheck @ 2-3 ms.
          //DEBUG_PIN5_HIGH;
          
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY_ADJ debug line - 01101 - 13
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x21;
#endif
          
          handle_queryadjust(STATE_REPLY);
          //DEBUG_PIN5_LOW;
          setup_to_receive();
          //delimiterNotFound = 1;
        } // queryadjust command
        //////////////////////////////////////////////////////////////////////
        // process the SELECT command
        //////////////////////////////////////////////////////////////////////
        // @ short distance has slight impact on performance
        else if ( bits >= 44  && ( ( cmd[0] & 0xF0 ) == 0xA0 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set SELECT debug line - 01110 - 14
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x30;
#endif
          
          //DEBUG_PIN5_HIGH;
          handle_select(STATE_READY);
          //DEBUG_PIN5_LOW;
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
          
        } // select command
       
      break;
      }
    
      case STATE_REPLY:		
      {
        // this state handles query, query adjust, ack, and select commands
        ///////////////////////////////////////////////////////////////////////
        // process the ACK command
        ///////////////////////////////////////////////////////////////////////
        if ( bits == NUM_ACK_BITS  && ( ( cmd[0] & 0xC0 ) == 0x40 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set ACK PKT debug line - 01010 - 10
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x10;
#endif
#if ENABLE_READS
          handle_ack(STATE_ACKNOWLEDGED);

          setup_to_receive();
#elif SENSOR_DATA_IN_ID
          handle_ack(STATE_ACKNOWLEDGED);
          delimiterNotFound = 1; // reset
#else
          // this branch for hardcoded query/acks
          handle_ack(STATE_ACKNOWLEDGED);
          //delimiterNotFound = 1; // reset
          setup_to_receive();
#endif
#if MONITOR_DEBUG_ON
          // for monitor - set STATE ACK debug line - 00011 - 3
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x11;
#endif
        }
        //////////////////////////////////////////////////////////////////////
        // process the QUERY command
        //////////////////////////////////////////////////////////////////////
        if ( bits == NUM_QUERY_BITS  && ( ( cmd[0] & 0xF0 ) == 0x80 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY PKT debug line - 01001 - 9
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x01;
#endif
          // i'm supposed to stay in state_reply when I get this, but if I'm
          // running close to 1.8v then I really need to reset and get in the
          // sleep, which puts me back into state_arbitrate. this is complete
          // a violation of the protocol, but it sure does make everything
          // work better. - polly 8/9/2008
          //DEBUG_PIN5_HIGH;
          handle_query(STATE_REPLY);
          //DEBUG_PIN5_LOW;
          //delimiterNotFound = 1;
          setup_to_receive();
        }
        //////////////////////////////////////////////////////////////////////
        // process the QUERYREP command
        //////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_QUERYREP_BITS && ( ( cmd[0] & 0x06 ) == 0x00 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY_REP debug line - 01100 - 12
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x20;
#endif
          
          //DEBUG_PIN5_HIGH;
	  do_nothing();
	  state = STATE_ARBITRATE;
          
#if MONITOR_DEBUG_ON
          // for monitor - set STATE ARBITRATE debug line - 00001 - 1
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x01;
#endif
          
          //handle_queryrep(STATE_ARBITRATE);
          //DEBUG_PIN5_LOW;
          setup_to_receive();
          //delimiterNotFound = 1; // reset
        } // queryrep command
        //////////////////////////////////////////////////////////////////////
        // process the QUERYADJUST command
        //////////////////////////////////////////////////////////////////////
          else if ( bits == NUM_QUERYADJ_BITS  && ( ( cmd[0] & 0xF8 ) == 0x48 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set QUERY_ADJ debug line - 01101 - 13
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x21;
#endif
          //DEBUG_PIN5_HIGH;
          handle_queryadjust(STATE_REPLY);
          //DEBUG_PIN5_LOW;
          //setup_to_receive();
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
          
        } // queryadjust command
        //////////////////////////////////////////////////////////////////////
        // process the SELECT command
        //////////////////////////////////////////////////////////////////////
        else if ( bits >= 44  && ( ( cmd[0] & 0xF0 ) == 0xA0 ) )
        {
#if MONITOR_DEBUG_ON
          // for monitor - set SELECT debug line - 01110 - 14
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT |= moo_debug_2;
          P3OUT = 0x30;
#endif
          
          //DEBUG_PIN5_HIGH;
          handle_select(STATE_READY); 
          //DEBUG_PIN5_LOW;
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
          
          //setup_to_receive();
        } // select command
        else if ( bits >= MAX_NUM_QUERY_BITS && ( ( cmd[0] & 0xF0 ) != 0xA0 ) && ( ( cmd[0] & 0xF0 ) != 0x80 ) )
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_READY;
          delimiterNotFound = 1;
          //DEBUG_PIN5_LOW;
        }
        break;
      }
      case STATE_ACKNOWLEDGED:		
      {      
        // responds to query, ack, request_rn cmds
        // takes action on queryrep, queryadjust, and select cmds
        /////////////////////////////////////////////////////////////////////
        // process the REQUEST_RN command
        //////////////////////////////////////////////////////////////////////
        if ( bits >= NUM_REQRN_BITS && ( cmd[0] == 0xC1 ) )
        {
#if 1
#if MONITOR_DEBUG_ON
          // for monitor - set REQ_RN debug line - 01011 - 11
    P1OUT |= moo_debug_1;
    P1OUT &= ~moo_debug_1;
    P2OUT |= moo_debug_2;
    P3OUT = 0x11;
#endif
          DEBUG_PIN5_HIGH;
          handle_request_rn(STATE_OPEN);
          DEBUG_PIN5_LOW;
          setup_to_receive();
#else
          handle_request_rn(STATE_READY);
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
#endif
        }
        
#if 1
        //////////////////////////////////////////////////////////////////////
        // process the QUERY command
        //////////////////////////////////////////////////////////////////////
        if ( bits == NUM_QUERY_BITS  && ( ( cmd[0] & 0xF0 ) == 0x80 ) )
        {
#if MONITOR_DEBUG_ON
              // for monitor - set QUERY PKT debug line - 01001 - 9
    P1OUT |= moo_debug_1;
    P1OUT &= ~moo_debug_1;
    P2OUT |= moo_debug_2;
    P3OUT = 0x01;
#endif
          //DEBUG_PIN5_HIGH;
          handle_query(STATE_REPLY);
          //DEBUG_PIN5_LOW;
          delimiterNotFound = 1;
          
#if MONITOR_DEBUG_ON
          // for monitor - set DELIMITER NOT FOUND debug line - 00110 - 6
          P1OUT |= moo_debug_1;
          P1OUT &= ~moo_debug_1;
          P2OUT &= ~moo_debug_2;
          P3OUT = 0x30;
#endif
          
          //setup_to_receive();
        }
        ///////////////////////////////////////////////////////////////////////
        // process the ACK command
        ///////////////////////////////////////////////////////////////////////
        // this code doesn't seem to get exercised in the real world. if i ever
        // ran into a reader that generated an ack in an acknowledged state,
        // this code might need some work.
        //else if ( bits == 20  && ( ( cmd[0] & 0xC0 ) == 0x40 ) )
        else if ( bits == NUM_ACK_BITS  && ( ( cmd[0] & 0xC0 ) == 0x40 ) )
        {
#if MONITOR_DEBUG_ON
              // for monitor - set ACK PKT debug line - 01010 - 10
    P1OUT |= moo_debug_1;
    P1OUT &= ~moo_debug_1;
    P2OUT |= moo_debug_2;
    P3OUT = 0x10;
#endif
          //DEBUG_PIN5_HIGH;
          handle_ack(STATE_ACKNOWLEDGED);
          
#if MONITOR_DEBUG_ON
              // for monitor - set STATE ACK debug line - 00011 - 3
    P1OUT |= moo_debug_1;
    P1OUT &= ~moo_debug_1;
    P2OUT &= ~moo_debug_2;
    P3OUT = 0x11;
#endif
          
          //DEBUG_PIN5_LOW;
          setup_to_receive();
        }
        //////////////////////////////////////////////////////////////////////
        // process the QUERYREP command
        //////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_QUERYREP_BITS && ( ( cmd[0] & 0x06 ) == 0x00 ) )
        {
          // in the acknowledged state, rfid chips don't respond to queryrep commands
          do_nothing();
          state = STATE_READY; 
          delimiterNotFound = 1;
        } // queryrep command

        //////////////////////////////////////////////////////////////////////
        // process the QUERYADJUST command
        //////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_QUERYADJ_BITS  && ( ( cmd[0] & 0xF8 ) == 0x48 ) )
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_READY; 
          delimiterNotFound = 1;
          //DEBUG_PIN5_LOW;
        } // queryadjust command
        //////////////////////////////////////////////////////////////////////
        // process the SELECT command
        //////////////////////////////////////////////////////////////////////
        else if ( bits >= 44  && ( ( cmd[0] & 0xF0 ) == 0xA0 ) )
        {
          //DEBUG_PIN5_HIGH;
          handle_select(STATE_READY); 
          delimiterNotFound = 1;
          //DEBUG_PIN5_LOW;
        } // select command   
        //////////////////////////////////////////////////////////////////////
        // process the NAK command
        //////////////////////////////////////////////////////////////////////
        else if ( bits >= 10 && ( cmd[0] == 0xC0 ) )
        //else if ( bits >= NUM_NAK_BITS && ( cmd[0] == 0xC0 ) )
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_ARBITRATE; 
          delimiterNotFound = 1;
          //DEBUG_PIN5_LOW;
        } 
        //////////////////////////////////////////////////////////////////////
        // process the READ command
        //////////////////////////////////////////////////////////////////////
        // warning: won't work for read addrs > 127d
        if ( bits == NUM_READ_BITS && ( cmd[0] == 0xC2 ) )
        {
          //DEBUG_PIN5_HIGH;
          handle_read(STATE_ARBITRATE);
          state = STATE_ARBITRATE;
          delimiterNotFound = 1 ;
          //DEBUG_PIN5_LOW;
        }
        // FIXME: need write, kill, lock, blockwrite, blockerase
        //////////////////////////////////////////////////////////////////////
        // process the ACCESS command
        //////////////////////////////////////////////////////////////////////
        if ( bits >= 56  && ( cmd[0] == 0xC6 ) )
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_ARBITRATE;
          delimiterNotFound = 1 ;
          //DEBUG_PIN5_LOW;
        }
#endif
        else if ( bits >= MAX_NUM_READ_BITS )
        {
          //DEBUG_PIN5_HIGH;
          //do_nothing();
          state = STATE_ARBITRATE;
          delimiterNotFound = 1 ;
          //DEBUG_PIN5_LOW;
        }          
        
#if 0
        // kills performance ...
        else if ( bits >= 44 ) 
        {
          do_nothing();
          state = STATE_ARBITRATE;
          delimiterNotFound = 1;
        }
#endif
        break;
      }
      case STATE_OPEN:		
      {
        // responds to query, ack, req_rn, read, write, kill, access, blockwrite, and blockerase cmds
        // processes queryrep, queryadjust, select cmds
        //////////////////////////////////////////////////////////////////////
        // process the READ command
        //////////////////////////////////////////////////////////////////////
        // warning: won't work for read addrs > 127d
        if ( bits == NUM_READ_BITS  && ( cmd[0] == 0xC2 ) )
        {
          //DEBUG_PIN5_HIGH;
          handle_read(STATE_OPEN);
          //DEBUG_PIN5_LOW;
          // note: setup_to_receive() et al handled in handle_read
        }
        //////////////////////////////////////////////////////////////////////
        // process the REQUEST_RN command
        //////////////////////////////////////////////////////////////////////
        else if ( bits >= NUM_REQRN_BITS  && ( cmd[0] == 0xC1 ) )
          //else if ( bits >= 30  && ( cmd[0] == 0xC1 ) )
        {
          handle_request_rn(STATE_OPEN);
          setup_to_receive();
          //delimiterNotFound = 1; // more bad reads??
         }
        //////////////////////////////////////////////////////////////////////
        // process the QUERY command
        //////////////////////////////////////////////////////////////////////
        if ( bits == NUM_QUERY_BITS  && ( ( cmd[0] & 0xF0 ) == 0x80 ) )
        {
          handle_query(STATE_REPLY);
          //setup_to_receive();
          delimiterNotFound = 1;
        }
        //////////////////////////////////////////////////////////////////////
        // process the QUERYREP command
        //////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_QUERYREP_BITS && ( ( cmd[0] & 0x06 ) == 0x00 ) ) 
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_READY;
          //delimiterNotFound = 1;
          setup_to_receive();
          //DEBUG_PIN5_LOW;
        } // queryrep command
        //////////////////////////////////////////////////////////////////////
        // process the QUERYADJUST command
        //////////////////////////////////////////////////////////////////////
        //else if ( bits == NUM_QUERYADJ_BITS  && ( ( cmd[0] & 0xF0 ) == 0x90 ) )
          else if ( bits == 9  && ( ( cmd[0] & 0xF8 ) == 0x48 ) )
        {
          //DEBUG_PIN5_HIGH;
          do_nothing();
          state = STATE_READY;
          delimiterNotFound = 1;
          //DEBUG_PIN5_LOW;
        } // queryadjust command
        ///////////////////////////////////////////////////////////////////////
        // process the ACK command
        ///////////////////////////////////////////////////////////////////////
        else if ( bits == NUM_ACK_BITS  && ( ( cmd[0] & 0xC0 ) == 0x40 ) )
        {
          //DEBUG_PIN5_HIGH;
          handle_ack(STATE_OPEN);
          //setup_to_receive();
          delimiterNotFound = 1;
          //DEBUG_PIN5_LOW;
        }
        //////////////////////////////////////////////////////////////////////
        // process the SELECT command
        //////////////////////////////////////////////////////////////////////
        else if ( bits >= 44  && ( ( cmd[0] & 0xF0 ) == 0xA0 ) )
        {
          handle_select(STATE_READY); 
          delimiterNotFound = 1;
        } // select command
        //////////////////////////////////////////////////////////////////////
        // process the NAK command
        //////////////////////////////////////////////////////////////////////
        //else if ( bits >= NUM_NAK_BITS && ( cmd[0] == 0xC0 ) )
        else if ( bits >= 10 && ( cmd[0] == 0xC0 ) )
        {
          handle_nak(STATE_ARBITRATE); 
          delimiterNotFound = 1;
        }

        break;
      }
    
    case STATE_READ_SENSOR:
      {      
#if SENSOR_DATA_IN_READ_COMMAND
        read_sensor(&readReply[0]);
        // crc is computed in the read state
        RECEIVE_CLOCK;
        state = STATE_READY;  
        delimiterNotFound = 1; // reset
#elif SENSOR_DATA_IN_ID
        read_sensor(&ackReply[3]);
        RECEIVE_CLOCK;
        ackReplyCRC = crc16_ccitt(&ackReply[0], 14);
        ackReply[15] = (unsigned char)ackReplyCRC;
        ackReply[14] = (unsigned char)__swap_bytes(ackReplyCRC);
        state = STATE_READY;
        delimiterNotFound = 1; // reset
#endif
        
        break;
      } // end case  
    } // end switch
    
  } // while loop
}

static inline void handle_query(volatile short nextState)
{
  TAR = 0;
#if (!ENABLE_SLOTS)  && (!ENABLE_SESSIONS)
    while ( TAR < 90 ); // if bit test is 22
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
  TAR = 0;
#elif (!ENABLE_SLOTS) && ENABLE_SESSIONS
  while ( TAR < 160 ); // if bit test is 22
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
  TAR = 0;
#else
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
  TAR = 0;
#endif

  // set up for TRcal
  if ( cmd[0] & BIT3)
  {
    divideRatio = 21;//64/3;
  } else
  {
    divideRatio = 8;
  }
  // set up for subcarrier symbol
  subcarrierNum = cmd[0] & (BIT2 | BIT1);
  if (subcarrierNum == 0)
  {
    subcarrierNum = 1;
  } else if ( subcarrierNum == 2 )
  {
    subcarrierNum = 2;
  } else if (subcarrierNum == 4)
  {
    subcarrierNum = 4;
  } else
  {
    subcarrierNum = 8;
  }
          
  // set up for TRext
  if (cmd[0] & BIT0)
  {
    TRext = 1;
  } else
  {
    TRext = 0;
  }
  
  //DEBUG_PIN5_HIGH;
  
#if ENABLE_SESSIONS
  
#if 1
// command-specific bit masks
#define QUERY_SEL_MASK		0xC0
#define QUERY_SESSION_MASK	0x30
#define QUERY_TARGET_MASK	0x08

// command-specific bit flags
#define QUERY_SEL_SL 		0xC0
#define QUERY_SEL_NOTSL 	0x80

  unsigned short sel = cmd[1] & QUERY_SEL_MASK;
  unsigned short session = (cmd[1] & QUERY_SESSION_MASK) >> 4;
  unsigned short target = cmd[1] & QUERY_TARGET_MASK;
  unsigned short match = 0;
  
  //DEBUG_PIN5_HIGH;

#define TARGETISEQUAL(t,s)      ((t == 0x00 && s == SESSION_STATE_A) || (t == 0x08  && s == SESSION_STATE_B)) 

  // if we are already in an inventory round and the session matches the previous session, invert the session inventory flag
  if ( state == STATE_ACKNOWLEDGED || state == STATE_OPEN || state == STATE_SECURED )
  {
	if ( session == previous_session )
        {
		// invert session's inventory flag
		if ( session_table[session] == SESSION_STATE_A ) session_table[session] = SESSION_STATE_B; else session_table[session] = SESSION_STATE_A;
	}
  }

  // now figure out if the SL and session flags match. Let's look at the SL flag first.
  if ( sel < QUERY_SEL_NOTSL || sel == QUERY_SEL_SL && SL == SL_ASSERTED || sel == QUERY_SEL_NOTSL && SL == SL_NOT_ASSERTED )
  {
	// SL flag matches -- now how about the session flag?
	if ( TARGETISEQUAL(target,session_table[session]) )
        {
		// session match too
		match = 1;
	}
  }

  if ( ! match )
  {
	// no matching SL/session flags. don't respond and transistion to READY state.
	TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
	TAR = 0;
	state = STATE_READY;
        //DEBUG_PIN5_LOW;
	return;
  }

  // OK, got matching SL and session inventory flags, so we're in this inventory round. save the session
  // to the previous_session variable in case this is a new session.
  previous_session = session;
  
#else
  //DEBUG_PIN5_HIGH;
  while ( TAR < 140 );
  TAR = 0;
#endif

#else

  // we don't care about SL or session inventory flags at all. just proceed
  // as if we have matched on them.
 
#endif

#if ENABLE_SLOTS

  // next step is to built a slot counter

  // parse for Q number and choose a Q value randomly
  Q = (cmd[1] & 0x07)<<1;
  if ((cmd[2] & 0x80) == 0x80)
    Q += 0x01;
          
  // pick Q randomly
  slot_counter = Q >> shift;
  
  // HACK ALERT: the Impinj reader seems to output at least two Queries before
  // it sends along an Ack. If I followed the spec, I might well reply to the
  // first Query but not the second, owing to a nonzero slot counter. In this case
  // the reader would just send along QueryReps until the slot counter got
  // to zero, and I'd emit another response. Problem is, we're a power-constrained
  // device and we don't necessarily have the ability to respond to a list of
  // QueryReps. Therefore, I will check to see if I'm in an inventory round --
  // that is, I've already seen a query -- and if I am, pretend my slot counter is
  // zero and respond right away.
  
  // slot counter built and it's 0. we can send a reply!
  if ( inInventoryRound == 1 || slot_counter == 0)
  {

    //DEBUG_PIN5_HIGH;
    // compute a RN16
    loadRN16();   
    
    // compute the CRC
    queryReplyCRC = crc16_ccitt(&queryReply[0],2);
    queryReply[3] = (unsigned char)queryReplyCRC;
    queryReply[2] = (unsigned char)__swap_bytes(queryReplyCRC);
    
#if ENABLE_HANDLE_CHECKING
    //last_handle_b0 = queryReply[0];
    //last_handle_b1 = queryReply[1];
#endif
    
    TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
    while ( TAR < 140 );
    TAR = 0;
    
    // send out the packet, and transition to STATE_REPLY
    sendToReader(&queryReply[0], 17); 
    state = nextState;
    
    // mix up the RN16 table a bit for next time
    mixupRN16();
  }

  // slot counter isn't 0, so we don't send a reply. We wait for a
  // followup QueryRep or QueryAdjust command. In the meantime,
  // we transition to STATE_ARBITRATE.
  else
  {
    //DEBUG_PIN5_HIGH;
    TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
    TAR = 0;
    state = STATE_ARBITRATE;
  }

  inInventoryRound = 1;

#else

#if ENABLE_HANDLE_CHECKING
  //last_handle_b0 = queryReply[0];
  //last_handle_b1 = queryReply[1];
#endif
  
  // we don't care about slots, so just send the packet and go to STATE_REPLY.
  sendToReader(&queryReply[0], 17); 
  state = nextState;

#endif
  //DEBUG_PIN5_LOW;
  
}

// Word to the wise: I've been testing this code against the Impinj RFIDemo, using
// the InventoryFilter page. It appears to me that it only sends the first three bytes
// pattern fields (aka the mask field in the spec) correctly. So don't expect it
// to be able to look for (say) entire EPCs correctly. Also, when testing this code
// out in RFIDDemo, put your mask data in hex in the leftmost part of the pattern
// field.
inline void handle_select(volatile short nextState)
{
  do_nothing();
  
//DEBUG_PIN5_HIGH;

#if ENABLE_SESSIONS
// command-specific bit masks
#define SELECT_TARGET_MASK		0x0E
#define SELECT_ACTIONB0_MASK		0x01
#define SELECT_ACTIONB1_MASK		0xC0
#define SELECT_MEMBANK_MASK             0x30
#define SELECT_POINTERB0_MASK           0x0F
#define SELECT_POINTERB1_MASK           0xF0
#define SELECT_LENGTHB0_MASK            0x0F
#define SELECT_LENGTHB1_MASK            0xF0
#define SELECT_MASK_MASK                0x0F
//#define SELECT_TRUNCATE_MASK

// command-specific bit flags
#define SELECT_TARGET_SL		0x04

  unsigned short target = (cmd[0] & SELECT_TARGET_MASK) >> 1;
  unsigned short action = (cmd[0] & SELECT_ACTIONB0_MASK) << 2;
  unsigned short action2 = (cmd[1] & SELECT_ACTIONB1_MASK) >> 6;
  action |= action2;
  unsigned short membank = (cmd[1] & SELECT_MEMBANK_MASK) >> 4;
  unsigned short pointer = (cmd[1] & SELECT_POINTERB0_MASK) << 4;
  unsigned short pointer2 = (cmd[2] & SELECT_POINTERB1_MASK) >> 4;
  pointer |= pointer2;
  unsigned short length = (cmd[2] & SELECT_LENGTHB0_MASK) << 4;
  unsigned short length2 = (cmd[3] & SELECT_LENGTHB1_MASK) >> 4;
  length |= length2;
  
  // can only handle length fields > 0 and membanks == 0 are invalid
  if ( length <= 0 || membank == 0x00 )
    return;
  
  unsigned char *mask = (unsigned char *)&cmd[3];
  unsigned short maskbit = 3; // start match attempt at leftmost bit of mask field
  
  unsigned char *sourcebyte = (unsigned char *)0;
  // OK, this is a little confusing. The pointer parameter is an offset
  // into a buffer, with a value of 0 meaning "the leftmost bit" and
  // a value of 7 meaning "the rightmost bit of the first byte." But my
  // bitCompare function thinks 7 is the leftmost bit of the first byte
  // and 0 is the rightmost bit of that byte. So I'll need to do a little
  // mapping here.
  unsigned short sourcebyteoffset = ( pointer/8 );
  unsigned short sourcebit = (7 - (pointer % 8)); // pointer->bitCompare translation
  
  if ( membank == 0x01 )
  {
    // match on epc
    if ( sourcebyteoffset >= 8 ) return;
    sourcebyte = (unsigned char *)&ackReply[2] + sourcebyteoffset;
  }
  else if ( membank == 0x02 )
  {
    // matching on tid. 
    if ( sourcebyteoffset >= 3 ) return;
    sourcebyte = (unsigned char *)&tid[0] + sourcebyteoffset;
  }
  else
  {
    // matching on user field. for now this is just one byte long, so
    // mess with the params to make sure the values are sane in this context
    sourcebyte = (unsigned char *)usermem;
    sourcebit = 7;
    if ( length > 8 ) length = 8;
  }
  
  unsigned short matching = 0;
  
  if ( bitCompare(sourcebyte,sourcebit,mask,maskbit,length) == 1 )
  {
    matching = 1;
  }

#define ASSERT(t) { \
	if (t == SELECT_TARGET_SL) SL = SL_ASSERTED; \
	else session_table[t] = SESSION_STATE_A; \
} 

#define DEASSERT(t) { \
	if (t == SELECT_TARGET_SL) SL = SL_NOT_ASSERTED; \
	else session_table[t] = SESSION_STATE_B; \
}

#define NEGATE(t) { \
	if (t == SELECT_TARGET_SL) if ( SL == SL_ASSERTED ) SL = SL_NOT_ASSERTED; else SL = SL_ASSERTED; \
	else if ( session_table[t] == SESSION_STATE_A ) session_table[t] = SESSION_STATE_B; else session_table[t] = SESSION_STATE_A; \
}

  switch ( action ) {
	case 0x0:
		if ( matching ) ASSERT(target) else DEASSERT(target);
		break;
	case 0x1:
		if ( matching ) ASSERT(target);
		break;
	case 0x2:
		if ( ! matching ) DEASSERT(target);
		break;
	case 0x3:
		if ( matching ) NEGATE(target);
		break;
	case 0x4:
		if ( matching ) DEASSERT(target) else ASSERT(target);
		break;
	case 0x5:
		if ( matching ) DEASSERT(target);
		break;
	case 0x6:
		if (!  matching ) ASSERT(target);
		break;
	case 0x7:
		if (!  matching ) NEGATE(target);
		break;
	default:
		break;
  }

#endif
  
  state = nextState;
  DEBUG_PIN5_LOW;
}

static inline void handle_queryrep(volatile short nextState)
{
   
  TAR = 0;
#if (!ENABLE_SESSIONS)
  while ( TAR < 150 );
#endif
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE; 
  TAR = 0;
  
#if ENABLE_SESSIONS

// command-specific bit masks
#define QUERYREP_UPDNB0_MASK	0x01	
#define QUERYREP_UPDNB1_MASK	0x80
  
  // fyi, unless i'm missing something, the impinj reader doesn't always send
  // correct session values. for now i will comment this section out, until
  // i can test against a different reader firmware rev
#if 0
  unsigned char session = (cmd[0] & QUERYREP_UPDNB0_MASK) << 1;
  unsigned char tmp = (cmd[1] & QUERYREP_UPDNB1_MASK) >> 7;
  session |= tmp;

  if ( session != previous_session )
  {
	// drop the packet
	return;
  }
#else
  // hack hack hack
  unsigned char session = previous_session;
#endif

  if ( state == STATE_ACKNOWLEDGED || state == STATE_OPEN || state == STATE_SECURED ) 
  {
	// invert session's inventory flag
	if ( session_table[session] == SESSION_STATE_A ) session_table[session] = SESSION_STATE_B; else session_table[session] = SESSION_STATE_A;
	state = STATE_READY;
	return;
  }
#endif

#if ENABLE_SLOTS
  slot_counter -= 1;
  if ( slot_counter != 0 )
  {
    state = STATE_ARBITRATE;
    return;
  }
#endif
  sendToReader(&queryReply[0], 17);
  state = nextState;
}

static inline void handle_queryadjust(volatile short nextState)
{
  
  TAR = 0;
#if !(ENABLE_SLOTS) && !(ENABLE_SESSIONS)
  while ( TAR < 300 );
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE; 
  TAR = 0;
#endif

#if ENABLE_SESSIONS

// command-specific bit masks
#define QUERYADJ_SESSION_MASK	0x0C

  unsigned short session = (cmd[0] & QUERYADJ_SESSION_MASK) >> 1;

  if ( session != previous_session )
  {
	// drop the packet, but stay in the same state
	TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
	TAR = 0;
	return;
  }

  // if we got this packet in ACKNOWLEDGED, OPEN, or SECURED states, invert the
  // session flag and transition to STATE_READY to take myself out of this
  // inventory round.
  if ( state == STATE_ACKNOWLEDGED || state == STATE_OPEN || state == STATE_SECURED ) 
  {
	// invert session's inventory flag
	if ( session_table[session] == SESSION_STATE_A ) session_table[session] = SESSION_STATE_B; else session_table[session] = SESSION_STATE_A;
	state = STATE_READY;
	TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
	TAR = 0;
	return;
  }

#endif
  
#if ENABLE_SLOTS

#define QUERYADJ_UPDNB0_MASK	0x01	
#define QUERYADJ_UPDNB1_MASK	0xC0	
  
  //DEBUG_PIN5_HIGH;

  unsigned char updn = (cmd[0] & QUERYADJ_UPDNB0_MASK) << 2;
  unsigned char tmp = (cmd[1] & QUERYADJ_UPDNB1_MASK) >> 6;
  updn |= tmp;

  if ( Q == 0xf && updn == 0x3 ) updn = 0x0;
  if ( Q == 0x0 && updn == 0x1 ) updn = 0x0;

  if ( updn == 0x6 ) Q += 1;
  else if ( updn == 0x3 ) Q -= 1;
  else if ( updn != 0x0 )
  {
    // impinj likes to send me plenty of updn values of 0x01, which isn't
    // valid. Spec says to ignore the command in these cases.
    return;
  }

  // pick Q randomly
  slot_counter = Q >> shift;
          
  // slot counter built and it's 0. we can send a reply!
  if (slot_counter == 0)
  {
	// compute a RN16
	loadRN16();   
   
	// compute a CRC
	queryReplyCRC = crc16_ccitt(&queryReply[0],2);
	queryReply[3] = (unsigned char)queryReplyCRC;
	queryReply[2] = (unsigned char)__swap_bytes(queryReplyCRC);
    
	TACCTL1 &= ~CCIE;     // Disable capturing and comparing interrupt
	TAR = 0;
    
	// send out the packet, and transition to STATE_REPLY
	sendToReader(&queryReply[0], 17); 
	state = nextState;
    
	// mix up the RN16 table a bit for next time
   	 mixupRN16();
  }
  else
  {
	// slot counter isn't 0, so we don't send a reply. We wait for a
	// followup QueryRep or QueryAdjust command. In the meantime,
	// we transition to STATE_ARBITRATE.
	state = STATE_ARBITRATE;
  }
  
  //DEBUG_PIN5_LOW;
  
#else

  // we don't care about slots, so just send the packet and go to STATE_REPLY.
  sendToReader(&queryReply[0], 17); 
  state = nextState;
#endif
}

static inline void handle_ack(volatile short nextState)
{
  TACCTL1 &= ~CCIE;
  TAR = 0;
  if ( NUM_ACK_BITS == 20 )
    while ( TAR < 90 );
  else
    while ( TAR < 400 );          // on the nose for 3.5MHz
  TAR = 0;
  
#if ENABLE_HANDLE_CHECKING
  //unsigned char ack_b0 = ((last_handle_b0 & 0xFC) >> 2) ;
  //ack_b0 |= 0x40;
  //unsigned char ack_b1 = ((last_handle_b1 & 0xFC) >> 2);
  //ack_b1 = ack_b1 || (last_handle_b0 & 0x03) << 6;
  //unsigned char ack_b2 = ((last_handle_b1 & 0x03) << 6);
  
  //if ( ack_b0 != cmd[0] || ack_b1 != cmd[1] || ack_b2 != (cmd[2] & 0xC0) )
  //    return; 
#endif
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  // after that sends tagResponse
  sendToReader(&ackReply[0], 129);
  state = nextState;
}

static inline void do_nothing()
{
  TACCTL1 &= ~CCIE;
  TAR = 0;
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
}

static inline void handle_request_rn(volatile short nextState)
{
  TACCTL1 &= ~CCIE;
  TAR = 0;
  // FIXME FIXME
  // here's a mystery: if I enable this line below, I clobber the follow-up
  // read command. specifically, the read command's cmd[0] shows up as 0xFF.
  // if i leave this line commented out, everything's fine.
  // theory #1 was that the bit_in_enable line doesn't switch around fast
  // enough. but i do the same thing for all the other commands, and i don't
  // have any problems. also, the time space between request_rn and the read
  // is *much larger* than betwen the other commands. theory #1 disproved.
  // theory #2 was that i had fallen asleep and wasn't processing the incoming
  // bits. but it turns out that i am wide awake. theory #2 disproven.
  // theory #3. generally when i see 0xff in the cmd[0] field it means that
  // the bit buffer got overwritten. as far as I can tell, it hasn't, and there's
  // plenty of room in the receiving buffer. theory #3 disproven.
  // hmmm.
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  if ( NUM_REQRN_BITS == 42 )
    while ( TAR < 80 );
  else if ( NUM_REQRN_BITS == 41 )
    while ( TAR < 170 );
  TAR = 0;
  sendToReader(&queryReply[0], 33);
  if ( read_counter == 0xffff ) read_counter = 0; else read_counter++;
  state = nextState;
}

static inline void handle_read(volatile short nextState)
{
     
#if SENSOR_DATA_IN_READ_COMMAND
  
  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE;
  TAR = 0;
  
  readReply[DATA_LENGTH_IN_BYTES] = queryReply[0];        // remember to restore correct RN before doing crc()
  readReply[DATA_LENGTH_IN_BYTES+1] = queryReply[1];        // because crc() will shift bits to add
  crc16_ccitt_readReply(DATA_LENGTH_IN_BYTES);    // leading "0" bit.
   
  // DATA_LENGTH_IN_BYTES*8 bits for data + 16 bits for the handle + 16 bits for the CRC + leading 0 + add one to number of bits for xmit code
  sendToReader(&readReply[0], ((DATA_LENGTH_IN_BYTES*8)+16+16+1+1));
  state = nextState;
  delimiterNotFound = 1;
            
#elif SIMPLE_READ_COMMAND

  //P1OUT &= ~RX_EN_PIN;   // turn off comparator
  TACCTL1 &= ~CCIE;
  TAR = 0;
  
#define USE_COUNTER 1
#if USE_COUNTER
  readReply[0] = __swap_bytes(read_counter);
  readReply[1] = read_counter;
#else
  readReply[0] = 0x03;
  readReply[1] = 0x04;
#endif
  readReply[2] = queryReply[0];        // remember to restore correct RN before doing crc()
  readReply[3] = queryReply[1];        // because crc() will shift bits to add
  crc16_ccitt_readReply(2);    // leading "0" bit.
            
  // after that sends tagResponse
  // 16 bits for data + 16 bits for the handle + 16 bits for the CRC + leading 0 + add one to number of bits for seong's xmit code
  sendToReader(&readReply[0], 50);
  state = nextState;
  delimiterNotFound = 1; // reset
#endif          
}

static inline void handle_nak(volatile short nextState)
{
  TACCTL1 &= ~CCIE;
  TAR = 0;
  state = nextState;
}


//****************************** SETUP TO RECEIVE  *************************************
// note: port interrupt can also reset, but it doesn't call this function
//       because function call causes PUSH instructions prior to bit read
//       at beginning of interrupt, which screws up timing.  so, remember
//       to change things in both places.
static inline void setup_to_receive()
{
  //P4OUT &= ~BIT3;
  _BIC_SR(GIE); // temporarily disable GIE so we can sleep and enable interrupts at the same time
  
  P1OUT |= RX_EN_PIN;
  
  delimiterNotFound = 0;
  // setup port interrupt on pin 1.2
  P1SEL &= ~BIT2;  //Disable TimerA2, so port interrupt can be used
  // Setup timer. It has to setup because there is no setup time after done with port1 interrupt.
  TACTL = 0;
  TAR = 0;
  TACCR0 = 0xFFFF;    // Set up TimerA0 register as Max
  TACCTL0 = 0;
  TACCTL1 = SCS + CAP;   //Synchronize capture source and capture mode
  TACTL = TASSEL1 + MC1 + TAIE;  // SMCLK and continuous mode and Timer_A interrupt enabled.

  // initialize bits
  bits = 0;
  // initialize dest
  dest = destorig;  // = &cmd[0]
  // clear R6 bits of word counter from prior communications to prevent dest++ on 1st port interrupt
  asm("CLR R6");
  
  P1IE = 0;
  P1IES &= ~RX_PIN; // Make positive edge for port interrupt to detect start of delimiter
  P1IFG = 0;  // Clear interrupt flag
            
  P1IE  |= RX_PIN; // Enable Port1 interrupt
  _BIS_SR(LPM4_bits | GIE);
  return;
}

inline void sleep()
{
  P1OUT &= ~RX_EN_PIN;

#if MONITOR_DEBUG_ON
  // for monitor - set SLEEP debug line - 01000 - 8
  P1OUT |= moo_debug_1;
  P1OUT &= ~moo_debug_1;
  P2OUT |= moo_debug_2;
  P3OUT = 0x00;
#endif
  
  // enable port interrupt for voltage supervisor
  P2IES = 0;
  P2IFG = 0;
  P2IE |= VOLTAGE_SV_PIN;
  P1IE = 0;
  P1IFG = 0;
  TACTL = 0;
  
  _BIC_SR(GIE); // temporarily disable GIE so we can sleep and enable interrupts at the same time
  P2IE |= VOLTAGE_SV_PIN; // Enable Port 2 interrupt
  
  if (is_power_good())
    P2IFG = VOLTAGE_SV_PIN;
  
  _BIS_SR(LPM4_bits | GIE);

//  P1OUT |= RX_EN_PIN;
  return;
}

unsigned short is_power_good()
{
  return P2IN & VOLTAGE_SV_PIN;
}


//*************************************************************************
//************************ PORT 2 INTERRUPT *******************************

// Pin Setup :
// Description : Port 2 interrupt wakes on power good signal from supervisor.

#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)   // (5-6 cycles) to enter interrupt
{  
  P2IFG = 0x00;
  P2IE = 0;       // Interrupt disable
  P1IFG = 0;
  P1IE = 0;
  TACTL = 0;
  TACCTL0 = 0;
  TACCTL1 = 0;
  TAR = 0;
  state = STATE_READY;
  LPM4_EXIT;
}

#if USE_2618
#pragma vector=TIMERA0_VECTOR
#else
#pragma vector=TIMERA0_VECTOR
#endif
__interrupt void TimerA0_ISR(void)   // (5-6 cycles) to enter interrupt
{
  TACTL = 0;    // have to manually clear interrupt flag
  TACCTL0 = 0;  // have to manually clear interrupt flag
  TACCTL1 = 0;  // have to manually clear interrupt flag
  LPM4_EXIT;
}

//*************************************************************************
//************************ PORT 1 INTERRUPT *******************************

// warning   :  Whenever the clock frequency changes, the value of TAR should be changed in aesterick lines
// Pin Setup :  P1.2
// Description : Port 1 interrupt is used as finding delimeter.

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)   // (5-6 cycles) to enter interrupt
{
  

#if USE_2618
  asm("MOV TAR, R7");  // move TAR to R7(count) register (3 CYCLES)
#else
  asm("MOV TAR, R7");  // move TAR to R7(count) register (3 CYCLES)
#endif
  P1IFG = 0x00;       // 4 cycles
  TAR = 0;            // 4 cycles
  LPM4_EXIT;

  asm("CMP #0000h, R5\n");          // if (bits == 0) (1 cycle)
  asm("JEQ bit_Is_Zero_In_Port_Int\n");                // 2 cycles
  // bits != 0:
  asm("MOV #0000h, R5\n");          // bits = 0  (1 cycles)

  asm("CMP #0010h, R7\n");          //************ this is finding delimeter (12.5us)  (2 cycles)*********/   2d  ->   14
  asm("JNC delimiter_Value_Is_wrong\n");            //(2 cycles)
  asm("CMP #0040h, R7");             //************ this is finding delimeter (12.5us)  (2 cycles)*********  43H
  asm("JC  delimiter_Value_Is_wrong\n");
  asm("CLR P1IE");
#if USE_2618
  asm("BIS #8010h, TACCTL1\n");     // (5 cycles)   TACCTL1 |= CM1 + CCIE
#else
  asm("BIS #8010h, TACCTL1\n");     // (5 cycles)   TACCTL1 |= CM1 + CCIE
#endif
  asm("MOV #0004h, P1SEL\n");       // enable TimerA1    (4 cycles)
  asm("RETI\n");

  asm("delimiter_Value_Is_wrong:\n");
  asm("BIC #0004h, P1IES\n");
  asm("MOV #0000h, R5\n");          // bits = 0  (1 cycles)
  delimiterNotFound = 1;
  asm("RETI");

  asm("bit_Is_Zero_In_Port_Int:\n");                 // bits == 0
#if USE_2618
  asm("MOV #0000h, TAR\n");     // reset timer (4 cycles)
#else
  asm("MOV #0000h, TAR\n");     // reset timer (4 cycles)
#endif
  asm("BIS #0004h, P1IES\n");   // 4 cycles  change port interrupt edge to neg
  asm("INC R5\n");            // 1 cycle
  asm("RETI\n");

}
//*************************************************************************
//************************ Timer INTERRUPT *******************************

// Pin Setup :  P1.2
// Description :

#if USE_2618
#pragma vector=TIMERA1_VECTOR
#else
#pragma vector=TIMERA1_VECTOR
#endif
__interrupt void TimerA1_ISR(void)   // (6 cycles) to enter interrupt
{

    asm("MOV 0174h, R7");  // move TACCR1 to R7(count) register (3 CYCLES)
    TAR = 0;               // reset timer (4 cycles)
    TACCTL1 &= ~CCIFG;      // must manually clear interrupt flag (4 cycles)

    //<--------------up to here 26 cycles + 6 cyles of Interrupt == 32 cycles ---------------->
    asm("CMP #0003h, R5\n");      // if (bits >= 3).  it will do store bits
    asm("JGE bit_Is_Over_Three\n");
    // bit is not 3
    asm("CMP #0002h, R5\n");   // if ( bits == 2)
    asm("JEQ bit_Is_Two\n");         // if (bits == 2).

    // <----------------- bit is not 2 ------------------------------->
    asm("CMP #0001h, R5\n");      // if ( bits == 1). it will measure RTcal value.
    asm("JEQ bit_Is_One\n");          // bits == 1

    // <-------------------- this is bit == 0 case --------------------->
    asm("bit_Is_Zero_In_Timer_Int:");
    asm("CLR R6\n");
    asm("INC R5\n");        // bits++
    asm("RETI");
    // <------------------- end of bit 0  --------------------------->

    // <-------------------- this is bit == 1 case --------------------->
    asm("bit_Is_One:\n");         // bits == 1.  calculate RTcal value
    asm("MOV R7, R9\n");       // 1 cycle
    asm("RRA R7\n");    // R7(count) is divided by 2.   1 cycle
    asm("MOV #0FFFFh, R8\n");   // R8(pivot) is set to max value    1 cycle
    asm("SUB R7, R8\n");        // R8(pivot) = R8(pivot) -R7(count/2) make new R8(pivot) value     1 cycle
    asm("INC R5\n");        // bits++
    asm("CLR R6\n");
    asm("RETI\n");
    // <------------------ end of bit 1 ------------------------------>

    // <-------------------- this is bit == 2 case --------------------->
    asm("bit_Is_Two:\n");
    asm("CMP R9, R7\n");    // if (count > (R9)(180)) this is hardcoded number, so have  to change to proper value
    asm("JGE this_Is_TRcal\n");
    // this is data
    asm("this_Is_Data_Bit:\n");
    asm("ADD R8, R7\n");   // count = count + pivot
    // store bit by shifting carry flag into cmd[bits]=(dest*) and increment dest*  // (5 cycles)
    asm("ADDC.b @R4+,-1(R4)\n"); // roll left (emulated by adding to itself == multiply by 2 + carry)
    // R6 lets us know when we have 8 bits, at which point we INC dest*            // (1 cycle)
    asm("INC R6\n");
    asm("CMP #0008,R6\n\n");   // undo increment of dest* (R4) until we have 8 bits
    asm("JGE out_p\n");
    asm("DEC R4\n");
    asm("out_p:\n");           // decrement R4 if we haven't gotten 16 bits yet  (3 or 4 cycles)
    asm("BIC #0008h,R6\n");   // when R6=8, this will set R6=0   (1 cycle)
    asm("INC R5\n");
    asm("RETI");
    // <------------------ end of bit 2 ------------------------------>

    asm("this_Is_TRcal:\n");
    asm("MOV R7, R5\n");    // bits = count. use bits(R5) to assign new value of TRcal
    TRcal = bits;       // assign new value     (4 cycles)
    asm("MOV #0003h, R5\n");      // bits = 3..assign 3 to bits, so it will keep track of current bits    (2 cycles)
    asm("CLR R6\n"); // (1 cycle)
    asm("RETI");

   // <------------- this is bits >= 3 case ----------------------->
    asm("bit_Is_Over_Three:\n");     // bits >= 3 , so store bits
    asm("ADD R8, R7\n");    // R7(count) = R8(pivot) + R7(count),
    // store bit by shifting carry flag into cmd[bits]=(dest*) and increment dest*  // (5 cycles)
    asm("ADDC.b @R4+,-1(R4)\n"); // roll left (emulated by adding to itself == multiply by 2 + carry)
    // R6 lets us know when we have 8 bits, at which point we INC dest*            // (1 cycle)
    asm("INC R6\n");
    asm("CMP #0008,R6\n");   // undo increment of dest* (R4) until we have 8 bits
    asm("JGE out_p1\n");
    asm("DEC R4\n");
    asm("out_p1:\n");           // decrement R4 if we haven't gotten 16 bits yet  (3 or 4 cycles)
    asm("BIC #0008h,R6\n");   // when R6=8, this will set R6=0   (1 cycle)
    asm("INC R5\n");              // bits++
    asm("RETI\n");
    // <------------------ end of bit is over 3 ------------------------------>
}


//
//
// experimental M4 code
//
//

/******************************************************************************
*   Pin Set up
*   P1.1 - communication output
*******************************************************************************/
void sendToReader(volatile unsigned char *data, unsigned char numOfBits)
{

  SEND_CLOCK;

  TACTL &= ~TAIE;
  TAR = 0;
  // assign data address to dest
  dest = data;
  // Setup timer
  P1SEL |= TX_PIN; //  select TIMER_A0
//  P1DIR |= TX_PIN; // already set.
  TACTL |= TACLR;   //reset timer A
  TACTL = TASSEL1 + MC0;     // up mode

  TACCR0 = 5;  // this is 1 us period( 3 is 430x12x1)

  TAR = 0;
  TACCTL0 = OUTMOD2; // RESET MODE

#if MILLER_4_ENCODING
  BCSCTL2 |= DIVM_1;
#endif
  
  //TACTL |= TASSEL1 + MC1 + TAIE;
  //TACCTL1 |= SCS + CAP;	//initially, it set up as capturing rising edge.

/**************************************************************************************************
*   The starting of the transmitting code. Transmitting code must send 4 or 16 of M/LF, then send
*   010111 preamble before sending data package. TRext determines how many M/LFs are sent.
*
*   Used Register
*   R4 = CMD address, R5 = bits, R6 = counting 16 bits, R7 = 1 Word data, R9 = temp value for loop
*   R10 = temp value for the loop, R13 = 16 bits compare, R14 = timer_value for 11, R15 = timer_value for 5
***************************************************************************************************/


  //<-------------- The below code will initiate some set up ---------------------->//
    //asm("MOV #05h, R14");
    //asm("MOV #02h, R15");
    bits = TRext;           // 5 cycles
    asm("NOP");             // 1 cycles
    asm("CMP #0001h, R5");  // 1 cycles
    asm("JEQ TRextIs_1");   // 2 cycles
    asm("MOV #0004h, R9");  // 1 cycles
    asm("JMP otherSetup");  // 2 cycles

    // initialize loop for 16 M/LF
    asm("TRextIs_1:");
    asm("MOV #000fh, R9");    // 2 cycles    *** this will chagne to right value
    asm("NOP");

    //
    asm("otherSetup:");
    bits = numOfBits;         // (2 cycles).  This value will be adjusted. if numOfBit is constant, it takes 1 cycles
    asm("NOP");               // (1 cycles), zhangh 0316
    
    asm("MOV #0bh, R14");     // (2 cycles) R14 is used as timer value 11, it will be 2 us in 3 MHz
    asm("MOV #05h, R15");     // (2 cycles) R15 is used as tiemr value 5, it will be 1 us in 3 MHz
    asm("MOV @R4+, R7");      // (2 cycles) Assign data to R7
    asm("MOV #0010h, R13");   // (2 cycles) Assign decimal 16 to R13, so it will reduce the 1 cycle from below code
    asm("MOV R13, R6");       // (1 cycle)
    asm("SWPB R7");           // (1 cycle)    Swap Hi-byte and Low byte
    asm("NOP");
    asm("NOP");
    // new timing needs 11 cycles
    asm("NOP");
    //asm("NOP");       // up to here, it make 1 to 0 transition.
    //<----------------2 us --------------------------------
    asm("NOP");   // 1
    asm("NOP");   // 2
    asm("NOP");   // 3
    asm("NOP");   // 4
    asm("NOP");   // 5
    asm("NOP");   // 6
    //asm("NOP");   // 7
    //asm("NOP");   // 8
    //asm("NOP");   // 9
    // <---------- End of 1 us ------------------------------
    // The below code will create the number of M/LF.  According to the spec,
    // if the TRext is 0, there are 4 M/LF.  If the TRext is 1, there are 16 M/LF
    // The upper code executed 1 M/LF, so the count(R9) should be number of M/LF - 1
    //asm("MOV #000fh, R9");    // 2 cycles    *** this will chagne to right value
    asm("MOV #0001h, R10");   // 1 cycles, zhangh?
    // The below code will create the number base encoding waveform., so the number of count(R9) should be times of M
    // For example, if M = 2 and TRext are 1(16, the number of count should be 32.
    asm("M_LF_Count:");
    asm("NOP");   // 1
    asm("NOP");   // 2
    asm("NOP");   // 3
    asm("NOP");   // 4
    asm("NOP");   // 5
    asm("NOP");   // 6
    asm("NOP");   // 7
    asm("NOP");   // 8
    asm("NOP");   // 9
    asm("NOP");   // 10
    asm("NOP");   // 11
    asm("NOP");   // 12
    asm("NOP");   // 13
    asm("NOP");   // 14
    asm("NOP");   // 15
    asm("NOP");   // 16
    // asm("NOP");   // 17

    asm("CMP R10, R9");       // 1 cycle
    asm("JEQ M_LF_Count_End"); // 2 cycles
    asm("INC R10");           // 1 cycle
    asm("NOP");   // 22
    asm("JMP M_LF_Count");      // 2 cycles

    asm("M_LF_Count_End:");
    // this code is preamble for 010111 , but for the loop, it will only send 01011
    asm("MOV #5c00h, R9");      // 2 cycles
    asm("MOV #0006h, R10");     // 2 cycles
    
    asm("NOP");                   // 1 cycle zhangh 0316, 2
    
    // this should be counted as 0. Therefore, Assembly DEC line should be 1 after executing
    asm("Preamble_Loop:");
    asm("DEC R10");               // 1 cycle
    asm("JZ last_preamble_set");          // 2 cycle
    asm("RLC R9");                // 1 cycle
    asm("JNC preamble_Zero");     // 2 cycle      .. up to 6
    // this is 1 case for preamble
    asm("NOP");
    asm("NOP");                   // 1 cycle
#if USE_2618
    asm("MOV R14, TACCR0");       // 3 cycle      .. 10
#else
    asm("MOV R14, TACCR0");       // 3 cycle      .. 10
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");                   // 1 cycle
#if USE_2618
    asm("MOV R15, TACCR0");       // 3 cycle      .. 18
#else
    asm("MOV R15, TACCR0");       // 3 cycle      .. 18
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");                   // .. 22
    asm("JMP Preamble_Loop");     // 2 cycles   .. 24

    // this is 0 case for preamble
    asm("preamble_Zero:");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");


    asm("JMP Preamble_Loop");     // 2 cycles .. 24

    asm("last_preamble_set:");
    asm("NOP");			// 4
    asm("NOP");
    asm("NOP");    // TURN ON
    asm("NOP");
    asm("NOP");             // 1 cycle
#if USE_2618
    asm("MOV.B R14, TACCR0");// 3 cycles
#else
    asm("MOV.B R14, TACCR0");// 3 cycles
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
#if USE_2618
    asm("MOV.B R15, TACCR0");
#else
    asm("MOV.B R15, TACCR0");
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    // Here are another 4 cycles. But 3~5 cycles might also work, need to try. 
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
//    asm("NOP");


    //<------------- end of initial set up

/***********************************************************************
*   The main loop code for transmitting data in 3 MHz.  This will transmit data in real time.
*   R5(bits) and R6(word count) must be 1 bigger than desired value.
*   Ex) if you want to send 16 bits, you have to store 17 to R5.
************************************************************************/

    // this is starting of loop
    asm("LOOPAGAIN:");
    asm("DEC R5");                              // 1 cycle
    asm("JEQ Three_Cycle_Loop_End");            // 2 cycle
    //<--------------loop condition ------------
//    asm("NOP");                                 // 1 cycle
    asm("RLC R7");                              // 1 cycle
    asm("JNC bit_is_zero");	                // 2 cycles  ..6

    // bit is 1
    asm("bit_is_one:");
//    asm("NOP");                               // 1 cycle
#if USE_2618
    asm("MOV R14, TACCR0");                   // 3 cycles   ..9
#else
    asm("MOV R14, TACCR0");                   // 3 cycles   ..9
#endif                // 4 cycles   ..11
    
    asm("DEC R6");                              // 1 cycle  ..10
    asm("JNZ bit_Count_Is_Not_16");              // 2 cycle    .. 12
    // This code will assign new data from reply and then swap bytes.  After that, update R6 with 16 bits
    //asm("MOV @R4+, R7");
#if USE_2618
    asm("MOV R15, TACCR0");                   // 3 cycles   .. 15
#else
    asm("MOV R15, TACCR0");                   // 3 cycles   .. 15
#endif
    
    asm("MOV R13, R6");                         // 1 cycle   .. 16
    asm("MOV @R4+, R7");                        // 2 cycles  .. 18

    asm("SWPB R7");                             // 1 cycle    .. 19
    //asm("MOV R13, R6");                         // 1 cycle
    // End of assigning data byte
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("JMP LOOPAGAIN");                       // 2 cycle    .. 24

    asm("seq_zero:");
#if USE_2618
    asm("MOV R15, TACCR0");         // 3 cycles       ..3
#else
    asm("MOV R15, TACCR0");         // 3 cycles       ..3
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");                     // 1 cycles .. 6


    // bit is 0, so it will check that next bit is 0 or not
    asm("bit_is_zero:");				// up to 6 cycles
    asm("DEC R6");                      // 1 cycle   .. 7
    asm("JNE bit_Count_Is_Not_16_From0");           // 2 cycles  .. 9
    // bit count is 16
    asm("DEC R5");                      // 1 cycle   .. 10
    asm("JEQ Thirteen_Cycle_Loop_End");     // 2 cycle   .. 12
    // This code will assign new data from reply and then swap bytes.  After that, update R6 with 16 bits
    asm("MOV @R4+,R7");                 // 2 cycles     14
    asm("SWPB R7");                     // 1 cycle      15
    asm("MOV R13, R6");                 // 1 cycles     16
    // End of assigning new data byte
    asm("RLC R7");		        // 1 cycles     17
    asm("JC nextBitIs1");	        // 2 cycles  .. 19
    // bit is 0
#if USE_2618
    asm("MOV R14, TACCR0");             // 3 cycles  .. 22
#else
    asm("MOV R14, TACCR0");             // 3 cycles  .. 22
#endif
    // Next bit is 0 , it is 00 case	
    asm("JMP seq_zero");                // 2 cycles .. 24

// <---------this code is 00 case with no 16 bits.
    asm("bit_Count_Is_Not_16_From0:");                  // up to 9 cycles
    asm("DEC R5");                          // 1 cycle      10
    asm("JEQ Thirteen_Cycle_Loop_End");     // 2 cycle    ..12
    asm("NOP");         	            // 1 cycles    ..13
    asm("NOP");                             // 1 cycles    ..14
    asm("NOP");                             // 1 cycles    ..15
    asm("RLC R7");	                    // 1 cycle     .. 16
    asm("JC nextBitIs1");	            // 2 cycles    ..18

#if USE_2618
    asm("MOV R14, TACCR0");               // 3 cycles   .. 21
#else
    asm("MOV R14, TACCR0");               // 3 cycles   .. 21
#endif
    asm("NOP");                         // 1 cycle   .. 22
    asm("JMP seq_zero");        // 2 cycles    .. 24

// whenever current bit is 0, then next bit is 1
    asm("nextBitIs1:");     // 18
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");       // 24

    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("JMP bit_is_one");  // end of bit 0 .. 6

    asm("bit_Count_Is_Not_16:");       // up to here 14
#if USE_2618
    asm("MOV R15, TACCR0");             // 3 cycles   .. 15
#else
    asm("MOV R15, TACCR0");             // 3 cycles   .. 15
#endif
    
    asm("NOP");                               // 1 cycle .. 16
    asm("NOP");                               // 1 cycle .. 17
    asm("NOP");                               // 1 cycle .. 18
    asm("NOP");                               // 1 cycle .. 19
    asm("NOP");                               // 1 cycle .. 20
    asm("NOP");                               // 1 cycle .. 21
    asm("NOP");                               // 1 cycle .. 22
    asm("JMP LOOPAGAIN");     // 2 cycle          .. 24

    // below code is the end of loop code
    asm("Three_Cycle_Loop_End:");
    asm("JMP lastBit");     // 2 cycles   .. 5

    asm("Thirteen_Cycle_Loop_End:");  // up to 12
    asm("NOP");   // 1
    asm("NOP");   // 2
    asm("NOP");   // 3
    asm("NOP");   // 4
    asm("NOP");   // 5
    asm("NOP");   // 6
    asm("NOP");   // 7
    asm("NOP");   // 8
    asm("NOP");   // 9
    asm("NOP");   // 10
    asm("NOP");   // 11
    asm("NOP");   // 12 ..24
    asm("NOP");   // 13
    asm("NOP");   // 14
    asm("JMP lastBit"); // 16
/***********************************************************************
*   End of main loop
************************************************************************/
// this is last data 1 bit which is dummy data
    asm("lastBit:");  // up to 4
    asm("NOP");       // 5
    asm("NOP");
#if USE_2618
    asm("MOV.B R14, TACCR0");// 3 cycles
#else
    asm("MOV.B R14, TACCR0");// 3 cycles
#endif
    asm("NOP");
    asm("NOP");
    asm("NOP");
#if USE_2618
    asm("MOV.B R15, TACCR0");
#else
    asm("MOV.B R15, TACCR0");
#endif
    asm("NOP");
    asm("NOP");
    // experiment

    asm("NOP");

    //TACCR0 = 0;

    TACCTL0 = 0;  // DON'T NEED THIS NOP
    RECEIVE_CLOCK;

}


/**
 * This code comes from the Open Tag Systems Protocol Reference Guide version 1.1
 * dated 3/23/2004. (http://www.opentagsystems.com/pdfs/downloads/OTS_Protocol_v11.pdf)
 * No licensing information accompanied the code snippet.
 **/
unsigned short crc16_ccitt(volatile unsigned char *data, unsigned short n) {
  register unsigned short i, j;
  register unsigned short crc_16;

  crc_16 = 0xFFFF; // Equivalent Preset to 0x1D0F
  for (i=0; i<n; i++) {
    crc_16^=data[i] << 8;
    for (j=0;j<8;j++) {
      if (crc_16&0x8000) {
        crc_16 <<= 1;
        crc_16 ^= 0x1021; // (CCITT) x16 + x12 + x5 + 1
      }
      else {
        crc_16 <<= 1;
      }
    }
  }
  return(crc_16^0xffff);
}

inline void crc16_ccitt_readReply(unsigned int numDataBytes)
{
  
  // shift everything over by 1 to accomodate leading "0" bit.
  // first, grab address of beginning of array
  readReply[numDataBytes + 2] = 0; // clear out this spot for the loner bit of handle
  readReply[numDataBytes + 4] = 0; // clear out this spot for the loner bit of crc
  bits = (unsigned short) &readReply[0];
  // shift all bytes and later use only data + handle
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");
  asm("RRC.b @R5+");  
  // store loner bit in array[numDataBytes+2] position
  asm("RRC.b @R5+");
  // make first bit 0
  readReply[0] &= 0x7f;
  
  // compute crc on data + handle bytes
  readReplyCRC = crc16_ccitt(&readReply[0], numDataBytes + 2);
  readReply[numDataBytes + 4] = readReply[numDataBytes + 2];
  // XOR the MSB of CRC with loner bit.
  readReply[numDataBytes + 4] ^= __swap_bytes(readReplyCRC); // XOR happens with MSB of lower nibble
  // Just take the resulting bit, not the whole byte
  readReply[numDataBytes + 4] &= 0x80;
  
  unsigned short mask = __swap_bytes(readReply[numDataBytes + 4]);
  mask >>= 3;
  mask |= (mask >> 7);
  mask ^= 0x1020;
  mask >>= 1;  // this is because the loner bit pushes the CRC to the left by 1
  // but we don't shift the crc because it should get pushed out by 1 anyway
  readReplyCRC ^= mask;
  
  readReply[numDataBytes + 3] = (unsigned char) readReplyCRC;
  readReply[numDataBytes + 2] |= (unsigned char) (__swap_bytes(readReplyCRC) & 0x7F);
}

#if 0
// not used now, but will need for later
unsigned char crc5(volatile unsigned char *buf, unsigned short numOfBits)
{
  register unsigned char shift;
  register unsigned char data, val;
  register unsigned short i;
  shift = 0x48;
  for (i = 0; i < numOfBits; i++)
  {
    if ( (i%8) == 0)
      data = *buf++;
    val = shift ^ data;
    shift = shift << 1;
    data = data << 1;
    if (val&0x80)
      shift = shift ^ POLY5;
  }
  shift = shift >>3;
  return (unsigned char)(shift);
}
#endif

#if ENABLE_SLOTS

void lfsr()
{ 
    // calculate LFSR
    rn16 = (rn16 << 1) | (((rn16 >> 15) ^ (rn16 >> 13) ^ (rn16 >> 9) ^ (rn16 >> 8)) & 1);
    rn16 = rn16 & 0xFFFF;
    
    // fit 2^Q-1
    rn16 = rn16>>(15-Q);
}

inline void loadRN16()
{
#if 1
  if (Q > 8)
  {
    queryReply[0] = RN16[(Q<<1)-9];
    queryReply[1] = RN16[(Q<<1)-8];
  }
  else
  {
    int index = ((Q+shift) & 0xF);
    queryReply[0] = RN16[index];
    queryReply[1] = RN16[index+1];
  }
#else
    queryReply[0] = 0xf0;
    queryReply[1] = 0x0f;
#endif
  
}

inline void mixupRN16()
{
  unsigned short tmp;
  unsigned short newQ = 0;
  unsigned short swapee_index = 0;
  
  newQ = RN16[shift] & 0xF;
  swapee_index = RN16[newQ];
  tmp = RN16[shift];
  RN16[Q] = RN16[swapee_index];
  RN16[swapee_index] = tmp;  
}


#endif

#if ENABLE_SESSIONS
// initialize sessions for power-on
void initialize_sessions()
{

	SL = SL_NOT_ASSERTED;	// selected flag powers up deasserted
	// all inventory flags power up in state 'A'
	session_table[S0_INDEX] = SESSION_STATE_A;
        session_table[S1_INDEX] = SESSION_STATE_A;
        session_table[S2_INDEX] = SESSION_STATE_A;
        session_table[S3_INDEX] = SESSION_STATE_A;
}
void handle_session_timeout() 
{
#if 0
	// selected flag is persistent throughout power up state
	// S0 inventory flag is persistent throughout power up state
	// S1 is reset, unless it is in the middle of an inventory round
	if ( ! inInventoryRound ) session_table[S1_INDEX] = SESSION_STATE_A;
	// S2 and S3 are always refreshed
	session_table[S2_INDEX] = SESSION_STATE_A;
	session_table[S3_INDEX] = SESSION_STATE_A;
#endif
}
#endif

#if ENABLE_SESSIONS
// compare two chunks of memory, starting at given bit offsets (relative to the starting byte, that is).
// startingBitX is a range from 7 (MSbit) to 0 (LSbit). Len is number of bits. Returns a
// 1 if they match and a 0 if they don't match.
int bitCompare(unsigned char *startingByte1, unsigned short startingBit1,
		unsigned char *startingByte2, unsigned short startingBit2,
		unsigned short len) {
                  
        unsigned char test1, test2;

	while ( len-- ) {

		test1 = (*startingByte1) & ( 1 << startingBit1 );
		test2 = (*startingByte2) & ( 1 << startingBit2 );

		if ( (test1 == 0 && test2 != 0) || ( test1 != 0 && test2 == 0 ) ) {
			return 0;
		}

		if ( len == 0 ) return 1;

		if ( startingBit1 == 0 ) {
			startingByte1++;
			startingBit1 = 7;
		}
		else
			startingBit1--;

		if ( startingBit2 == 0 ) {
			startingByte2++;
			startingBit2 = 7;
		}
		else
			startingBit2--;

	}
        
        return 1;
}
#endif
