/*-
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if !defined(lint) && defined(sccs)
static char sccsid[] = "@(#)put.c	8.2 (Berkeley) 5/24/94";
#endif /* not lint */

#include "whoami.h"
#include "0.h"
#include "opcode.h"
#ifdef PC
#   include "pc.h"
#   include "align.h"
#else
    short               *obufp = obuf;
#endif
#include <ctype.h>
#ifdef DEBUG
#include "OPnames.h"    /*table of the printing opcode names*/
#endif

char                    showit[] = "'x'";

#define WIDTH           80
#define COLUMN          39
static  char            lstTmp[BUFSIZ];  
static  char            lstBuf[WIDTH+16];
static  char            lstHex[WIDTH+16];
static  int             lstPos1 = -1;
static  int             lstPos2 = -1;

static  void            Printf(const char *fmt, ...);
static  void            Println();

#ifdef DEBUG
static  void            Prints(const char *str);
static  void            Printw(int o);
static  void            Resetln();

#define RESETLN()       Resetln()
#define PRINTF(x)       Printf x
#define PRINTS(s)       Prints(s)
#define PRINTW(o)       Printw(o)
#define PRINTLN()       Println()
#else
#define RESETLN()
#define PRINTF(x)
#define PRINTS(s)
#define PRINTW(x)
#define PRINTLN()
#endif


#ifdef OBJ
/*
 * Put is responsible for the interpreter equivalent of code generation.
 * Since the interpreter is specifically designed for Pascal, little work
 * is required here.
 */
