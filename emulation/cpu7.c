// coding rules
// no use of variables is allowed except of the (cpu{}) structure and (core)
// use of constants is allowed but theirs number should be minimal

#include <setjmp.h>     // this library requires a heap!
#include <stdlib.h>
#include "GenericTypeDefs.h"

// ============================================================================================================
// CPU instructions code table

void i_(void);
void i_NOP(void);
void i_SKIP(void);
void i_DO(void);
void i_RETURN(void);
void i_CALL(void);
void i_ACALL(void);
void i_NTCALL(void);
void i_NTACALL(void);
void i_END(void);
void i_ENDALL(void);
void i_MAXTHDS(void);
void i_THREADS(void);
void i_DUP(void);
void i_DROP(void);
void i_SWAP(void);
void i_ROT(void);
void i_OVER(void);
void i_DEPTH(void);
void i_EMPTY(void);
void i_AND(void);
void i_OR(void);
void i_XOR(void);
void i_COM(void);
void i_NOT(void);
void i_SHL(void);
void i_SHR(void);
void i_ADD(void);
void i_SUB(void);
void i_MUL(void);
void i_DIV(void);
void i_MOD(void);
void i_EQ(void);
void i_NEQ(void);
void i_GT(void);
void i_GTEQ(void);
void i_SM(void);
void i_SMEQ(void);
void i_INC(void);
void i_DEC(void);
void i_IF(void);
void i_ELSE(void);
void i_ENDIF(void);
void i_REPEAT(void);
void i_REPIF(void);
void i_UNTIL(void);
void i_WHILE(void);
void i_AGAIN(void);
void i_BREAK(void);
void i_RD8(void);
void i_RD16(void);
void i_RD32(void);
void i_RDVLN(void);
void i_WR8(void);
void i_WR16(void);
void i_WR32(void);
void i_WRVLN(void);
void i_RANDOM(void);
void i_MEMCOPY(void);
void i_STRCOPY(void);
void i_MEMDIFF(void);
void i_STRDIFF(void);
void i_MEMFILL(void);
void i_STRLEN(void);
void i_STRSCAN(void);
void i_SYSFN(void);
void i_DELAY(void);
void i_ENTER(void);
void i_LEAVE(void);
void i_SETPR(void);
void i_GETVAR(void);
void i_SETVAR(void);


#define UCX_GROUP 0x70  // define the starting code of the unconditionally executed instructions

// instruction codes
// the index in this array corresponds with the instruction code
void (*const instc[128])(void) = {
    i_,         i_SETPR,    i_,         i_BREAK,    i_AGAIN,    i_CALL,     i_ACALL,    i_RETURN,
    i_MAXTHDS,  i_THREADS,  i_,         i_ENDALL,   i_END,      i_NTCALL,   i_NTACALL,  i_SYSFN,
    i_,         i_EMPTY,    i_DEPTH,    i_DROP,     i_DUP,      i_SWAP,     i_ROT,      i_OVER,
    i_ENTER,    i_LEAVE,    i_GETVAR,   i_SETVAR,   i_DELAY,    i_,         i_,         i_,
    i_COM,      i_NOT,      i_AND,      i_OR,       i_XOR,      i_,         i_SHL,      i_SHR,
    i_SM,       i_SMEQ,     i_EQ,       i_NEQ,      i_GTEQ,     i_GT,       i_,         i_,
    i_,         i_,         i_,         i_,         i_,         i_,         i_,         i_,
    i_,         i_,         i_,         i_,         i_,         i_,         i_,         i_,
    i_ADD,      i_,         i_SUB,      i_,         i_MUL,      i_,         i_DIV,      i_MOD,
    i_INC,      i_DEC,      i_,         i_RANDOM,   i_,         i_,         i_,         i_,
    i_MEMFILL,  i_,         i_MEMDIFF,  i_MEMCOPY,  i_,         i_,         i_,         i_,
    i_STRLEN,   i_STRSCAN,  i_STRDIFF,  i_STRCOPY,  i_,         i_,         i_,         i_,
    i_RDVLN,    i_,         i_,         i_,         i_RD32,     i_RD16,     i_RD8,      i_,
    i_WRVLN,    i_,         i_,         i_,         i_WR32,     i_WR16,     i_WR8,      i_,
// NOTE: the last group of codes is not checked for conditional execution
    i_DO,       i_SKIP,     i_,         i_,         i_,         i_,         i_,         i_,
    i_REPEAT,   i_UNTIL,    i_WHILE,    i_REPIF,    i_IF,       i_ENDIF,    i_ELSE,     i_NOP
// NOTE: the instruction NOP is fixed at code 0x7f
};

