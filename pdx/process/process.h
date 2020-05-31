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
 *	@(#)process.h	8.1 (Berkeley) 6/6/93
 */

/*
 * Definitions for process module.
 *
 * This module contains the routines to manage the execution and
 * tracing of the debuggee process.
 */

typedef struct process PROCESS;

extern PROCESS		*process;

				/* initial process start up */
extern void		initstart(void);

				/* start program running */
extern void		run(void);

				/* restart program */
extern void		restart(void);	

				/* initialize program arguments */
extern void		arginit(void);

				/* special argument handling */
extern void		setargs(const char *m, const char *r);

				/* add a new argument to list for program */
extern void		newarg(const char *arg);

				/* set standard input for program */
extern void		inarg(const char *filename);

				/* set standard output for program */
extern void		outarg(const char *filename);

				/* continue execution where last left off */
extern void		cont(void);
extern void		fixintr(void);

				/* single step */
extern void		step(void);

				/* single step command */
extern void		stepc(int cnt);

				/* execute up to a given address */
extern void		stepto(ADDRESS addr);

				/* single step, skip over calls */
extern void		next(int cnt);

				/* note the termination of the program */
extern void		endprogram();

				/* print current error */
extern void		printstatus(void);

				/* print current source line and file */
extern void		printwhere(LINENO lineno, const char *filename);

				/* TRUE if process has terminated */
extern BOOLEAN		isfinished(PROCESS *p);


/* process.rep */
/*int iread()	--*/		/* read from the process' address space */
/*int dread()	--*/

/*int iwrite()	--*/		/* write to the process' address space */		
/*int dwrite()  --*/

/*resume.c*/
extern void		resume(void);


