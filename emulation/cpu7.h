//
// VLN (variable length number) is a method of expressing numeric values with unknown bit length as blocks of
// 16-bit data words. Each data word contains 14 bits of actual data, plus two modifier flags. Thus the ideal
// data length for VLN is in multiples of 14 bits: 14, 28, 42, 56, etc.
// Actual data chunks occupy bits 0 through 13 in every 16-bit data word.
//
// ============================================================================================================
// CPU7 ADDRESS MAP
//
// flat memory map
// +------------------------------------+ .....
// |                                    |
// |                                    |
// | program memory, external resources |
// |     peripheral registers, etc.     |
// |     (RAM or ROM, 16-bit words)     |
// |                                    |
// |                                    |
// +------------------------------------+ 0x0000000
//
// hardware resources not part of the memory map
// +------------------------------------+ 
// | call stacks (28-bit addresses,     |
// |  each stored in two 16-bit words)  |
// +------------------------------------+
// | data stacks (56-bit values,        |
// |  each stored in four 16-bit words) |
// +------------------------------------+
//
// NOTE: code execution always starts from virtual address 0 after reset or an error
//
//
// ============================================================================================================
//
//
// CPU register map
//
//                                                   +-------+
//                      7-bit process index register |  PXR  |
// +-------------------------------------------------+-------+
// |   56-bit DLYC (free-running incremental delay counter)  |
// +---------------------------------------------------------+
//
//                                    . . . . . . . . . . . . . . ..
//                                  . . . . . . . . . . . . . . .. .
//                                . . . . . . . . . . . . . . .. . .
//                              +----------------------------+ . . .
//         program code pointer |         28-bit PCP        0| . . .
// +----------------------------+----------------------------+ . . .
// |         56-bit CAR (conditional action register)      **| . . .
// +----------------------------+----------------------------+ . . .
// |                    56-bit register R0 (A)               | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register R1 (B)               | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register R2 (C)               | . . .
// +------------------------------------------+--------------+ . . .
//                                            | 14-bit R3 (D)| . . .
//                                            +--------------+ . . .
//                                            | 14-bit R4 (E)| . . .
//                                            +--------------+ . . .
//                  instruction code register | 14-bit ICR   | . . .
//                                            +--------------+ . . .
//                         call stack pointer | 14-b CSP   00| . . .
//                                            +--------------+ . . .
//                         data stack pointer | 14-b DSP  000| . . .
//                                            +--------------+ . . .
//                data stack pointer snapshot | 14-b SSP  000| . . .
//                                            +--------------+ . . .
//                   data stack depth counter | 14-bit DDC   | . . .
//                                            +--------------+ . . .
//          data stack depth counter snapshot | 14-bit SDC   | . . .
//                                            +--------------+ . . .
//                  process priority register | 14-bit PPR   | . . .
// +------------------------------------------+--------------+ . . .
// |             56-bit DCR (delay compare register)         | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V0                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V1                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V2                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V3                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V4                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V5                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V6                   | . . .
// +---------------------------------------------------------+ . . .
// |                    56-bit register V7                   | . .
// +---------------------------------------------------------+ .
//
// ============================================================================================================

#ifndef _CPU_H
#define _CPU_H

#include "GenericTypeDefs.h"
#include "system.h"     // needed for the MEM[] structure

// ============================================================================================================
// CPU7 constants

// number of V-registers in this realisation
#define VREGS       8

// error codes
#define ERR_INVALID 0x100   // invalid instruction code
#define ERR_COLD    0x101   // cold start
#define ERR_ALIGN   0x102   // alignment error
#define ERR_INVMEM  0x103   // invalid memory location
#define ERR_DSINDEX 0x104   // invalid data stack index
#define ERR_DSFULL  0x105   // data stack overflow
#define ERR_DSEMPTY 0x106   // data stack underflow
#define ERR_CSFULL  0x107   // call stack overflow
#define ERR_CSEMPTY 0x108   // call stack underflow
#define ERR_CALC    0x109   // arithmetic error (eg. division by 0)
#define ERR_ECST    0x10a   // error in conditional structure
                            // ELSE or ENDIF without an opening IF
                            // UNTIL without an opening REPEAT
