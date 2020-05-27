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
 *	@(#)breakpoint.h	8.1 (Berkeley) 6/6/93
 */

/*
 * Breakpoint module definitions.
 *
 * This module contains routines that manage breakpoints at a high level.
 * This includes adding and deleting breakpoints, handling the various
 * types of breakpoints when they happen, management of conditions for
 * breakpoints, and display information after single stepping.
 */

unsigned short tracing;
unsigned short var_tracing;
unsigned short inst_tracing;

BOOLEAN isstopped;

#define ss_lines		(tracing != 0)
#define ss_variables		(var_tracing != 0)
#define ss_instructions		(inst_tracing != 0)
#define single_stepping		(ss_lines || ss_variables || ss_instructions)

/*
 * types of breakpoints
 */

typedef enum {
	ALL_ON,			/* turn TRACE on */
	ALL_OFF,		/* turn TRACE off */
	INST,			/* trace instruction (source line) */
	CALL, RETURN,		/* trace procedure/function */
	BLOCK_ON,		/* set CALL breakpoint */
	BLOCK_OFF,		/* clear CALL breakpoint */
	TERM_ON,		/* turn TRACEVAR on */
	TERM_OFF,		/* turn TRACEVAR off */
	AT_BP,			/* print expression at a line */
	STOP_BP,		/* stop execution */
	CALLPROC,		/* return from a "call"-ed procedure */
	END_BP,			/* return from program */
	STOP_ON,		/* start looking for stop condition */
	STOP_OFF,		/* stop looking for stop condition */
} BPTYPE;

/*
 * Things that are on the tracing or condition list are either
 * associated with the trace (implying printing) or stop commands.
 */

typedef enum { TRPRINT, TRSTOP } TRTYPE;

/*
 * routines available from this module
 */

int addvar();		/* add a variable to the trace list */
int delvar();		/* delete a variable from the trace list */
int printvarnews();	/* print out variables that have changed */
int trfree();		/* free the entire trace list */
int addcond();		/* add a condition to the list */
int delcond();		/* delete a condition from the list */
BOOLEAN trcond();	/* determine if any trace condition is true */
BOOLEAN stopcond();	/* determine if any stop condition is true */

int addbp();		/* add a breakpoint */
int delbp();		/* delete a breakpoint, return FALSE if unsuccessful */
int bpfree();		/* free all breakpoint information */
int setallbps();	/* set traps for all breakpoints */
int unsetallbps();	/* remove traps at all breakpoints */
BOOLEAN bpact();	/* handle a breakpoint */
int fixbps();		/* destroy temporary breakpoints left after a fault */
int status();		/* list items being traced */
