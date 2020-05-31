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
 *	@(#)runtime.h	8.1 (Berkeley) 6/6/93
 */

/*
 * Definitions for the runtime environment.
 *
 * In general, runtime organization is language, machine, and
 * even compiler dependent.
 */

/*isactive.c*/

				/* tell if a symbol is currently active */
extern BOOLEAN		isactive(SYM *f);


/*entry.c*/

				/* return address of current proc/func */
extern ADDRESS		return_addr(void);

				/* entry of caller of current proc/func */
extern ADDRESS		caller_addr(void);

				/* entry address of a function parameter */
extern ADDRESS		fparamaddr(ADDRESS a);

/*x.c*/
				/* print out currently active procedures */
extern void		where(void);

				/* dump the world */
extern void		dump(void);

extern void		callproc(NODE *procnode, NODE *arglist);

extern void		procreturn(SYM *f);

				/* address of a variable */
extern ADDRESS		address(register SYM *s, FRAME *frame);

				/* address of first line in a procedure */
extern ADDRESS		firstline(SYM *f);

				/* find address of beginning of a procedure */
extern void		findbeginning(SYM *f);

				/* step to first line in current procedure */
extern void		runtofirst(void);

				/* address of last line in program */
extern ADDRESS		lastaddr(void);