int
put(int n, int opt, ...)
{
        register int    i, ip;
        register const char *cp;
        register long   lp;
        register double dp;
        int      subop, suboppr, op, oldlc;
        const char *string;
        static int casewrd;
        va_list  ap;

        /*
         * It would be nice to do some more optimizations here.  The work
         * done to collapse offsets in lval should be done here, the IFEQ
         * etc relational operators could be used etc.
         */
        oldlc = (int)lc;        /* its either this or change return a char * */
        if ( !CGENNING )
                /*
                 * code disabled - do nothing
                 */
                return (oldlc);
        suboppr = subop = (opt >> 8) & 0377;
        op = opt & 0377;
        string = 0;
        va_start(ap, opt);

#ifdef DEBUG
        if ((cp = otext[op]) == NIL) {
                printf("op= %o\n", op);
                panic("put - bad op");
        }
        Resetln();
#endif

        switch (op) {
                case O_ABORT:
                        ip = va_arg(ap, int);
                        cp = "*";
                        break;

                case O_AS:
                        ip = va_arg(ap, int);
                        switch(ip) {
                        case 0:
                                break;
                        case 2:
                                op = O_AS2;
                                n = 1;
                                break;
                        case 4:
                                op = O_AS4;
                                n = 1;
                                break;
                        case 8:
                                op = O_AS8;
                                n = 1;
                                break;
                        default:
                                goto pack;
                        }
#ifdef DEBUG
                        cp = otext[op];
#endif
                        break;

                case O_FOR1U:
                case O_FOR2U:
                case O_FOR4U:
                case O_FOR1D:
                case O_FOR2D:
                case O_FOR4D:
                /* relative addressing */
                        ip = va_arg(ap, int);
                        ip -= (unsigned) lc + sizeof(short);

                /* try to pack the jump */
                        if (ip <= 127 && ip >= -128) {
                                suboppr = subop = ip;
                                ip = va_arg(ap, int);
                                n--;
                        } else {
                                /* have to allow for extra displacement */
                                ip -= sizeof(short);
                        }
                        break;

                case O_CONG:
                case O_LVCON:
                case O_CON:
                case O_LINO:
                case O_NEW:
                case O_DISPOSE:
                case O_DFDISP:
                case O_IND:
                case O_OFF:
                case O_INX2:
                case O_INX4:
                case O_CARD:
                case O_ADDT:
                case O_SUBT:
                case O_MULT:
                case O_IN:
                case O_CASE1OP:
                case O_CASE2OP:
                case O_CASE4OP:
                case O_FRTN:
                case O_WRITES:
                case O_WRITEC:
                case O_WRITEF:
                case O_MAX:
                case O_MIN:
                case O_ARGV:
                case O_CTTOT:
                case O_INCT:
                case O_RANG2:
                case O_RSNG2:
                case O_RANG42:
                case O_RSNG42:
                case O_SUCC2:
                case O_SUCC24:
                case O_PRED2:
                case O_PRED24:
                        ip = va_arg(ap, int);
                        if (ip == 0)
                                break;
                        goto pack;

                case O_CON2:
                case O_CON24:
                        ip = va_arg(ap, int);
                pack:
                /* try to pack the jump */
                        if (ip <= 127 && ip >= -128) {
                                suboppr = subop = ip;
                                ip = va_arg(ap, int);
                                n--;
                                if (op == O_CON2) {
                                        op = O_CON1;
#ifdef DEBUG
                                        cp = otext[O_CON1];
#endif
                                }
                                if (op == O_CON24) {
                                        op = O_CON14;
#ifdef DEBUG
                                        cp = otext[O_CON14];
#endif
                                }
                        }
                        break;

                case O_CON8:
                        dp = va_arg(ap, double);
                        PRINTF(("%5d\tCON8\t%22.14e", lc-HEADER_BYTES, dp));
#ifdef DEC11
                        word(op);
#else
                        word(op << 8);
#endif
                        qword(dp);
                        PRINTLN();
                        return (oldlc);

                default:
                        ip = va_arg(ap, int);
                        if (op >= O_REL2 && op <= O_REL84) {
                                if ((i = (subop >> INDX) * 5 ) >= 30)
                                        i -= 30;
                                else
                                	i += 2;
#ifdef DEBUG
                                string  = &"IFEQ\0IFNE\0IFLT\0IFGT\0IFLE\0IFGE"[i];
#endif
                                suboppr = 0;
                        }
                        break;

                case O_IF:
                case O_TRA:
                /* relative addressing */
                        ip = va_arg(ap, int);
                        ip -= ( unsigned ) lc + sizeof(short);
                        break;

                case O_CONC:
                        ip = va_arg(ap, int);
#ifdef DEBUG
                        showit[1] = ip, string = showit;
#endif
                        suboppr = 0;
                        op = O_CON1;
#ifdef DEBUG
                        cp = otext[O_CON1];
#endif
                        subop = ip;
                        goto around;

                case O_CONC4:
                        ip = va_arg(ap, int);
#ifdef DEBUG
                        showit[1] = ip, string = showit;
#endif
                        suboppr = 0;
                        op = O_CON14;
                        subop = ip;
                        goto around;

                case O_CON1:
                case O_CON14:
                        ip = va_arg(ap, int);
                        suboppr = subop = ip;
around:
                        n--;
                        break;

                case O_CASEBEG:
                        casewrd = 0;
                        return (oldlc);

                case O_CASEEND:
                        if ((unsigned) lc & 1) {
                                lc--;
                                word(casewrd);
                        }
                        return (oldlc);

                case O_CASE1:
                        ip = va_arg(ap, int);
                        PRINTF(("%5d\tCASE1\t%d", lc-HEADER_BYTES, ip));
                        /*
                         * this to build a byte size case table saving bytes across
                         * calls in casewrd so they can be put out by word()
                         */
                        lc++;
                        if ((unsigned) lc & 1)
#ifdef DEC11
                                casewrd = ip & 0377;
#else
                                casewrd = (ip & 0377) << 8;
#endif
                        else {
                                lc -= 2;
#ifdef DEC11
                                word(((ip & 0377) << 8) | casewrd);
#else
                                word((ip & 0377) | casewrd);
#endif
                        }
                        PRINTLN();
                        return (oldlc);

                case O_CASE2:
                        ip = va_arg(ap, int);
                        PRINTF(("%5d\tCASE2\t%d", lc-HEADER_BYTES, ip));
                        word(ip);
                        PRINTLN();
                        return (oldlc);

                case O_PUSH:
                case O_RANG4:
                case O_RANG24:
                case O_RSNG4:
                case O_RSNG24:
                case O_SUCC4:
                case O_PRED4:
                        lp = va_arg(ap, long);
                        if (op == O_PUSH && lp == 0) {
                                return (oldlc);
                        }
                        if (lp < 128 && lp >= -128 && lp != 0) {
                                suboppr = subop = lp;
                                lp = va_arg(ap, long);
                                n--;
                        }
                        goto longgen;

                case O_TRA4:
                case O_CALL:
                case O_FSAV:
                case O_GOTO:
                case O_NAM:
                case O_READE:
                        /* absolute long addressing */
                        lp = va_arg(ap, long);
                        lp -= HEADER_BYTES;
                        goto longgen;

                case O_RV1:
                case O_RV14:
                case O_RV2:
                case O_RV24:
                case O_RV4:
                case O_RV8:
                case O_RV:
                case O_LV:
                        /*
                         * positive offsets represent arguments
                         * and must use "ap" display entry rather
                         * than the "fp" entry
                         */
                        ip = va_arg(ap, int);
                        if (ip >= 0) {
                                subop++;
                                suboppr++;
                        }
#ifdef PDP11
                        break;
#else
                        /*
                         * offsets out of range of word addressing must
                         * use long offset opcodes
                         */
                        if (ip < SHORTADDR && ip >= -SHORTADDR)
                                break;
                        else {
                                op += O_LRV - O_RV;
#ifdef DEBUG
                                cp = otext[op];
#endif
                                if (op == O_LRV)
                                        n--;    /* handled below */
                        }
                        lp = ip;
                        goto longgen;
#endif /*!PDP11*/
                case O_BEG:
                case O_NODUMP:
                case O_CON4:
                case O_CASE4:
                        lp = va_arg(ap, long);
                longgen:
#ifdef DEBUG
                        Printf("%5d\t%s", lc - HEADER_BYTES, cp+1);
                        if (suboppr)
                                 Printf(":%d", suboppr);
                        if (string)
                                 Printf("\t%s", string);
                        if (n>1)
                                 Printf("\t");
#endif
                        if (op != O_CASE4)
#ifdef DEC11
                                word((op & 0377) | subop << 8);
#else
                                word(op << 8 | (subop & 0377));
#endif
                        if (n>1) {
                                PRINTF(("%ld ", lp));
                                dword(lp);
                                for (i=2; i<n; i++) {
                                        lp = va_arg(ap, long);
                                        PRINTF(("%ld ", lp));
                                        dword(lp);
                                }
                        }
                        if (op == O_LRV) {
                                ip = va_arg(ap, int);
                                PRINTF(("%d ", ip));
                                word(ip);
                        }
                        PRINTLN();
                        return (oldlc);
        }

#ifdef DEBUG
        Printf("%5d\t%s", lc - HEADER_BYTES, cp+1);
        if (suboppr)
                Printf(":%d", suboppr);
        if (string)
                Printf("\t%s", string);
        if (n>1)
                Printf("\t");
#endif

        if (op != TNONE)
#ifdef DEC11
                word((op & 0377) | subop << 8);
#else
                word(op << 8 | (subop & 0377));
#endif
        if (n>1) {
                PRINTF(("%d ", ip));
                word(ip);
                for (i=2; i<n; i++) {
                        ip = va_arg(ap, int);
                        PRINTF(("%d ", ip));
                        word(ip);
                }
        }
        PRINTLN();
        return (oldlc);
}
#endif  /*OBJ*/