// instruction words as text
// each instruction must not be longer than 7 characters
// the index in this array corresponds with the instruction code
// this array is not used by CPU7 and is only here for consistency and easier adding/removal of
// new instructions in future. It may be used however by the Torth compiler.
const CHAR8 instw[128][8] = {
          "",    "SETPR",         "",    "BREAK",    "AGAIN",     "CALL",    "ACALL",   "RETURN",
   "MAXTHDS",  "THREADS",         "",   "ENDALL",      "END",   "NTCALL",  "NTACALL",    "SYSFN",
          "",    "EMPTY",    "DEPTH",     "DROP",      "DUP",     "SWAP",      "ROT",     "OVER",
     "ENTER",    "LEAVE",        "!",       "=!",    "DELAY",         "",         "",         "",
       "COM",      "NOT",      "AND",       "OR",      "XOR",         "",      "SHL",      "SHR",
         "<",       "<=",       "==",       "<>",       ">=",        ">",         "",         "",
          "",         "",         "",         "",         "",         "",         "",         "",
          "",         "",         "",         "",         "",         "",         "",         "",
         "+",         "",        "-",         "",        "*",         "",        "/",       "//",
        "++",       "--",         "",   "RANDOM",         "",         "",         "",         "",
      "FILL",         "",     "DIFF",        "=",         "",         "",         "",         "",
      "LEN$",    "SCAN$",    "DIFF$",       "=$",         "",         "",         "",         "",
          "",         "",         "",         "",     "RD32",     "RD16",      "RD8",         "",
          "",         "",         "",         "",     "WR32",     "WR16",      "WR8",         "",
        "DO",     "SKIP",         "",         "",         "",         "",         "",         "",
    "REPEAT",    "UNTIL",    "WHILE",    "REPIF",       "IF",    "ENDIF",     "ELSE",      "NOP"
};

// ============================================================================================================

#include "system.h"     // CPU7 definitions and references to the host system
#include "cpu7.h"

#ifdef SYSFN_ENABLE
#include "sysfn.h"
#endif

// macro for verification of conditional code execution
#define EXECUTING ((cpu[core].car&CA_MASK)==CA_NONE || (cpu[core].car&CA_MASK)==CA_EXEC)

// CPU7 working structures
volatile UINT8 pxr;         // current running core
volatile UINT64 dlyc;       // delay counter; free running incremental counter
volatile cpu_t cpu[CORES];  // register structures
volatile UINT32 call_stack[CORES][CALL_STACK_SIZE/4];
volatile UINT32 data_stack[CORES][DATA_STACK_SIZE/4];

// needed for error handling
jmp_buf envbuf;


// push selected 56-bit accumulator into the data stack
// input
// (r) R56 index, range 0..2, uncontrolled
void push(UINT8 r) {
    if(cpu[core].dsp>=DATA_STACK_SIZE) longjmp(envbuf,ERR_DSFULL);  // reset
    data_stack[core][cpu[core].dsp>>2]=(cpu[core].r56[r]&MASK28);
    data_stack[core][(cpu[core].dsp+4)>>2]=((cpu[core].r56[r]>>28)&MASK28);
    cpu[core].dsp+=8;
    cpu[core].ddc++;
}


// pull a 56-bit number from the data stack and load it into selected accumulator
// input
// (r) R56 index, range 0..2, uncontrolled
// output
// cpu[pxr].r56[r] returned value
void pull(UINT8 r) {
    if(cpu[core].dsp<8) longjmp(envbuf,ERR_DSEMPTY);    // reset
    cpu[core].dsp-=8;
    cpu[core].r56[r]=((UINT64)(data_stack[core][(cpu[core].dsp+4)>>2])<<28);
    cpu[core].r56[r]|=(UINT64)(data_stack[core][cpu[core].dsp>>2]);
    cpu[core].ddc--;
}


// push R0 (lower 28 bits only) into the call stack
void pushca(void) {
    if(cpu[core].csp>=CALL_STACK_SIZE) longjmp(envbuf,ERR_CSFULL);  // reset
    call_stack[core][cpu[core].csp>>2]=(A&MASK28);
    cpu[core].csp+=4;
}


