#include <GenericTypeDefs.h>
#include <string.h>
#include "torth.h"
#include "cpu7.h"
#include "system.h"

#ifdef FILE_OPERATIONS
#include <stdio.h>
#endif

// data types for addc()function
#define TYPE_INIT 0         // initialise for new compilation
#define TYPE_CPU7 1         // 7-bit CPU instruction code
#define TYPE_DATA 2         // normal numeric data constant
#define TYPE_WORD 3         // raw 16-bit words
#define TYPE_RAW8 4         // raw bytes (enclosed in quotes string)

#define LABEL_LENGTH 26     // maximum length of a label/word
#define FNAME_LENGTH 128    // maximum length of a file name for INCLUSE and LIBRARY
                            // the same memory buffer is used for reading files

UINT16 w;                   // temporary data word
INT8 last_ctype;            // last data type in addc()
INT64 lastc;                // virtual address of the last constant string


// add constant to the compiled code and update the output pointer
// input
// (*addr) current code compilation address
// (ctype) data type (see TYPE_xxxx definitions)
// (c) data constant to be added
// output
// (addr) update code compilation address
void addc(UINT32 *addr, INT8 ctype, UINT64 c) {    
    if(last_ctype>-1 && ctype!=last_ctype) {
        if(last_ctype==TYPE_CPU7) {
            w=((w>>7) | (MASK7<<7));
            MEM16WR(((*addr)++), (WT_CPU | w));
        }
        else if(last_ctype==TYPE_RAW8) {
            MEM16WR(((*addr)++), (w>>8));
        }
        w=MASK16;
        last_ctype=-1;
    }    
    switch(ctype) {
        case TYPE_WORD: {
            MEM16WR(((*addr)++), c);
            w=MASK16;
            last_ctype=-1;
            break;
        }
        case TYPE_DATA: {
            if((c&OVER14)==0) {         // use 14-bit constant
                MEM16WR(((*addr)++), (WT_DL | (c&MASK14)));
            }
            else if((c&OVER28)==0) {    // use 28-bit constant
                MEM16WR(((*addr)++), (WT_DNL | (c&MASK14)));
                MEM16WR(((*addr)++), (WT_DL | ((c>>14)&MASK14)));
            }
            else {                      // use 56-bit constant
                MEM16WR(((*addr)++), (WT_DNL | (c&MASK14)));
                MEM16WR(((*addr)++), (WT_DNL | ((c>>14)&MASK14)));
                MEM16WR(((*addr)++), (WT_DNL | ((c>>28)&MASK14)));
                MEM16WR(((*addr)++), (WT_DL | ((c>>42)&MASK14)));
            }
            w=MASK16;
            last_ctype=-1;
            break;
        }
        case TYPE_CPU7: {
            c=(c&MASK7)<<7;
            w=((w>>7) | c);
            if(last_ctype==TYPE_CPU7) {                
                MEM16WR(((*addr)++), (WT_CPU | w));
                w=MASK16;
                last_ctype=-1;
            }
            break;
        }
        case TYPE_RAW8: {
            c=(c&MASK8)<<8;
            w=((w>>8) | c);
            if(last_ctype==TYPE_RAW8) {                
                MEM16WR(((*addr)++), w);
                w=MASK16;
                last_ctype=-1;
            }
            break;
        }
        case TYPE_INIT:
        default: {
            w=MASK16;
            last_ctype=-1;
            break;
        }
    }
    last_ctype=ctype;
}