static void
Printf(const char *fmt, ...)
{
        if (opt('k') && lstPos1 >= 0) {
                va_list ap;
                int tab, i;

                va_start(ap, fmt);
                (void) vsprintf(lstTmp, fmt, ap);
                for (i=0; lstTmp[i]; i++)
                        if (lstTmp[i] != '\t')
                                lstBuf[ lstPos1++ ] = lstTmp[i];
                        else {
                                tab = 8 - (lstPos1 % 7);
                                while (tab-- > 0)
                                        lstBuf[ lstPos1++ ] = ' ';
                        }
                lstBuf[ lstPos1 ] = '\0';
        }
}


static void
Println(void)
{
        if (opt('k') && (lstPos1 > 0 || lstPos2 > 0))
                printf("%-*s%s\n", COLUMN, lstBuf, lstHex);
        lstPos1 = lstPos2 = -1;
}


#ifdef DEBUG
static void
Prints(const char *str)
{
        if (opt('k') && lstPos1 >= 0) {
                if (strlen(str) + lstPos1 + 2 < COLUMN) {
                        lstPos1 += sprintf(lstBuf+lstPos1, "\"%s\"", str);
                } else {
                        printf("%s\"%s\"\n", lstBuf, str);
                        lstBuf[ lstPos1 = 0 ] = '\0';
                }
        }
}

