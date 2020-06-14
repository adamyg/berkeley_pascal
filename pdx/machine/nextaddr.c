/* -*- mode: c; tabs: 4; hard-tabs: yes; -*- */
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

#if !defined(lint) && defined(SCCSID)
static char sccsid[] = "@(#)nextaddr.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * Calculate the next address that will be executed from the current one.
 *
 * If the next address depends on runtime data (e.g. a conditional
 * branch will depend on the value on top of the stack),
 * we must execute up to the given address with "stepto".
 *
 * If the second argument is TRUE, we treat a CALL instruction as
 * straight line rather than following it as a branch.
 */

#include "defs.h"
#include "machine.h"
#include "process/process.h"
#include "breakpoint.h"
#include "sym.h"
#include "pxops.h"
#include "optab.h"
#include "mappings.h"
#include "runtime.h"
#include "process/pxinfo.h"
#include "process/process.rep"

#include <assert.h>

#ifdef tahoe
#define EVEN 3
#else
#define EVEN 1
#endif

#if defined(_WIN32)
#define DISASSEMBLE
#endif

#if defined(PXEMBEDDED)
#define PROCESS_SP(__p) (__p->isp)
#else
#define PROCESS_SP(__p) (__p->sp)
#endif

#define DISWIDTH    41      /* right column */
#define DISASMTAB   16      /* data tab column */
typedef struct {
    char rbuffer[1024];     /* right/hex column buffer */
    int  rlength;
    int  cursor;            /* line cursor */
    int  words;             /* number of elements/words */
} disasmline_t;

LOCAL ADDRESS docase(int ncases, int size, ADDRESS addr);
LOCAL ADDRESS dofor(int size, ADDRESS addr, short subop, int incr);

LOCAL void asm_col0(disasmline_t *line, ADDRESS addr, const char *fmt, ...);
LOCAL void asm_col1(disasmline_t *line, ADDRESS addr, const char *fmt, ...);
LOCAL void asm_colx(disasmline_t *line, BOOLEAN isnext, ADDRESS addr, const char *fmt, va_list ap);
LOCAL void asm_colf(disasmline_t *line, const char *fmt, ...);
LOCAL void asm_string(disasmline_t *line, const char *cbuffer, int slen);
LOCAL void asm_word(disasmline_t *line, short word);
LOCAL void asm_addr(disasmline_t *line, ADDRESS addr);
LOCAL void asm_flush(disasmline_t *line);


ADDRESS
nextaddr(ADDRESS beginaddr, BOOLEAN isnext)
{
    return nextaddrx(beginaddr, isnext, 0);
}