// pull R0 (lower 28 bits only) from the call stack
void pullca(void) {
    if(cpu[core].csp<4) longjmp(envbuf,ERR_CSEMPTY);    // reset
    cpu[core].csp-=4;
    A=call_stack[core][cpu[core].csp>>2]&MASK28;
}


// execute a single CPU instruction
// input
// (code) 7-bit instruction code, uncontrolled
void cpu_instr(UINT8 code) {
    code&=MASK7;
    if(code>=UCX_GROUP || EXECUTING) instc[code]();
}


// start CPU code execution
// the same code is executed during reset/error but the entry is not
// by calling the function but by longjmp(envbuf,(error code constant)) instead
// error code is pushed into the data stack and made available to the program further
void cpu_run(void) {
    core=0;
    A=ERR_COLD;
    setjmp(envbuf);
    cpu[0].r56[0]=cpu[core].r56[0]; // copy A from the current core to core 0.A
    if(cpu[0].r56[0]!=ERR_COLD) cpu[0].r56[1]=(cpu[core].pcp-2); else cpu[0].r56[1]=0;  // copy PCP from the current core to core 0.B
    core=(UINT8)-1;
    while(++core<CORES) {
        cpu[core].csp=0;
        cpu[core].dsp=0;
        cpu[core].ssp=0;
        cpu[core].ddc=0;
        cpu[core].sdc=0;
        cpu[core].car=CA_NONE;
        cpu[core].dcr=0;
        cpu[core].pcp=0;
        cpu[core].ppr=0;    // this indicates that the core is not running
    }
    dlyc=0;
    core=0;             // core 0 starts after reset
    cpu[core].ppr=1;    // enable core 0
    push(ACCB);         // push the address
    push(ACCA);         // push the error code
    for(;;) {
        A=0;    // accumulated value
        D=0;    // shift bit counter
        do {
            if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
            B=(UINT64)MEM16RD(cpu[core].pcp);
            cpu[core].pcp+=2;       // increase PCP to point to the next word
            A=(((B&MASK14)<<D) | A);
            D+=14;
        } while((B&WT_MASK)==WT_DNL);
        if((B&WT_MASK)==WT_CPU) {   // execute CPU instruction(s) from the read value
            cpu[core].icr=(B&MASK14);
            cpu_instr(cpu[core].icr);
            cpu_instr(cpu[core].icr>>7);
        }
        else if((B&WT_MASK)!=WT_IGN && EXECUTING) push(ACCA);   // push VLN input data
        if(CORES>1) {   // switch to the next active core
            do {
                core=((++core<CORES)? core : 0);
                while(cpu[core].ppr==0) core=((++core<CORES)? core : 0);
            } while(cpu[core].dcr && (dlyc<cpu[core].dcr)); // ignore the threads that are executing delays
            cpu[core].dcr=0;
        }
    }
}

// ============================================================================================================
// CPU instructions microcode

// invalid instruction code
void i_(void) {
    longjmp(envbuf,ERR_INVALID);    // reset
}


// nop
// no operation
void i_NOP(void) {}


// skip
// enable skipping mode
// all following instructions will be skipped until the first DO
void i_SKIP(void) {
    if(EXECUTING) {
        pull(ACCA);
        cpu[core].pcp+=A;
        cpu[core].car<<=CA_LENGTH;
        cpu[core].car|=CA_SKIP;
    }
}


// do
// cancel skipping mode and return to normal execution
void i_DO(void) {
    if((cpu[core].car&CA_MASK)==CA_SKIP) cpu[core].car>>=CA_LENGTH;
}


// return or ;
// restore PCP from the call stack
void i_RETURN(void) {
    do {
        pullca();
        if(A&1) cpu[core].car>>=CA_LENGTH;
    } while(A&1);   // remove all 'marked' addresses before the actual return address, i.e. those for the loop structures
    cpu[core].pcp=A;
    if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
}


// end
// end the current thread and reset the CPU registers associated with it
// this instruction performs a normal RETURN operation with core 0
void i_END(void) {
    if(core) {
        cpu[core].csp=0;
        cpu[core].dsp=0;
        cpu[core].ssp=0;
        cpu[core].ddc=0;
        cpu[core].sdc=0;
        cpu[core].car=CA_NONE;
        cpu[core].dcr=0;
        cpu[core].pcp=0;
        cpu[core].ppr=0;    // this indicates that the core is not running
    } else {
        do {
            pullca();
            if(A&1) cpu[core].car>>=CA_LENGTH;
        } while(A&1);   // remove all 'marked' addresses before the actual return address, i.e. those for the loop structures
        cpu[core].pcp=A;
        if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
    }
}