// add an atomic word to the compiled code
// input
// (*addr) current compilation address, uncontrolled
// (*name) the word as text, uncontrolled
// output
// (*addr) update compilation address
// addw() exit error code
UINT8 addw(UINT32 *addr, CHAR8 *name) {    
    UINT8 x=0;
    do {
        if(name[x]>='a' && name[x]<='z') name[x]-=0x20; // uppercase the word
    } while(name[++x]);

    #ifdef FILE_OPERATIONS

    FILE *f;
    UINT8 *data;
    CHAR8 fn[FNAME_LENGTH+1];
    INT64 tmpc, flen;
    if(!strcmp(name, "INCLUDE") || !strcmp(name, "LIBRARY")) {
        if(lastc<0) return(TORTH_CONSTREQ);
        if((*addr)&1) (*addr)++;        
        tmpc=lastc;
        w=MASK16;
        last_ctype=-1;
        lastc=-1;
        memset(fn,0,sizeof(fn));
        while((x=MEM8RD(tmpc++)) && (strlen(fn)<=FNAME_LENGTH)) fn[strlen(fn)]=x;
        f=fopen(fn,"rb");   // open the file for reading
        if(!f) return(TORTH_FILEERR);
        fseek(f,0,SEEK_END);
        flen=ftell(f);      // get the file length in (flen)
        rewind(f);
        if(flen<0 || !f) {  // seek error
            fclose(f);
            return(TORTH_FILEERR);
        }
        if((flen+(*addr)+64)>=MEMSIZE) {    // too big file (leaving a safety buffer of 64 bytes)
            fclose(f);
            return(TORTH_NOMEM);
        }
        if(!strcmp(name, "INCLUDE")) {      // load and compile an external source file
            tmpc=MEMSIZE-flen;  // calculate the virtual file buffer address
            tmpc+=CPU7_MEMORY;  // convert into host physical address
            data=(UINT8 *)((unsigned int)tmpc);
            tmpc=fread(data,1,flen,f);      // read the file into buffer
            fclose(f);
            x=torth_compile((CHAR8 **)&data,addr);
        }
        else if(!strcmp(name, "LIBRARY")) {     // directly insert an external binary file            
            for(;;) {   // read the file and insert it into the program memory
                tmpc=fread(fn,1,FNAME_LENGTH,f);
                if(tmpc<1) break;
                data=(UINT8 *)fn;
                while(tmpc--) MEM8WR(((*addr)++), *(data++));
            }
            fclose(f);
            x=TORTH_NOERR;
        }        
        if((*addr)&1) (*addr)++;
        return(x);
    }
    else
        
    #endif

    {   // find the word CPU code
        x=(UINT8)-1;
        while(++x<0x80) {
            if(!strcmp(name, instw[x])) {
                addc(addr, TYPE_CPU7, x);
                return(TORTH_NOERR);
            }
        }
    }    
    return(TORTH_INVATOM);
}


// find label in the compiled code and return its address
// input
// (*label) ASCIIZ label
// (org) start address of the lookup area (must be even number)
// (addr) current compilation address - end address of the lookup area
// output
// findl() label address or MASK32 if the label has not been found
UINT32 findl(CHAR8 *label, UINT32 org, UINT32 addr) {    
    if(!label || (*label)==0 || org>=MEMSIZE || addr>=MEMSIZE) return(MASK32);
    CHAR8 *l;
    UINT16 w;
    UINT32 al, a=org;
    addr&=~1;
    while(a<addr) {
        w=MEM16RD(a);
        if((w&WT_MASK)==WT_IGN) {    // labels are marked in the compiled code by using the WT_IGN type
            l=label;
            while((w&WT_MASK)==WT_IGN && a<addr) {
                if((*l)==0) {       // found it
                    al=w&MASK14;    // although not checked, the next two words must be WT_IGN type
                    w=MEM16RD(a+2);
                    al=((w&MASK14)<<14 | al);   // read the next two words and form a 28-bit address
                    return(al);                 // to return
                }
                w=MEM16RD(a);
                a+=2;                
                if((w&MASK8)==(*l)) l++; else break;
            }
        }
    }
    return(MASK32);
}


