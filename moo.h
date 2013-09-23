/* See license.txt for license information. */

#ifndef MOO_H
#define MOO_H

// Hardware definitions for Moo 1.0 and 1.1 (MSP430F2618)
// (derived from Intel WISP 4.1 DL ["Blue WISP"] definitions)
// See the schematics/ directory for pinouts and diagrams.

#include "board.h"
#include "states.h"


void setup_to_receive();
void sleep();
unsigned short is_power_good();
#if ENABLE_SLOTS
void lfsr();
void loadRN16(), mixupRN16();
#endif // ENABLE_SLOTS
void crc16_ccitt_readReply(unsigned int);

#endif // MOO_H
