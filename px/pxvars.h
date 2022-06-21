/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
/*-
 * Copyright (c) 1980 The Regents of the University of California.
 * All rights reserved.
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
 *      @(#)vars.h      5.4 (Berkeley) 4/16/91
 */

/*
 * px - Berkeley Pascal interpreter
 *
 * Version 4.0, January 1981
 *
 * Original version by Ken Thompson
 *
 * Substantial revisions by Bill Joy and Chuck Haley
 * November-December 1976
 *
 * Rewritten for VAX 11/780 by Kirk McKusick
 * Fall 1978
 *
 * Rewritten in ``C'' using libpc by Kirk McKusick
 * Winter 1981
 *
 * px is described in detail in the "PX 4.0 Implementation Notes"
 * The source code for px is in several major pieces:
 *
 *      int.c           C main program which reads in interpreter code
 *      interp.c        Driver including main interpreter loop and
 *                      the interpreter instructions grouped by their
 *                      positions in the interpreter table.
 *      utilities.c     Interpreter exit, backtrace, and runtime statistics.
 *
 * In addition there are several headers defining mappings for panic
 * names into codes, and a definition of the interpreter transfer
 * table. These are made by the script make.ed1 in this directory and
 * the routine opc.c from ${PASCALDIR}. (see the makefile for details)
 */

#define PX		0			/* normal run of px */
#define PIX		1			/* load and go */
#define PIPE		2			/* bootstrap via a pipe */
#define PDX		3			/* invoked by the debugger "pdx" */
#define releq		0
#define relne		2
#define rellt		4
#define relgt		6
#define relle		8
#define relge		10



/*
 * stack routines and structures
 */
struct sze8 {
        char element[8];
};


/*
 * emulated pc types
 */
union progcntr {
	signed char	*cp;
	unsigned char	*ucp;
	signed short	*sp;
	unsigned short	*usp;
	signed long	*lp;
	double		*dbp;
	struct hdr	*hdrp;
	struct sze8	*s8p;
};



/*
 * THE RUNTIME DISPLAY
 *
 * The entries in the display point to the active static block marks.
 * The first entry in the display is for the global variables,
 * then the procedure or function at level one, etc.
 * Each display entry points to a stack frame as shown:
 *
 *              base of stack frame
 *                ---------------
 *                |             |
 *                | block mark  |
 *                |             |
 *                ---------------  <-- display entry "stp" points here
 *                |             |  <-- display entry "locvars" points here
 *                |   local     |
 *                |  variables  |
 *                |             |
 *                ---------------
 *                |             |
 *                |  expression |
 *                |  temporary  |
 *                |  storage    |
 *                |             |
 *                - - - - - - - -
 *
 * The information in the block mark is thus at positive offsets from
 * the display.stp pointer entries while the local variables are at negative
 * offsets from display.locvars. The block mark actually consists of
 * two parts. The first part is created at CALL and the second at entry,
 * i.e. BEGIN. Thus:
 *
 *              -------------------------
 *              |                       |
 *              |  Saved lino           |
 *              |  Saved lc             |
 *              |  Saved dp             |
 *              |                       |
 *              -------------------------
 *              |                       |
 *              |  Saved (dp)           |
 *              |                       |
 *              |  Pointer to current   |
 *              |   routine header info |
 *              |                       |
 *              |  Saved value of       |
 *              |   "curfile"           |
 *              |                       |
 *              |  Empty tos value      |
 *              |                       |
 *              -------------------------
 */



/*
 * program variables
 */
extern union display	_display;		/* runtime display */
extern struct dispsave	*_dp;			/* ptr to active frame */
extern long		*_pcpcount;		/* pointer to pxp buffer */
extern long		_cntrs; 		/* number of counters */
extern long		_lino;			/* current line number */
extern long		_mode;			/* execl by PX, PIPE, or PIX */
extern long		_rtns;			/* number of routine cntrs */
extern long		_runtst;		/* TRUE => runtime tests */
extern bool		_nodump;		/* TRUE => no post mortum dump */
extern long		_createtime;		/* image creation time */
						/* source version.c */


/*
 * Px execution profile array
 */
#ifdef   PROFILE
#define  NUMOPS 	256
extern long		_profcnts[NUMOPS];
#endif /*PROFILE*/
