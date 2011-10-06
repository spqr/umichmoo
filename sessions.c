/* See license.txt for license information. */

#include "mymoo.h"
#if ENABLE_SESSIONS

unsigned char SL;
unsigned char previous_session = 0x00;
unsigned char session_table[] = {
    SESSION_STATE_A, SESSION_STATE_A,
    SESSION_STATE_A, SESSION_STATE_A
};

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

// compare two chunks of memory, starting at given bit offsets (relative to the
// starting byte, that is).
// startingBitX is a range from 7 (MSbit) to 0 (LSbit). Len is number of bits.
// Returns a 1 if they match and a 0 if they don't match.
inline int bitCompare(unsigned char *startingByte1,
		      unsigned short startingBit1,
		      unsigned char *startingByte2,
		      unsigned short startingBit2,
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

#endif // ENABLE_SESSIONS
