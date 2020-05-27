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
 *
 *	@(#)opinfo.h	8.1 (Berkeley) 6/6/93
 */

/*
 * Tree operator definitions.
 */

struct opinfo {
	short opflags;
	char *opstring;
};

OPINFO opinfo[];

#define O_NOP	0	/* no op */
#define O_NAME	1	/* leaf op for address of a variable */
#define O_QNAME	2	/* variable qualified by surrounding block */
#define O_LCON	3	/* long constant (i.e. integer) */
#define O_FCON	4	/* floating constant (i.e. real) */
#define O_SCON	5	/* string constant (type alfa) */
#define O_INDEX	6	/* array subscript (does just one index) */
#define O_INDIR 7	/* indirection through a pointer */
#define O_RVAL	8	/* get value of an operand address */
#define O_COMMA	9	/* as in parameter lists */

/*
 * arithmetic operators
 */

#define O_ITOF	10	/* convert integer to real */
#define O_ADD	11
#define O_ADDF	12
#define O_SUB	13
#define O_SUBF	14
#define O_NEG	15
#define O_NEGF	16
#define O_MUL	17
#define O_MULF	18
#define O_DIVF	19	/* real divided by real */
#define O_DIV	20	/* integer divided by integer, integer result */
#define O_MOD	21

/*
 * logical operators
 */

#define O_AND	22
#define O_OR	23

/*
 * relational operators
 */

#define O_LT	24
#define O_LTF	25
#define O_LE	26
#define O_LEF	27
#define O_GT	28
#define O_GTF	29
#define O_GE	30
#define O_GEF	31
#define O_EQ	32
#define O_EQF	33
#define O_NE	34
#define O_NEF	35

/*
 * debugger commands
 */

#define O_ASSIGN 36
#define O_CHFILE 37
#define O_CONT	38
#define O_LIST	39
#define O_NEXT	40
#define O_PRINT	41
#define O_STEP	42
#define O_WHATIS 43
#define O_WHERE	44
#define O_XI	45
#define O_XD	46
#define O_CALL	47
#define O_EDIT	48
#define O_DUMP	49
#define O_HELP	50
#define O_REMAKE 51
#define O_RUN	52
#define O_SOURCE 53
#define O_STATUS 54
#define O_TRACE	55
#define O_TRACEI 56
#define O_STOP	57
#define O_STOPI	58
#define O_DELETE 59
#define O_WHICH 60
#define O_QLINE 61		/* filename : linenumber */
#define O_ALIAS 62
#define O_GRIPE 63

#define O_LASTOP 63		/* must be number of last operator */

/*
 * operator flags and predicates
 */

#define LEAF 01
#define UNARY 02
#define BINARY 04
#define BOOL 010
#define REALOP 020
#define INTOP 040

#define isbitset(a, m)	((a&m) == m)
#define isleaf(o)	isbitset(opinfo[o].opflags, LEAF)
#define isunary(o)	isbitset(opinfo[o].opflags, UNARY)
#define isbinary(o)	isbitset(opinfo[o].opflags, BINARY)
#define isreal(o)	isbitset(opinfo[o].opflags, REALOP)
#define isint(o)	isbitset(opinfo[o].opflags, INTOP)
#define isboolean(o)	isbitset(opinfo[o].opflags, BOOL)

#define degree(o)	(opinfo[o].opflags&(LEAF|UNARY|BINARY))