// endall
// end all threads except for thread 0, and reset the CPU registers associated with them
void i_ENDALL(void) {
    core=CORES;
    while(--core) {
        cpu[core].csp=0;
        cpu[core].dsp=0;
        cpu[core].ssp=0;
        cpu[core].ddc=0;
        cpu[core].sdc=0;
        cpu[core].car=CA_NONE;
        cpu[core].dcr=0;
        cpu[core].pcp=0;
        cpu[core].ppr=0;    // this indicates that the core is not running
    }
}


// x call
// store the current PCP into call stack and call code located at relative address (PCP-X)
// note that the X parameter is relative to the current location and the resulting address
// is always lower than the current one
void i_CALL(void) {
    A=cpu[core].pcp;
    pushca();
    pull(ACCA);
    cpu[core].pcp=(cpu[core].pcp-A);
    if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
}


// x acall
// store the current PCP into call stack and call code located at absolute address X
// this instruction MUST BE AVOIDED at any cost and generally speaking only exists to
// allow the implementation of built-in ROM library in future
void i_ACALL(void) {
    A=cpu[core].pcp;
    pushca();
    pull(ACCA);
    if(A&1) longjmp(envbuf,ERR_ALIGN);  // reset
    cpu[core].pcp=A;
    if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
}


// x ntcall
// create a new thread, store the current PCP into call stack and call code located at
// relative address (PCP-X)
// if no core is available for the new thread, core 0 will be used unconditionally
// note that the X parameter is relative to the current location and the resulting address
// is always lower than the current one
void i_NTCALL(void) {
    pull(ACCA);
    B=cpu[core].pcp;
    core=(CORES-1);
    while(core && cpu[core].ppr) core--;    // find an available core
    A=(B-A);
    if(A&1) longjmp(envbuf,ERR_ALIGN);  // reset
    cpu[core].pcp=A;
    if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
}


// x ntacall
// create a new thread, store the current PCP into call stack and call code located at
// absolute address X
// if no core is available for the new thread, core 0 will be used unconditionally
// this instruction MUST BE AVOIDED at any cost and generally speaking only exists to
// allow the implementation of built-in ROM library in future
void i_NTACALL(void) {
    pull(ACCA);
    core=(CORES-1);
    while(core && cpu[core].ppr) core--;    // find an available core
    if(A&1) longjmp(envbuf,ERR_ALIGN);      // reset
    cpu[core].pcp=A;
    if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
}


//  maxthds
// return the number of serviceable threads in the system (number of cores)
void i_MAXTHDS(void) {
    A=CORES;
    push(ACCA);
}


// threads
// return into the stack the number of the currently running threads (occupied cores)
void i_THREADS(void) {
    A=0;
    D=0;
    while((D<CORES) && cpu[D].ppr) A++;
    push(ACCA);
}


// dup
// duplicate the top of the stack
void i_DUP(void) {
    pull(ACCA);
    push(ACCA);
    push(ACCA);
}


// drop
// remove the top of the stack
void i_DROP(void) {
    pull(ACCA);
}


// x y z rot
// rotate the top three stack elements x y z --> y z x
void i_ROT(void) {
    pull(ACCC);     // Z
    pull(ACCB);     // Y
    pull(ACCA);     // X
    push(ACCB);
    push(ACCC);
    push(ACCA);
}


// x swap
// swap the top element (after retrieving the parameter) with the one at position x; x>=0, with 0 being the top
void i_SWAP(void) {
    pull(ACCA); // get the parameter X
    if(A>=cpu[core].ddc) longjmp(envbuf,ERR_DSINDEX);   // reset
    pull(ACCB); // get the "top of the stack"
    C=cpu[core].dsp;
    cpu[core].dsp-=(A<<3);
    pull(ACCA); // get the "internal element"
    push(ACCB);
    cpu[core].dsp=C;
    push(ACCA);
}


// x over
// copy the X-th in depth (starting from 0 being the top) stack element on the top
void i_OVER(void) {
    pull(ACCA); // get the parameter X
    if(A>=cpu[core].ddc) longjmp(envbuf,ERR_DSINDEX);   // reset
    C=cpu[core].dsp;
    cpu[core].dsp-=(A<<3);
    pull(ACCA); // get the "internal element"
    cpu[core].dsp=C;
    push(ACCA);
}


