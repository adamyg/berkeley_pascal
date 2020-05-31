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

extern unsigned short tracing;
extern unsigned short var_tracing;
extern unsigned short inst_tracing;

extern BOOLEAN isstopped;

#define ss_lines                (tracing != 0)
#define ss_variables            (var_tracing != 0)
#define ss_instructions         (inst_tracing != 0)
#define single_stepping         (ss_lines || ss_variables || ss_instructions)

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
/*bp.c*/
                                /* add a variable to the trace list */
extern void             addbp(ADDRESS addr, BPTYPE type, SYM *block,
                                NODE *cond, NODE *node, LINENO line);

                                /* delete a breakpoint, return FALSE if unsuccessful */
extern void             delbp(unsigned int id);

                                /* free all breakpoint information */
extern void             bpfree(void);


/*trinfo.c*/
                                /* add a variable from the trace list */
extern void             addvar(TRTYPE trtype, NODE *node, NODE *cond);

                                /* delete a variable from the trace list */
extern void             delvar(TRTYPE trtype, NODE *node, NODE *cond);

                                /* print out variables that have changed */
extern void             prvarnews(void);
extern void             printnews(void);

                                /* free the entire trace list */
extern void             trfree(void);


/*trcond.c*/
                                /* add a condition to the list */
extern void             addcond(TRTYPE trtype, NODE *p);

                                /* delete a condition from the list */
extern void             delcond(TRTYPE trtype, NODE *p);

                                /* determine if any trace condition is true */
extern BOOLEAN          trcond(void);

                                /* free all existing breakpoints */
extern void             condfree(void);

                                /* determine if any stop condition is true */
extern BOOLEAN          stopcond(void);


/*setbps.c*/
                                /* set traps for all breakpoints */
extern void             setallbps(void);

                                /* remove traps at all breakpoints */
extern void             unsetallbps(void);


/*bpact.c*/
                                /* handle a breakpoint */
extern BOOLEAN          bpact(void);


/*fixbps.c*/
                                /* destroy temporary breakpoints left after a fault */
extern void             fixbps(void);


/*status.c*/
                                /* list items being traced */
extern void             status(void);