static void
Printw(int o)
{
        if (opt('k') && lstPos2 >= 0) {
                if (lstPos2 > (WIDTH-5) - COLUMN) {
                        Println();
                        Resetln();
                }                        
                lstPos2 += sprintf(lstHex+lstPos2, " %04x", o&0xffff);
        }
}

static void
Resetln(void)
{
        lstBuf[ lstPos1 = 0 ] = 0;
        lstHex[ lstPos2 = 0 ] = 0;
}
#endif  /*DEBUG*/



/*
 * listnames outputs a list of enumerated type names which can then be
 * selected from to output a TSCAL a pointer to the address in the code
 * of the namelist is kept in value[ NL_ELABEL ].
 */
int
listnames(ap)
        register struct nl *ap;
{
        struct nl *next;
#ifdef OBJ
        register int oldlc;
#endif
        register int len;
        register unsigned w;
        register char *strptr;

        if ( !CGENNING )
                /* code is off - do nothing */
                return(0);
        if (ap->class != TYPE)
                ap = ap->type;
        if (ap->value[ NL_ELABEL ] != 0) {
                /* the list already exists */
                return( ap -> value[ NL_ELABEL ] );
        }
#ifdef OBJ
        oldlc = (int) lc;                       /* same problem as put */
        (void) put(2, O_TRA, lc);
        ap->value[ NL_ELABEL ] = (int) lc;
#endif
#ifdef PC
        putprintf(" .data", 0);
        aligndot(A_STRUCT);
        ap -> value[ NL_ELABEL ] = (int) getlab();
        (void) putlab((char *) ap -> value[ NL_ELABEL ] );
#endif
        /* number of scalars */
        next = ap->type;
        len = next->range[1]-next->range[0]+1;
#ifdef OBJ
        (void) put(2, O_CASE2, len);
#endif
#ifdef PC
        putprintf( "        .word %d" , 0 , len );
#endif
        /* offsets of each scalar name */
        len = (len+1)*sizeof(short);
#ifdef OBJ
        (void) put(2, O_CASE2, len);
#endif
#ifdef PC
        putprintf( "        .word %d" , 0 , len );
#endif
        next = ap->chain;
        do {
                for(strptr = next->symbol;  *strptr++;  len++)
                        continue;
                len++;
#ifdef OBJ
                (void) put(2, O_CASE2, len);
#endif
#ifdef PC
                putprintf( "        .word %d" , 0 , len );
#endif
        } while (next = next->chain);
        /* list of scalar names */
        strptr = getnext(ap, &next);

#ifdef OBJ
        do  {
#ifdef DEC11
                w = (unsigned) *strptr;
#else
                w = *strptr << 8;
#endif
                if (!*strptr++)
                        strptr = getnext(next, &next);
#ifdef DEC11
                w |= *strptr << 8;
#else
                w |= (unsigned) *strptr;
#endif
                if (!*strptr++)
                        strptr = getnext(next, &next);
                word((int) w);
        } while (next);
        /* jump over the mess */
        patch((PTR_DCL) oldlc);
#endif
#ifdef PC
        while (next) {
                while (*strptr) {
                        putprintf("        .byte   0%o" , 1 , *strptr++);
                        for (w = 2; (w <= 8) && *strptr; w ++) {
                                putprintf(",0%o" , 1 , *strptr++);
                        }
                        putprintf("" , 0);
                }
                putprintf("    .byte   0" , 0);
                strptr = getnext(next, &next);
        }
        putprintf("        .text", 0);
#endif
        return(ap -> value[ NL_ELABEL ]);
}


char *
getnext(next, new)
        struct nl *next, **new;
{
        if (next != NIL) {
                next = next->chain;
                *new = next;
        }
        if (next == NLNIL)
                return("");
#ifdef OBJ
        if (CGENNING) {
                Printf("%5d\t\t\"%s\"\n", lc-HEADER_BYTES, next->symbol);
                Println();
        }
#endif
        return(next->symbol);
}