#define ERR_DBLENT  0x10b   // double entering error
#define ERR_NOENT   0x10c   // LEAVE without preceding ENTER

// word types
#define WT_MASK 0xc000      // general bitmask for the word type constants
#define WT_CPU  0x0000      // code word containing two 7-bit CPU instructions
#define WT_DNL  0x4000      // data word, not last in a VLN
#define WT_DL   0x8000      // data word, last in a VLN
#define WT_IGN  0xc000      // ignored word (used by the Torth compiler to store labels)

// bitmasks
#define MASK7  0x000000000000007fULL
#define MASK14 0x0000000000003fffULL
#define OVER14 0x00ffffffffffc000ULL
#define MASK28 0x000000000fffffffULL
#define OVER28 0x00fffffff0000000ULL
#define MASK56 0x00ffffffffffffffULL
#define OVER56 0xff00000000000000ULL

// additional masks for 'standard' width raw data
#define MASK8  0x00000000000000ffULL
#define OVER8  0xffffffffffffff00ULL
#define MASK16 0x000000000000ffffULL
#define OVER16 0xffffffffffff0000ULL
#define OVER24 0xffffffffff000000ULL
#define MASK32 0x00000000ffffffffULL
#define OVER32 0xffffffff00000000ULL
#define OVER40 0xffffff0000000000ULL
#define OVER48 0xffff000000000000ULL

// CPU7 register aliases
#define A cpu[core].r56[0]
#define B cpu[core].r56[1]
#define C cpu[core].r56[2]
#define D cpu[core].r14[0]
#define E cpu[core].r14[1]

// CAR bitmask definitions
#define CA_LENGTH   2   // number of bits of the CA_xx condition
#define CA_MASK     3   // general bitmask
#define CA_NONE     0   // normal execution (no conditional structure)
#define CA_SKIP     1   // skipping mode
#define CA_NOEXEC   2   // conditional structure; not executing code
#define CA_EXEC     3   // conditional structure; executing code

// CPU register structure
typedef struct {
    UINT32 pcp;     // program code pointer
    UINT16 csp;     // call stack pointer
    UINT16 dsp;     // data stack pointer
    UINT16 ssp;     // snapshot pointer    
    UINT32 car;     // only the two lowest order bits are monitored to determine the current condition
                    // see the CA_xxx definitions
    UINT64 dcr;     // delay compare register, kept 0 if there is no active delay, otherwise contains the compare
                    // value for the DLYC

    UINT64 r56[3];  // registers R0, R1 and R2
    UINT16 r14[2];  // registers R3 and R4    
    UINT16 icr;     // instruction code register
                    // contains two 7-bit instruction codes with the one in the lower
                    // 7 bits executed first, and the one in the higher 7 bits executed second
    UINT16 ddc;     // data stack depth counter
    UINT16 sdc;     // snapshot data stack counter
    UINT16 ppr;     // process priority register
    UINT64 v[VREGS]; // register block Vx
} cpu_t;

extern volatile UINT8 pxr;          // currently running core
extern volatile UINT64 dlyc;        // delay counter; free running incremental counter
extern volatile cpu_t cpu[CORES];   // CPU register structure

#define core pxr    // PXR alias

// definitions for indexes of the 56-bit accumulators in cpu[].r56[]
#define ACCA 0
#define ACCB 1
#define ACCC 2


// instruction words as text
// each instruction must not be longer than 7 characters
// the index in this array corresponds with the instruction code
// this array is not used by the CPU and is only here for consistency and easier adding/removal of
// new instructions in future
extern const CHAR8 instw[128][8];


// push selected accumulator as a variable length number (VLN) into the data stack
// input
// (r) R56 index, range 0..2, uncontrolled
void push(UINT8 r);

// pull a VLN from the data stack and load it into selected accumulator
// input
// (r) R56 index, range 0..2, uncontrolled
// output
// cpu[pxr].r56[r] returned value
void pull(UINT8 r);

// execute a single CPU instruction
// input
// (code) 7-bit instruction code, uncontrolled
void cpu_instr(UINT8 code);

// start CPU code execution
void cpu_run(void);

#endif
