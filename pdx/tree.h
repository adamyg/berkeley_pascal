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
 *	@(#)tree.h	8.1 (Berkeley) 6/6/93
 */

/*
 * This file contains the declarations of the variables and routines
 * within the "tree" subdirectory that are accessible from outside.
 */

#include "tree/opinfo.h"

/*
 * Evaluation stack manipulation macros.  These are publically
 * available because "eval" leaves it's result on the stack.
 *
 * These macros allow one to operate on stacks of arbitrary types
 * (including a stack of different typed objects).
 *
 * Sadly, underflow and overflow are not checked for.
 */

typedef char STACK;

#define WMASK			(sizeof(int) - 1)

#ifdef tahoe
#define push(type, value)	((*(type *)sp) = value, sp += (sizeof(type) + WMASK) & ~WMASK, value)
#define pop(type)		(sp -= (sizeof(type) + WMASK) & ~WMASK, (*((type *) sp)))
#else
#define push(type, value)	((type *) (sp += sizeof(type)))[-1] = (value)
#define pop(type)		(*((type *) (sp -= sizeof(type))))
#endif
#define alignstack()		sp = (char *) (( ((int) sp) + WMASK)&~WMASK)
#define downalignstack()	sp = (char *) (( ((int) sp))&~WMASK)

extern STACK		stack[];
extern STACK		*sp;

/*build.c*/
				/* create a node in the parse tree */
extern NODE		*build(OP op, ...);


/*prtree.c*/
				/* print a tree in source form */
extern void		prtree(NODE *p);

				/* backtrace error */
extern void		trerror(const char *s, NODE *tree, ...);


/*tree.c*/
				/* evaluate a tree, leaving value on stack */
extern void		eval(NODE *p);

				/* Push bytes onto the expression stack */
extern BOOLEAN		rpush(ADDRESS addr, int len);

				/* pop a small item from the stack given its type */
extern long		popsmall(SYM *t);

				/* evaluate a node for a conditional */
extern BOOLEAN		cond(NODE *p);

				/* return the object address of a node */
extern ADDRESS		lval(NODE *p);


/*tfree.c*/
				/* release storage for a tree */
extern void		tfree(NODE *p);


/*tr_equal.c*/
				/* test if two trees are structurally equivalent */
extern BOOLEAN		tr_equal(NODE *t1, NODE *t2);


/*misc.c*/
				/* TRUE if output is being redirected */
extern BOOLEAN		isredirected(void);

				/* Invoke an editor */
extern void		edit(const char *filename);

				/* Send mail to pdx support */
extern void		gripe(void);

				/* Give the user some help */
extern void		help(void);

				/* Divert output */
extern void		setout(const char *filename);
extern void		unsetout(void);


/*tracesto.c*/
extern void		trace(int cmd, NODE *exp, NODE *where, NODE *cond);
extern void		stop(int cmd, NODE *exp, NODE *where, NODE *cond);


/*assign.c*/
extern void		assign(NODE *var, NODE *exp);

/*end*/
