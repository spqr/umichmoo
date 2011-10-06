/* See license.txt for license information. */

#ifndef SESSIONS_H
#define SESSIONS_H

// selected and session inventory flags
#define S0_INDEX		0x00
#define S1_INDEX		0x01
#define S2_INDEX		0x02
#define S3_INDEX		0x03

#define SL_ASSERTED		1
#define SL_NOT_ASSERTED		0
#define SESSION_STATE_A		0
#define SESSION_STATE_B		1

extern unsigned char SL;
extern unsigned char previous_session;
extern unsigned char session_table[];

void initialize_sessions ();
void handle_session_timeout ();
int bitCompare (unsigned char *startingByte1,
                unsigned short startingBit1,
                unsigned char *startingByte2,
                unsigned short startingBit2,
                unsigned short len);

#endif // __SESSIONS_H