// return the number of elements currently in the data stack
void i_DEPTH(void) {
    A=(cpu[core].ddc+1);
    push(ACCA);
}


// clear the data stack
void i_EMPTY(void) {
    cpu[core].dsp=0;
    cpu[core].ssp=0;
    cpu[core].ddc=0;
    cpu[core].sdc=0;
}


// x y and
// logical AND operation on X and Y
void i_AND(void) {
    pull(ACCB);
    pull(ACCA);
    A&=B;
    push(ACCA);
}


// x y or
// logical OR operation on X and Y
void i_OR(void) {
    pull(ACCB);
    pull(ACCA);
    A|=B;
    push(ACCA);
}


// x y xor
// logical XOR operation on X and Y
void i_XOR(void) {
    pull(ACCB);
    pull(ACCA);
    A^=B;
    push(ACCA);
}


// x com
// return the complementary form of X (signed X)
void i_COM(void) {
    pull(ACCA);
    A=((~A)+1);
    push(ACCA);
}


// x not
// logical NOT operation on X
void i_NOT(void) {
    pull(ACCA);
    A=~A;
    push(ACCA);
}


// x y <<
// bitwise shift left of X by Y positions
void i_SHL(void) {
    pull(ACCB);
    pull(ACCA);
    A<<=B;
    push(ACCA);
}


// x y >>
// bitwise shift right of X by Y positions
void i_SHR(void) {
    pull(ACCB);
    pull(ACCA);
    A>>=B;
    push(ACCA);
}


// x y +
// addition of X and Y
void i_ADD(void) {
    pull(ACCB);
    pull(ACCA);
    A+=B;
    push(ACCA);
}


// x y -
// subtraction of Y from X
void i_SUB(void) {
    pull(ACCB);
    pull(ACCA);
    A-=B;
    push(ACCA);
}


// x y *
// multiplication of X by Y
void i_MUL(void) {
    pull(ACCB);
    pull(ACCA);
    A*=B;
    push(ACCA);
}


// x y /
// division of X by Y
void i_DIV(void) {
    pull(ACCB);
    if(!B) longjmp(envbuf,ERR_CALC);    // reset
    pull(ACCA);
    A/=B;
    push(ACCA);
}


// x y //
// modulo operation of X by Y
void i_MOD(void) {
    pull(ACCB);
    if(!B) longjmp(envbuf,ERR_CALC);    // reset
    pull(ACCA);
    A%=B;
    push(ACCA);
}


// x y ==
// compare X and Y and return 1 if X==Y, or 0 otherwise
void i_EQ(void) {
    pull(ACCB);
    pull(ACCA);
    A=((A==B)? 1 : 0);
    push(ACCA);
}


// x y <>
// compare X and Y and return 1 if X<>Y, or 0 otherwise
void i_NEQ(void) {
    pull(ACCB);
    pull(ACCA);
    A=((A!=B)? 1 : 0);
    push(ACCA);
}


// x y >
// compare X and Y and return 1 if X>Y, or 0 otherwise
void i_GT(void) {
    pull(ACCB);
    pull(ACCA);
    A=((A>B)? 1 : 0);
    push(ACCA);
}


// x y >=
// compare X and Y and return 1 if X>=Y, or 0 otherwise
void i_GTEQ(void) {
    pull(ACCB);
    pull(ACCA);
    A=((A>=B)? 1 : 0);
    push(ACCA);
}


// x y <
// compare X and Y and return 1 if X<Y, or 0 otherwise
void i_SM(void) {
    pull(ACCB);
    pull(ACCA);
    A=((A<B)? 1 : 0);
    push(ACCA);
}


// x y <=
// compare X and Y and return 1 if X<=Y, or 0 otherwise
void i_SMEQ(void) {
    pull(ACCB);
    pull(ACCA);
    A=((A<=B)? 1 : 0);
    push(ACCA);
}


// ++
// increment of the top element by 1
void i_INC(void) {
    pull(ACCA);
    A++;
    push(ACCA);
}


// --
// decrement of the top element by 1
void i_DEC(void) {
    pull(ACCA);
    A--;
    push(ACCA);
}


