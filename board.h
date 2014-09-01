#ifndef __MOO_BOARD_H__
#define __MOO_BOARD_H__
#include "version.h"

#if MOO_VERSION == MOO1_1
#include "board_1_1.h"
#elif MOO_VERSION == MOO1_2
#include "board_1_2.h"
#elif MOO_VERSION == MOO1_2_1
#include "board_1_2_1.h"
#else
#error "Moo Version Not Supported"
#endif

#endif /* __MOO_BOARD_H__ */
