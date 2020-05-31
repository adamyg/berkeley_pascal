/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
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
 *	@(#)machine.h	8.1 (Berkeley) 6/6/93
 */

/*
 * Definitions for the machine module.
 *
 * This module has the low level machine interface.  This consists
 * mostly of dealing with machine instructions and also setting
 * and unsetting breakpoints.
 */

extern ADDRESS	pc;			/* current program counter */
extern LINENO	curline;		/* line number associated with pc */
extern SYM	*curfunc;		/* pointer to active function symbol */

				/* set a breakpoint */
extern void		setbp(ADDRESS addr);

				/* unset a breakpoint */
extern void		unsetbp(ADDRESS addr);

				/* test if a breakpoint has occurred */
extern BOOLEAN		isbperr(void);

				/* print out an execution error message */
extern void		printerror(void);

				/* address of next line to be executed */
extern ADDRESS		nextaddrx(ADDRESS beginaddr, BOOLEAN isnext, int disasm);
extern ADDRESS		nextaddr(ADDRESS beginaddr, BOOLEAN isnext);

				/* test if address is at end of procedure */
extern BOOLEAN		isendofproc(ADDRESS addr);

				/* print the instruction at a given address */
extern void		printinst(ADDRESS lowaddr, ADDRESS highaddr);

				/* print the contents of a given data address */
extern void		printdata(ADDRESS lowaddr, ADDRESS highaddr);