// x if
// if X is not 0, executes until the corresponding ELSE or ENDIF
// if X is 0, skips until the corresponding ELSE or ENDIF
void i_IF(void) {
    if(EXECUTING) {
        pull(ACCA);
        cpu[core].car<<=CA_LENGTH;
        cpu[core].car|=(A? CA_EXEC: CA_NOEXEC);
    } else {
        cpu[core].car<<=CA_LENGTH;
        cpu[core].car|=CA_NOEXEC;
    }
    A=cpu[core].pcp|1;
    pushca();
}


// else
// inverts the current condition for execution in IF structure
// (can be used also to terminate REPEAT...UNTIL structure)
void i_ELSE(void) {
    if((cpu[core].car&CA_MASK)==CA_EXEC) {
        cpu[core].car&=~CA_MASK;
        cpu[core].car|=CA_NOEXEC;
    }
    else if((cpu[core].car&CA_MASK)==CA_NOEXEC) {
        cpu[core].car&=~CA_MASK;
        cpu[core].car|=CA_EXEC;
    }
    else longjmp(envbuf,ERR_ECST);  // reset
}


// endif
// terminates the current IF block
void i_ENDIF(void) {
    if((cpu[core].car&CA_MASK)==CA_NONE) longjmp(envbuf,ERR_ECST);  // reset
    cpu[core].car>>=CA_LENGTH;
    pullca();
}


// repeat
// marks the beginning of a REPEAT structure
void i_REPEAT(void) {
    if(EXECUTING) {
        cpu[core].car<<=CA_LENGTH;
        cpu[core].car|=CA_EXEC;
    } else {
        cpu[core].car<<=CA_LENGTH;
        cpu[core].car|=CA_NOEXEC;
    }
    A=cpu[core].pcp|1;
    pushca();   // store 'marked' address in the call stack
}


// x repif
// combines IF and REPEAT instructions
// if X is not 0, enters the REPEAT structure in a normal way
// if X is 0, ignores the entire following REPEAT structure
void i_REPIF(void) {
    if(EXECUTING) {
        pull(ACCA);
        if(A) {
            cpu[core].car<<=CA_LENGTH;
            cpu[core].car|=CA_EXEC;
        } else {
            cpu[core].car<<=CA_LENGTH;
            cpu[core].car|=CA_NOEXEC;
        }
    } else {
        cpu[core].car<<=CA_LENGTH;
        cpu[core].car|=CA_NOEXEC;
    }
    A=cpu[core].pcp|1;
    pushca();   // store 'marked' address in the call stack
}


// x until
// if X is zero, return to the corresponding REPEAT, otherwise continue after UNTIL
// (can be used also by already called by CALL/ACALL code to conditionally cancel its return address)
// (condition opposite to WHILE)
void i_UNTIL(void) {
    if((cpu[core].car&CA_MASK)==CA_NONE) longjmp(envbuf,ERR_ECST);  // reset
    pullca();
    if((cpu[core].car&CA_MASK)==CA_EXEC) {
        pull(ACCB);
        if(!B) {
            if(A&1) longjmp(envbuf,ERR_ALIGN);  // reset
            cpu[core].pcp=A;
            if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
            A|=1;   // mark the stored address
            pushca();
        } else {
            cpu[core].car>>=CA_LENGTH;
        }
    } else {
        cpu[core].car>>=CA_LENGTH;
    }
}


// x while
// if X is not zero, return to the corresponding REPEAT, otherwise continue after WHILE
// (can be used also by already called by CALL/ACALL code to conditionally cancel its return address)
// (condition opposite to UNTIL)
void i_WHILE(void) {
    if((cpu[core].car&CA_MASK)==CA_NONE) longjmp(envbuf,ERR_ECST);  // reset
    pullca();
    if((cpu[core].car&CA_MASK)==CA_EXEC) {
        pull(ACCB);
        if(B) {
            if(A&1) longjmp(envbuf,ERR_ALIGN);  // reset
            cpu[core].pcp=A;
            if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
            A|=1;   // mark the stored address
            pushca();
        } else {
            cpu[core].car>>=CA_LENGTH;
        }
    } else {
        cpu[core].car>>=CA_LENGTH;
    }
}


// x again
// return to the beginning of the current REPEAT structure if X is not 0
// (can be used also by already called by CALL/ACALL code to re-execute itself)
void i_AGAIN(void) {
    pull(ACCA);
    if(A) {
        pullca();
        if(A&1) longjmp(envbuf,ERR_ALIGN);  // reset
        cpu[core].pcp=A;
        if(cpu[core].pcp>=MEMSIZE) longjmp(envbuf,ERR_INVMEM);  // reset
        pushca();
    }
}