ADDRESS
nextaddrx(ADDRESS beginaddr, BOOLEAN isnext, int disasm)
{
    disasmline_t line = {0};
    register PXOP op;
    ADDRESS addr;
    short offset;
    int nextbyte;
    SYM *s;
    union {
	short word;
	short words[2];
	char  byte[2];
    } o;

#ifdef tahoe
    doret(process);
#endif
    addr = beginaddr;
    iread(&o.word, addr, sizeof(o.word));
    op = (PXOP) o.byte[0];
    nextbyte = o.byte[1];

#if defined(DISASSEMBLE)
    if (disasm) {
	if (1 == disasm) fputc('\n', stdout);
	asm_col0(&line, addr, "%s", optab[op].opname);
	asm_word(&line, o.word);
    }
#endif
 
    addr += sizeof(short);
    switch(op) {

    /*
     * The version of px we are using assumes that the instruction
     * at the entry point of a function is a TRA4 to the beginning
     * of the block.
     */
	case O_CALL: {
	    ADDRESS eaddr;

	    if (isnext) {
		addr += sizeof(int);
#ifdef tahoe
		addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
#endif
	    } else {
#ifdef tahoe
		addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
#endif
		iread(&eaddr, addr, sizeof(eaddr));
#if defined(DISASSEMBLE)
		if (disasm) {
		    asm_colf(&line, ":%ld", (long) addr);
		    asm_addr(&line, addr);
		}
#endif
		addr = eaddr + sizeof(short);
#ifdef tahoe
		addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
#endif
		iread(&addr, addr, sizeof(addr));
#if defined(DISASSEMBLE)
		if (disasm) asm_addr(&line, addr);
#endif
		stepto(addr);
		if (linelookup(addr) == 0) {
		    bpact();
		    addr = pc;
		}
		if (ss_lines && trcond()) {
		    s = whatblock(addr);
		    if (s == NIL) {
			panic("bad call addr");
		    }
		    printentry(s);
		}
	    }
	    break;
	}

	case O_FCALL: {
	    ADDRESS eaddr;
	    ADDRESS fparam;

	    if (!isnext) {
		stepto(addr - sizeof(short));
#ifdef tahoe
		doret(process);
#endif
		dread(&fparam, PROCESS_SP(process) + sizeof(ADDRESS), sizeof(fparam));
		dread(&eaddr, fparam, sizeof(eaddr));
		addr = eaddr - ENDOFF;
		stepto(addr);
#ifdef tahoe
		doret(process);
#endif
		if (linelookup(addr) == 0) {
		    bpact();
		    addr = pc;
		}
		if (ss_lines && trcond()) {
		    s = whatblock(addr);
		    if (s == NIL) {
			panic("bad call addr");
		    }
		    printentry(s);
		}
	    }
	    break;
	}

	case O_END:
	    if ((addr - sizeof(short)) == lastaddr()) {
		stepto(addr - sizeof(short));
		endprogram();
	    } else {
		addr = return_addr();
		s = whatblock(pc);
		stepto(addr);
		if (ss_lines && trcond()) {
		    printexit(s);
		}
		if (linelookup(addr) == 0) {
		    bpact();
		    addr = pc;
		}
	    }
	    break;

	case O_TRA4:
	case O_GOTO:
#ifdef tahoe
	    addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
#endif
	    iread(&addr, addr, sizeof(addr));
#if defined(DISASSEMBLE)
	    if (disasm) {
		asm_colf(&line, ":%ld", (long) addr);
		asm_addr(&line, addr);
	    }
#endif
	    break;

	case O_TRA:
	    iread(&offset, addr, sizeof(offset));
#if defined(DISASSEMBLE)
	    if (disasm) {
		asm_colf(&line, ":%d", offset);
		asm_word(&line, offset);
	    }
#endif
	    addr += offset;
	    break;

	case O_CON: {
	    short consize;

	    if (nextbyte == 0) {
#ifdef tahoe
		addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
#endif
		iread(&consize, addr, sizeof(consize));
#if defined(DISASSEMBLE)
		if (disasm) {
		    asm_colf(&line, ":%d", consize);
		    asm_word(&line, consize);
		}
#endif
		addr += sizeof(consize);
	    } else {
		consize = nextbyte;
	    }
	    addr += consize;
	    break;
	}

	case O_CASE1OP:
	    addr = docase(nextbyte, 1, addr);
	    break;

	case O_CASE2OP:
	    addr = docase(nextbyte, 2, addr);
	    break;

	case O_CASE4OP:
	    addr = docase(nextbyte, 4, addr);
	    break;

	case O_FOR1U:
	    addr = dofor(2, addr, nextbyte, 1);
	    break;

	case O_FOR2U:
	    addr = dofor(2, addr, nextbyte, 1);
	    break;

	case O_FOR4U:
	    addr = dofor(4, addr, nextbyte, 1);
	    break;

	case O_FOR1D:
	    addr = dofor(2, addr, nextbyte, -1);
	    break;

	case O_FOR2D:
	    addr = dofor(2, addr, nextbyte, -1);
	    break;

	case O_FOR4D:
	    addr = dofor(4, addr, nextbyte, -1);
	    break;

	case O_IF:
	    stepto(addr - sizeof(short));
#ifdef tahoe
	    doret(process);
	    dread(&offset, PROCESS_SP(process)+sizeof(int)-sizeof(offset), sizeof(offset));
#else
	    dread(&offset, PROCESS_SP(process), sizeof(offset));
#endif
#if defined(DISASSEMBLE)
	    if (disasm) {
		asm_colf(&line, " %d", offset);
	    }
#endif
	    if (offset == 0) {
		iread(&offset, addr, sizeof(offset));
		addr += offset;
	    } else {
		addr += sizeof(offset);
	    }
	    break;

	default: {
	    int i;

	    for (i = 0; optab[op].argtype[i] != 0; i++) {
		ARGTYPE argtype = optab[op].argtype[i];
		switch(optab[op].argtype[i]) {
		    case ADDR4:
		    case LWORD:
#if defined(DISASSEMBLE)
			if (disasm) {
			    ADDRESS t_addr = 0;
			    iread(&t_addr, addr, sizeof(t_addr));
			    asm_col0(&line, addr, "%s", (argtype == ADDR4 ? "addr4" : "lword"));
			    asm_colf(&line, "%ld", (long)addr);
			    asm_addr(&line, t_addr);
			}
#endif
			addr += 4;
#ifdef tahoe
			addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
#endif
			break;

		    case SUBOP:
#if defined(DISASSEMBLE)
			if (disasm) {
                            asm_colf(&line, ":%d", nextbyte);
			}
#endif
			break;

		    case ADDR2:
		    case HWORD:
		    case PSUBOP:
		    case DISP:
		    case VLEN:
			if (i != 0 || nextbyte == 0) {
#if defined(DISASSEMBLE)
			    if (disasm) {
				iread(&o.word, addr, sizeof(o.word));
				asm_colf(&line, (i ? " %d" : ":%d"), o.word);
				asm_word(&line, o.word);
			    }
#endif
			    addr += sizeof(short);
			} else {
#if defined(DISASSEMBLE)
			    if (disasm) {
				asm_colf(&line, ":%d", nextbyte);
			    }
#endif
			}
			break;

		    case STRING: {
#if defined(DISASSEMBLE)
                        char cbuffer[256]; int slen = 0;
#endif
			char c;

                        assert(nextbyte <= 255);
			while (nextbyte > 0) {
			    iread(&c, addr, 1);
			    if (c == '\0') {
				break;
			    }
#if defined(DISASSEMBLE)
			    if (disasm) cbuffer[slen++] = c;
#endif
			    nextbyte--;
			    addr++;
			}
#if defined(DISASSEMBLE)
                        if (disasm) asm_string(&line, cbuffer, slen);
#endif
			addr++;
			addr = (ADDRESS)(((int)addr + EVEN) & ~EVEN);
			break;
		    }

		    default:
			panic("bad argtype (%u)", argtype);
			/*NOTREACHED*/
		}
	    }
	    break;
	}
    }
    if (disasm) asm_flush(&line);
    return addr;
}