#ifdef OBJ
/*
 * Putspace puts out a table of nothing to leave space
 * for the case branch table e.g.
 */
void
putspace(n)
        int n;
{
        register int i;

        if ( !CGENNING )
                /*
                 * code disabled - do nothing
                 */
                return;
        PRINTF(("%5d\t.=.+%d\n", lc - HEADER_BYTES, n));
        for (i = n; i > 0; i -= 2)
                word(0);
        PRINTLN();
}


void
putstr(sptr, padding)
        char *sptr;
        int padding;
{
        register unsigned short w;
        register char *strptr = sptr;
        register int pad = padding;

        if ( !CGENNING )
                /*
                 * code disabled - do nothing
                 */
                return;

        RESETLN();
        PRINTF(("%5d\t\t", lc-HEADER_BYTES));
        PRINTS(strptr);

        if (pad == 0) {
                do {
#ifdef DEC11
                        w = (unsigned short) * strptr;
#else
                        w = (unsigned short) * strptr<<8;
#endif
                        if (w)
#ifdef DEC11
                            w |= *++strptr << 8;
#else
                            w |= *++strptr;
#endif
                        word((int) w);
                } while (*strptr++);
        } else {
#ifdef DEC11
                    do  {
                            w = (unsigned short) * strptr;
                            if (w) {
                                    if (*++strptr)
                                            w |= *strptr << 8;
                                    else {
                                            w |= ' ' << 8;
                                            pad--;
                                    }
                                    word((int) w);
                            }
                    } while (*strptr++);
#else
                    do  {
                            w = (unsigned short)*strptr<<8;
                            if (w) {
                                    if (*++strptr)
                                            w |= *strptr;
                                    else {
                                            w |= ' ';
                                            pad--;
                                    }
                                    word(w);
                            }
                    } while (*strptr++);
#endif
                while (pad > 1) {
#ifdef DEC11
                        word(' ' | (' ' << 8));
#else
                        word((' ' << 8) | ' ');
#endif
                        pad -= 2;
                }
                if (pad == 1)
#ifdef DEC11
                        word(' ');
#else
                        word(' ' << 8);
#endif
                else
                        word(0);
        }
        PRINTLN();
}
#endif


#ifndef PC
int lenstr(sptr, padding)
        char *sptr;
        int padding;
{
        register int cnt;
        register char *strptr = sptr;

        cnt = padding;
        do {
                cnt++;
        } while (*strptr++);
        return((++cnt) & ~1);
}
#endif



/*
 * Patch repairs the branch at location loc to come to the current
 * location.  for PC, this puts down the label and the branch just
 * references that label.  lets here it for two pass assemblers.
 */
void
patch(loc)
        PTR_DCL loc;
{
#ifdef OBJ
        patchfil(loc, (long)(lc-loc-2), 1);
#endif
#ifdef PC
        (void) putlab((char *) loc );
#endif
}


#ifdef OBJ
void
patch4(loc)
        PTR_DCL loc;
{
        patchfil(loc, (long)(lc - HEADER_BYTES), 2);
}


/*
 * Patchfil makes loc+2 have jmploc as its contents.
 */
void
patchfil(loc, jmploc, words)
        PTR_DCL loc;
        long jmploc;
        int words;
{
        register int i;
        short val;

        if (!CGENNING)
                return;
        if (loc > (unsigned) lc)
                panic("patchfil");
#ifdef DEBUG
        if (opt('k'))
                printf("\tpatch %u 0x%lx/%ld\n", loc - HEADER_BYTES, jmploc, jmploc);
#endif
        val = (short)jmploc;
        do {
#ifndef DEC11
                if (words > 1)
                        val = (short)(jmploc >> 16);
	    	else
                        val = (short)(jmploc);
#endif
                i = ((unsigned) loc + 2 - ((unsigned) lc & ~01777))/2;
                if (i >= 0 && i < 1024) {
                        obuf[i] = val;
                } else {
                        if(lseek(ofil, (long) loc+2, 0) == -1)
                                perror("patchfil: 1seek1"), panic("patchfil");
#ifdef DEBUG
                        if (opt('x'))
                                dumphex(tell(ofil), &val, 2);
#endif
                        if (write(ofil, (char *) (&val), 2) != 2)
                                perror("patchfil: write"), panic("patchfil");
                        if (lseek(ofil, (long)0, 2) == -1)
                                perror("patchfil: 1seek2"), panic("patchfil");
                }
                loc += 2;
#ifdef DEC11
                val = (short)(jmploc >> 16);
#endif
        } while (--words);
}



