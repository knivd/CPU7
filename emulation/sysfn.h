//
// External function calls
//
// These functions are *** HARDWARE DEPENDENT *** and are written for the ELLO 16p computer
//
// ============================================================================================================

#ifndef _SYSFN_H
#define _SYSFN_H

#include "GenericTypeDefs.h"

// execute a system function call with code X
// input parameters (if any) are taken from the data stack, and the output (if any) is also returned there
void sysfn(UINT64 x);

#endif