// get an unsigned integer numeric constant from the source
// input
// (**source) pointer to the source code in text form, uncontrolled
// (*nsys) pointer to the current numeric base, uncontrolled
// output
// (*source) new value of the source
// (nsys) updated nueric base
// getn() the read number
//        -1 in case of no number
//        -2 in case of an error
INT64 getn(CHAR8 **source, UINT8 *nsys) {
    if(!strchr("0123456789ABCDEFabcdef$%", **source)) return(-1);   // valid characters to start
    INT64 n=-2;
    while(**source && *source) {
        if((**source)=='%') {       // binary format number
            if(*nsys!=10 || n>-1) break;
            *nsys=2;
        }
        else if((**source)=='$') {  // hexadecimal format number
            if(*nsys!=10 || n>-1) break;
            *nsys=16;
        }
        else if((**source)>='0' && (**source)<='9') {
            if(*nsys==2 && (**source)>'1') break;
            if(n<0) n=0;
            n=(n*(*nsys))+((**source)-'0');
        }
        else if(((**source)>='A' && (**source)<='F') || ((**source)>='a' && (**source)<='f')) {
            if(*nsys!=16) break;
            UINT8 c=(**source);
            if(c>='a') c-=0x20;
            if(n<0) n=0;
            n=(n*(*nsys))+((**source)-'A');
        }
        else break;
    }
    if(n!=-1) lastc=-1;
    return(n);
}