/*
 *      Put the word o into the code
 */
void
word(o)
        int o;
{
        PRINTW(o);
        *obufp = o;
        obufp++;
        lc += 2;
        if (obufp >= obuf+512)
                pflush();
}


/*
 * Put the dword 'o' into the code
 */
void
dword(o)
        long o;
{
#ifdef DEC11
        word((int)(o & 0xffff));
        word((int)(o >> 16));
#else
        word((int)(o >> 16));
        word((int)(o & 0xffff));
#endif
}


/*
 * Put the qword 'o' into the code
 */
void
qword(o)
        double o;
{
#define DBLSHORTS       (sizeof(double)/sizeof(short))
        short s[ DBLSHORTS ];
        int i;

        *((double *)s) = o;
        for (i=0; i<DBLSHORTS; i++)
                word(s[i]);
}


void
dumphex(pos, pBuffer, bcnt)
        long pos;
        const void *pBuffer;
        long bcnt;
{
        #define HEXWIDTHLINE    16
        #define HEXWIDTHCOLS    4
        #define HEXBUFFER       (14+(HEXWIDTHLINE*4))
        char    ascii[ HEXWIDTHLINE+1 ];
        char    hex[ HEXBUFFER+1 ];
        unsigned char *pTmp;
        long    lpos, col;
        int     idx;

/* Init */
        if (!bcnt)
                return;
        lpos = 0, col = 0;
        ascii[ HEXWIDTHLINE ] = '\0';

/* Full lines */
        pTmp = (unsigned char *)pBuffer;
        while (bcnt--)
        {
                if (lpos == 0)
                {
                        idx = sprintf(hex, "0x%04X ", pos);
                        col = 0;
                }

                idx += sprintf(hex+idx, "%02x%s", (int)*pTmp,
                ++col%HEXWIDTHCOLS || lpos == (HEXWIDTHLINE-1) ? " " : " | ");
                ascii[ lpos ] = (isprint(*pTmp) ? *pTmp : '.');

                if (++lpos >= HEXWIDTHLINE)
                {
                        printf("%s [%s]\n", hex, ascii);
                        lpos = 0;
                }
                pTmp++, pos++;
        }

/* Incomplete line */
        if (lpos) {
                do {
                        idx += sprintf(hex+idx, "  %s", ++col%HEXWIDTHCOLS ||
                                lpos == (HEXWIDTHLINE-1) ? " " : "   ");
                        ascii[ lpos ] = ' ';
                } while (++lpos < HEXWIDTHLINE);
                printf("%s [%s]\n", hex, ascii);
        }
}


/*
 * Flush the code buffer
 */
void
pflush()
{
        extern char *obj;
        register int i;

        i = (obufp - ((short *) obuf)) * sizeof(short);
#ifdef DEBUG
        if (opt('x'))
                dumphex(tell(ofil), obuf, i);
#endif
        if (i != 0 && write(ofil, (char *)obuf, i) != i)
                perror(obj), pexit(DIED);
        obufp = obuf;
}
#endif


/*
 * Getlab - returns the location counter.
 * included here for the eventual code generator.
 *      for PC, thank you!
 */
char *
getlab()
{
#ifdef OBJ
        return (lc);
#endif
#ifdef PC
        static long lastlabel;

        return ( (char *) ++lastlabel );
#endif
}


/*
 * Putlab - lay down a label.
 *      for PC, just print the label name with a colon after it.
 */
char *
putlab(l)
        char *l;
{
#ifdef PC
        putprintf(PREFIXFORMAT , 1 , (int)LABELPREFIX , (int)l);
        putprintf(":" , 0 );
#endif
        return (l);
}

