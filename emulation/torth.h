//
// Torth macrosyntax rules
//
// :LABEL [@addr] ...............  ;
//
// .LABEL   create a constant with the memory address of LABEL
// _LABEL   call LABEL (equivalent to .LABEL CALL )
// _!LABEL  call LABEL (equivalent to .LABEL NTCALL )
// &LABEL   call LABEL (equivalent to .LABEL ACALL )
// &!LABEL  call LABEL (equivalent to .LABEL NTACALL )
//
// ` ....... commentaries ....... `
// `! ...... commentaries until the end of the current line
//
// " filename " INCLUDE     include Torth source from an external file (enabled with FILE_OPERATIONS)
// " filename " LIBRARY     include p-code from an external file (enabled with FILE_OPERATIONS)
//
// ============================================================================================================

#ifndef _TORTH_H
#define _TORTH_H

#include <GenericTypeDefs.h>

#define FILE_OPERATIONS     // this definition enables the use of INCLUDE and LIBRARY
                            // comment it out to disable all file operations

// compilation error codes
#define TORTH_NOERR     0   // no error, successful compilation
#define TORTH_INTERR    1   // internal error
#define TORTH_INVADDR   2   // invalid memory address
#define TORTH_NOCQ      3   // missing string closing quote
#define TORTH_INVSCHR   4   // invalid 'special' character
#define TORTH_INVNUM    5   // invalid numeric constant
#define TORTH_INVATOM   6   // invalid or unknown atomic word
#define TORTH_INVLBL    7   // invalid label
#define TORTH_REPEATLBL 8   // repeated label
#define TORTH_UNKNLBL   9   // unknown label
#define TORTH_ADDRREQ   10  // address @ field is required
#define TORTH_CONSTREQ  11  // constant string is required
#define TORTH_FILEERR   12  // file operation error (open or read)
#define TORTH_NOMEM     13  // program is longer than the available memory

#define CODE_RETURN 0x04    // code of the instruction RETURN (refer to "cpu7.h" and "cpu7.c")


// compile Torth source code into p-code
// input
// (**source) pointer to the source code in text form
// (*addr) originating virtual address for the compiled code (must be even number)
// output
// (*source) new value of the source
// (addr) current virtual address
// torth_compile() error code
UINT8 torth_compile(CHAR8 **source, UINT32 *addr);

#endif