// x break
// break the current REPEAT structure if X is not 0
void i_BREAK(void) {
    if((cpu[core].car&CA_MASK)!=CA_EXEC && (cpu[core].car&CA_MASK)!=CA_NOEXEC) longjmp(envbuf,ERR_ECST);    // reset
    pull(ACCA);
    if(A) {
        cpu[core].car&=~CA_MASK;
        cpu[core].car|=CA_NOEXEC;
    }
}


// a rd8
// read a byte from address A and return it to the stack
void i_RD8(void) {
    pull(ACCB);
    A=MEM8RD(B);
    push(ACCA);
}


// a rd16
// read a 16-bit word from address A and return it to the stack
void i_RD16(void) {
    pull(ACCB);
    A=MEM16RD(B);
    push(ACCA);
}


// a rd32
// read a 32-bit word from address A and return it to the stack
void i_RD32(void) {
    pull(ACCB);
    A=MEM16RD(B+2);
    A=((A<<16) | MEM16RD(B));
    push(ACCA);
}


// a rdvln
// read a VLN value from address A and return it to the stack after returning the updated A
void i_RDVLN(void) {
    pull(ACCB);
    A=0;
    D=0;
    do {
        C=(UINT64)MEM16RD(B);
        B+=2;
        A=(((C&MASK14)<<D) | A);
        D+=14;
    } while((C&WT_MASK)==WT_DNL);
    push(ACCB);
    push(ACCA);
}


// x a wr8
// write byte X to address A
void i_WR8(void) {
    pull(ACCB);
    pull(ACCA);
    MEM8WR(B, A);
}


// x a wr16
// write 16-bit word X to address A
void i_WR16(void) {
    pull(ACCB);
    pull(ACCA);
    MEM16WR(B, A);
}


// x a wr32
// write 32-bit word X to address A
void i_WR32(void) {
    pull(ACCB);
    pull(ACCA);
    MEM16WR(B, A);
    MEM16WR((B+2), (A>>16));
}


// x a wrvln
// write VLN X to address A and return updated A
void i_WRVLN(void) {
    pull(ACCB);
    pull(ACCA);
    do {
        C=A&MASK14;
        A>>=14;
        if(A) C|=WT_DNL; else C|=WT_DL;
        MEM16WR(B, C);
        B+=2;
    } while(A);
    push(ACCB);
}


// rnd
// generate and return a 56-bit random number in stack
void i_RANDOM(void) {
    A=((A<<12) ^ rand());
    A=((A<<9) ^ rand());
    A=((A<<13) ^ rand());
    A=((A<<5) ^ rand());
    A=((A<<6) ^ rand());
    A=((A<<11) ^ rand());
    push(ACCA);
}


// a n c =
// copy memory block with size C from address A to address N
// allows overlapping
void i_MEMCOPY(void) {
    pull(ACCC); // get the counter in T
    pull(ACCB); // get the new address in B
    pull(ACCA); // get the old address in A
    if(A==B) return;
    if(B<A) {   // the new address is smaller than the old address - incremental copy
        while(C--) MEM8WR(B++, MEM8RD(A++));
    } else {    // the new address is greater than the old address - decremental copy
        A+=C;
        B+=C;
        while(C--) MEM8WR(--B, MEM8RD(--A));
    }
}


// a n $=
// copy zero-terminated block from address A to address N
// allows overlapping
void i_STRCOPY(void) {
    pull(ACCB); // get the new address in B
    pull(ACCA); // get the old address in A
    if(A==B) return;
    if(B<A) {   // the new address is smaller than the old address - incremental copy
        do {
            E=MEM8RD(A++);
            MEM8WR(B++, E);
        } while(E);
    } else {    // the new address is greater than the old address - decremental copy
        C=B;
        while(MEM8RD(C++)); // find the closing 0
        C-=B;   // calculate the length of the string
        A+=C;
        B+=C;
        while(C--) MEM8WR(--B, MEM8RD(--A));
    }
}


// x y c diff
// compare two memory blocks with length C, one from address X, other from address Y
// return 0 if they are not different, otherwise returns the number of remaining to test characters
void i_MEMDIFF(void) {
    pull(ACCC); // get the counter in T
    pull(ACCB); // get Y in B
    pull(ACCA); // get X in A
    while(C--) if(MEM8RD(A++)!=MEM8RD(B++)) break;
    push(ACCC);
}