/*
 * Find the next address that will be executed after the
 * case statement at the given address.
 */

LOCAL ADDRESS 
docase(int ncases, int size, ADDRESS addr)
{
    register ADDRESS i;
    ADDRESS firstval, lastval, jmptable;
    short offset;
    long swtval, caseval;

    stepto(addr - 2);
#ifdef tahoe
    doret(process);
#endif
    if (ncases == 0) {
	iread(&ncases, addr, sizeof(ncases));
	addr += sizeof(short);
    }
    jmptable = addr;
    firstval = jmptable + ncases*sizeof(short);
#ifdef tahoe
    if (size == 4) {
	firstval = (ADDRESS)(((int)firstval + EVEN) & ~EVEN);
    }
#endif
    lastval = firstval + ncases*size;
#ifdef tahoe
    if (size <= 4) {
	dread(&swtval, PROCESS_SP(process), 4);
#else
    if (size <= 2) {
	dread(&swtval, PROCESS_SP(process), 2);
#endif
    } else {
	dread(&swtval, PROCESS_SP(process), size);
    }
    for (i = firstval; i < lastval; i += size) {
	caseval = 0;
#ifdef tahoe
	iread((char *)&caseval + sizeof caseval - size, i, size);
	if (swtval == caseval)
#else
	iread(&caseval, i, size);
	if (cmp(&swtval, &caseval, size) == 0)
#endif
	{
	    i = ((i - firstval) / size) * sizeof(offset);
	    iread(&offset, jmptable + i, sizeof(offset));
	    addr = jmptable + offset;
	    return addr;
	}
    }
    return((lastval+1)&~1);
}

LOCAL ADDRESS 
dofor(int size, ADDRESS addr, short subop, int incr)
{
    register PROCESS *p;
    long i, limit;
    ADDRESS valaddr;

    stepto(addr - sizeof(short));
    p = process;
#ifdef tahoe
    doret(p);
#endif
    i = limit = 0;
    if (subop == 0) {
	iread(&subop, addr, sizeof (short)); /*APY, was dread()*/
	addr += sizeof (short);
    }
    dread(&valaddr, PROCESS_SP(p), sizeof(valaddr));
#ifdef tahoe
    dread((char *)&i + sizeof i - size, valaddr, size);
#else
    dread(&i, valaddr, size);
#endif
    dread(&limit, PROCESS_SP(p) + sizeof(valaddr), sizeof limit);
    i += incr;

/*
 * It is very slow to go through the loop again and again.
 * If it is desired to just skip to the end, the next 4 lines
 * should be skipped.
 */
    if ((incr > 0 && i < limit) || (incr < 0 && i > limit)) {
	return(addr + subop);
    } else {
	return(addr);
    }
}

/*
 * Determine whether or not the given address corresponds to the
 * end of a procedure.
 */

BOOLEAN 
isendofproc(ADDRESS addr)
{
    PXOP op;

    iread(&op, addr, sizeof(op));
    return (op == O_END);
}


/*
 * Simple disassembler formatters
 */

LOCAL void
asm_col0(disasmline_t *line, ADDRESS addr, const char *fmt, ...)
{
    int len1, len2 = 0;

    asm_flush(line);
    len1 = fprintf(stdout, "%04x ", addr);
    if (fmt && *fmt) {
	va_list ap;
	va_start(ap, fmt);
	len2 = vfprintf(stdout, fmt, ap);
	va_end(ap);
    }
    line->cursor = (len1 + len2);
    line->words = 1;
}


LOCAL void
asm_colf(disasmline_t *line, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    if (line->words++ >= 1) {
	if (*fmt != ':' && line->cursor < DISASMTAB) {
	    line->cursor += fprintf(stdout, "%*s", DISASMTAB - line->cursor, "");
	}
    }
    line->cursor += vfprintf(stdout, fmt, ap);
    va_end(ap);
}


LOCAL void
asm_string(disasmline_t *line, const char *s, int slen)
{
    unsigned b;

#define STRWIDTH    (DISWIDTH - 4)
    while (slen > 0) {
	asm_flush(line);

	if (slen < STRWIDTH) {
	    asm_colf(line, "  \"%.*s\"%*s", slen, s, STRWIDTH - slen, "");
        } else {
	    asm_colf(line, "  \"%*s\"", STRWIDTH, s);
	}

	for (b = 0; b < STRWIDTH && slen; b += 2) {
#if defined(DEC11)
	    short word= (*s++);
	    if (--slen) { /* strings are word padded */
		word |= (*s++ << 8), --slen;
            }
#else
	    short word= (*s++ << 8);
	    if (--slen) { /* strings are word padded */
		word |= *s++, --slen;
            }
#endif
	    asm_word(line, word);
	}
    }
}


LOCAL void
asm_word(disasmline_t *line, short word)
{
    const int rlength = line->rlength;
    if (rlength < sizeof(line->rbuffer)) {
	int space = sizeof(line->rbuffer) - rlength,
	  len = snprintf(line->rbuffer + rlength, space, "%04hx ", word);

	line->rlength += ((len < 0 || len >= space) ? space : len);
    }
}


LOCAL void
asm_addr(disasmline_t *line, ADDRESS addr)
{
#if defined(ADDR32)
    assert(sizeof(ADDRESS) == 4);
#if defined(DEC11)
    asm_word(line, addr & 0xffff);
    asm_word(line, (addr >> 16) & 0xffff);
#else
    asm_word(line, (addr >> 16) & 0xffff);
    asm_word(line, addr & 0xffff);
#endif

#else
    assert(sizeof(ADDRESS) == 2);
    asm_word(line, addr);
#endif
}


LOCAL void
asm_flush(disasmline_t *line)
{
    const int rlength = line->rlength;
    if (rlength) {
	if (line->cursor < DISWIDTH) {
	    fprintf(stdout, "%*s", line->cursor - DISWIDTH, "");
	}
	fprintf(stdout, " %.*s\n", rlength, line->rbuffer);

    } else if (line->cursor) {
	fputc('\n', stdout);
    }
    line->cursor = line->rlength = line->words = 0;
}