// compile Torth source code into p-code
// input
// (**source) pointer to the source code in text form
// (*addr) originating virtual address for the compiled code (must be even number)
// output
// (*source) new value of the source
// (addr) current virtual address
// torth_compile() error code
UINT8 torth_compile(CHAR8 **source, UINT32 *addr) {
    UINT32 org;     // holds the start address of the compiled code in the last compilation
    INT64 laddr, n=-1;
    UINT8 lx, ntf, nsys=10;
    CHAR8 label[LABEL_LENGTH+1];
    if(addr) org=(*addr); else org=0;
    w=MASK16;
    last_ctype=-1;
    lastc=-1;
    addc(addr, TYPE_INIT, 0);
    while(**source && *source && addr) {
        if((*addr)>=MEMSIZE) return(TORTH_NOMEM);

        // numeric constants
        n=getn(source, &nsys);
        if(n==-2) return(TORTH_INVNUM);

        // skip the whitespaces
        while((**source)<=' ') {
            if(n>-1) {
                addc(addr, TYPE_DATA, (UINT64)n);
                n=-1;
                nsys=10;
            } else
                if(nsys!=10) return(TORTH_INVNUM);
            (*source)++;
            continue;
        }

        // skip the comments
        while((**source)=='`') {
            lastc=-1;
            if(nsys!=10 || n>-1) return(TORTH_INVNUM);
            UINT8 linef=((*(++(*source))=='!')? 1 : 0);
            while(((**source)!='\n' && linef) || ((**source)!='`' && !linef)) (*source)++;
            while((**source)<=' ' || (**source)=='`') (*source)++;
            continue;
        }

        // string constants (copied directly into the program memory)
        if((**source)=='"') {
            if(nsys!=10 || n>-1) return(TORTH_INVNUM);
            lastc=(INT64)(*addr);       // store the current compilation address
            while(*(++(*source))!='"') {
                if((**source)<' ') return(TORTH_NOCQ);
                if((**source)=='_') {   // 'special' characters
                    switch(*(++(*source))) {
                        case '"': {     // _" will put a "
                            addc(addr, TYPE_RAW8, '"');
                            break;
                        }
                        case '_': {     // __ will put a _
                            addc(addr, TYPE_RAW8, '_');
                            break;
                        }
                        case '\\': {    // _\ will put a \n
                            addc(addr, TYPE_RAW8, '\n');
                            break;
                        }
                        case '#': {     // _# will put a \r
                            addc(addr, TYPE_RAW8, '\r');
                            break;
                        }
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                        {   // _hh will insert a character with hexadecimal ASCII hh
                            UINT8 c, h=0;
                            c=*((*source)++);
                            if(c>='a') c-=0x20;
                            if(c>='A') h=((c-'A')<<4); else h=((c-'0')<<4);
                            c=(**source);
                            if(c>='a') c-=0x20;
                            if(c>='A') h|=(c-'A'); else h|=(c-'0');
                            addc(addr, TYPE_RAW8, h);
                            break;
                        }
                        default: {
                            return(TORTH_INVSCHR);
                        }
                    }
                } else
                    addc(addr, TYPE_RAW8, **source);
            }
            addc(addr, TYPE_RAW8, 0);       // append a closing 0 and create ASCIIZ string
            addc(addr, TYPE_DATA, lastc);   // push the constant address into the stack
            continue;
        }

        // labels
        if((**source)==':' || (**source)=='.' || (**source)=='_' || (**source)=='&') {
            lastc=-1;
            if(nsys!=10 || n>-1) return(TORTH_INVNUM);
            memset(label,0,(LABEL_LENGTH+1));
            lx=0;
            label[lx++]=(*((*source)++));   // first get the 'function' character
            if(label[0]!=':')
                ntf=((**source)=='!');      // 'new thread' reference flag
            else
                while((**source)==' ') (*source)++;
            while(lx<LABEL_LENGTH && 
                    strchr("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_", label[lx]))
                label[lx++]=(*((*source)++));
            if(lx<2 || (**source)>' ') return(TORTH_INVLBL);
            if(label[0]==':') {         // new label
                if(findl(&label[1],org,*addr)!=MASK32) return(TORTH_REPEATLBL);
                lx=1;
                while(label[lx]) addc(addr, TYPE_WORD, (WT_IGN | label[lx++]));
                addc(addr, TYPE_WORD, WT_IGN);  // store the closing 0 in ASCIIZ
                while((**source)==' ') (*source)++;
                if((**source)=='@') {
                    while((**source)==' ') (*source)++;
                    laddr=getn(source, &nsys);
                    if(laddr<0) return(TORTH_ADDRREQ);
                } else
                    laddr=(*addr)+4;
                addc(addr, TYPE_WORD, (WT_IGN | (laddr&MASK14)));
                addc(addr, TYPE_WORD, (WT_IGN | ((laddr>>14)&MASK14)));
            }
            else if(label[0]=='.') {    // label reference
                laddr=findl(&label[1],org,*addr);
                if(laddr==MASK32) return(TORTH_UNKNLBL);
                addc(addr, TYPE_DATA, laddr);
            }
            else if(label[0]=='_') {    // label call
                laddr=findl(&label[1],org,*addr);
                if(laddr==MASK32) return(TORTH_UNKNLBL);
                addc(addr, TYPE_DATA, laddr);
                if(!ntf) {
                    if(addw(addr, "CALL")!=TORTH_NOERR) return(TORTH_INTERR);
                } else {
                    if(addw(addr, "NTCALL")!=TORTH_NOERR) return(TORTH_INTERR);
                }
            }
            else if(label[0]=='&') {    // label call (absolute)
                laddr=findl(&label[1],org,*addr);
                if(laddr==MASK32) return(TORTH_UNKNLBL);
                addc(addr, TYPE_DATA, laddr);
                if(!ntf) {
                    if(addw(addr, "ACALL")!=TORTH_NOERR) return(TORTH_INTERR);
                } else {
                    if(addw(addr, "NTACALL")!=TORTH_NOERR) return(TORTH_INTERR);
                }
            }
            else return(TORTH_INVLBL);  // normally it should never get here
        }

        // atomic words
        if(nsys!=10 || n>-1) return(TORTH_INVNUM);
        memset(label,0,(LABEL_LENGTH+1));
        lx=0;
        while(lx<LABEL_LENGTH && (**source)>' ') label[lx++]=(*((*source)++));
        if(lx<1 || (**source)>' ') return(TORTH_INVATOM);
        if(!strcmp(label, ";")) strcpy(label, instw[CODE_RETURN]);  // the RETURN instruction can be also ;
        if(lx==TORTH_NOERR && (strcmp(label, "REPEAT")==0 || strcmp(label, "REPIF")==0) &&
            ((*addr)&1)==0) lx=addw(addr, "NOP");    // insert a padding NOP for REPEAT/REFIF at even address
        lx=addw(addr, label);        
        if(lx!=TORTH_NOERR) return(lx);
    }
    if(n>-1) addc(addr, TYPE_DATA, (UINT64)n);
    return(TORTH_NOERR);
}