// x y c diff$
// compare two zero-terminated strings, one from address X, other from address Y
// return 0 if they are not different, otherwise returns the number of tested characters
void i_STRDIFF(void) {
    pull(ACCB); // get Y in B
    pull(ACCA); // get X in A
    for(C=0; ; A++, B++, C++) {
        if(MEM8RD(A)!=MEM8RD(B)) break;
        if((MEM8RD(A) | MEM8RD(B))==0) {
            C=0;
            break;
        }
    }
    push(ACCC);
}


// x c v fill
// fill a memory block with start address X and length C with the value V
void i_MEMFILL(void) {
    pull(ACCB); // get the value
    pull(ACCC); // get the counter in T
    pull(ACCA); // get X in A
    while(C--) {
        MEM8WR(A++, (B&MASK8));
        if(B&OVER8) {
            MEM8WR(A++, (B>>8));
            if(C) C--;
            if((B&OVER16) && C) {
                MEM8WR(A++, (B>>16));
                if(C) C--;
                if((B&OVER24) && C) {
                    MEM8WR(A++, (B>>24));
                    if(C) C--;
                    if((B&OVER32) && C) {
                        MEM8WR(A++, (B>>32));
                        if(C) C--;
                        if((B&OVER40) && C) {
                            MEM8WR(A++, (B>>40));
                            if(C) C--;
                            if((B&OVER48) && C) {
                                MEM8WR(A++, (B>>48));
                                if(C) C--;
                            }
                        }
                    }
                }
            }
        }
    }
}


// x len$
// return the length of a zero-terminated string X (excluding the zero character)
void i_STRLEN(void) {
    pull(ACCA); // get X in A
    C=0;
    while(MEM8RD(A++)==0) break;
    push(ACCC);
}


// x y scan$
// search for the first occurrence of the zero-terminated string Y in X
// return the memory address, or 0 if not found
// NOTE: the result of this function makes it impossible to distinguish
//       between Y being found at address 0, and not being found at all
void i_STRSCAN(void) {
    pull(ACCB); // get Y in A
    pull(ACCA); // get X in A
    for(C=B; MEM8RD(A); A++) {
        if(MEM8RD(C)==0) {  // Y found, push the start address into stack
            A-=(C-B);
            push(ACCA);
            return;
        }
        if(MEM8RD(A)==MEM8RD(C)) C++; else C=B;
    }
    A=0;    // not found, also if X is void
    push(ACCA);
}


// x sysfn
// call system function X
void i_SYSFN(void) {
    pull(ACCA); // get X in A
    if(A<0x80) {
        cpu_instr(A);
    } else {

        #ifdef SYSFN_ENABLE
            sysfn(A);
        #endif

    }
}


// x delay
// delay the current thread for at least X nanoseconds
void i_DELAY(void) {
    pull(ACCA); // get X in A
    A/=DLY_RESOLUTION;
    A+=dlyc;
    if(!A) A++; // do not allow zero value for the compare register
    cpu[core].dcr=A;
}


// enter
// takes a snapshot of the data stack pointer
void i_ENTER(void) {
    if(cpu[core].ssp) longjmp(envbuf,ERR_DBLENT);   // reset: double entering error
    cpu[core].ssp=cpu[core].dsp;
    cpu[core].sdc=cpu[core].ddc;
}


// leave
// restores the data stack pointer from an earlier snapshot
void i_LEAVE(void) {
    if(!cpu[core].ssp) longjmp(envbuf,ERR_NOENT);   // reset: missing ENTER
    if(cpu[core].dsp<cpu[core].ssp) longjmp(envbuf,ERR_DSEMPTY);    // reset: stack exhaustion
    cpu[core].dsp=cpu[core].ssp;
    cpu[core].ssp=0;
    cpu[core].sdc=0;
}


// x !
// reads variable register x (where x is a valid number for register Vx) into the data stack
void i_GETVAR(void) {
    pull(ACCA); // get X in A
    B=cpu[core].v[A%VREGS];
    push(ACCB);
}


//  v x =!
// writes the value v into a variable register x (where x is a valid number for register Vx)
void i_SETVAR(void) {
    pull(ACCA); // get X in A
    pull(ACCB); // get V in B
    cpu[core].v[A%VREGS]=B;
}


// x setpr
// set priority x for the current thread
void i_SETPR(void) {
    pull(ACCA);
    if(A) cpu[core].ppr=A; else i_END();
}
