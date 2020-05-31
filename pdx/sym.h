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
 *	@(#)sym.h	8.1 (Berkeley) 6/6/93
 */

typedef char		*ARGLIST;

/*
 * This header file declares the variables and routines that
 * are defined within the "sym" subdirectory and that can be
 * accessed from outside.
 */

extern SYM		*program;		/* program symbols */

/*
 * attributes
 */
/*attributes.c*/
				/* symbol string name */
extern const char	*name(SYM *s);

				/* class name of a symbol */
extern const char	*classname(SYM *s);

				/* token number of reserved word */
extern int		toknum(SYM *s);

				/* associated token value */
extern int		tokval(SYM *s);

				/* size in bytes of a type */
extern int		size(SYM *t);

				/* the reduced type, i.e. no type names */
extern SYM		*rtype(SYM *t);

				/* symbol (block) that contains given symbol */
extern SYM		*container(SYM *s);

				/* address of the beginning of a function */
extern ADDRESS		codeloc(SYM *f);


/*
 * predicates
 */

/*predicates.c*/
				/* test if a symbol is a block name */
extern BOOLEAN		isblock(SYM *s);

				/* test if a symbol is a builtin type */
extern BOOLEAN		isbuiltin(SYM *s);

				/* test if a symbol is a parameter */
extern BOOLEAN		isparam(SYM *s);

				/* test if a symbol is a var parameter */
extern BOOLEAN		isvarparam(SYM *s);

				/* test if a symbol is a variable */
extern BOOLEAN		isvariable(SYM *s);

				/* test if a symbol name is not unique */
extern BOOLEAN		isambiguous(SYM *s);

				/* test to see if two types are compatible */
extern BOOLEAN		compatible(SYM *t1, SYM *t2);

				/* test if a symbol should be printed */
extern BOOLEAN		should_print(SYM *s, SYM *f);


/*which.c*/
				/* read in a symbol from object file */
extern SYM		*readsym(FILE *fp);

				/* find the appropriate symbol of given name */
extern SYM		*which(SYM *s);

				/* find a symbol for a given block */
extern SYM		*findsym(SYM *s, SYM *f);

				/* find symbol with given class */
extern SYM		*findclass(SYM *s, char cl);


/*tree.c*/
				/* construct a tree for the dot operator */
extern NODE		*dot(NODE *record, SYM *field);

				/* construct a tree for subscripting */
extern NODE		*subscript(NODE *a, NODE *slist);

				/* return the type of a tree, checking also */
extern SYM		*treetype(NODE *p, va_list);

				/* evaluate a subscript index */
extern long		evalindex(SYM *arraytype, NODE *subs);

				/* free a constant string type */
extern void		unmkstring(SYM *s);

				/* check if a tree is boolean-valued */
extern void		chkboolean(NODE *p);


/*printdec.c*/
				/* print out a symbol's declaration */
extern void		printdecl(SYM *s);
extern void		listparams(SYM *s);


/*print.c*/
				/* print out values of a fn's parameters */
extern void		printparams(SYM *f, FRAME *frame);

				/* note entrance of a block */
extern void		printentry(SYM *s);

				/* note exiting from a block */
extern void		printexit(SYM *s);

				/* note call of a function */
extern void		printcall(SYM *s, SYM *t);

				/* note return of a function */
extern void		printrtn(SYM *s);

				/* print the name and value of a variable */
extern void		printv(SYM *s, FRAME *frame);

				/* print the full "path" of an identifier */
extern void		printwhich(SYM *s);


/*printval.c*/
				/* print an eval result */
extern void		printval(SYM *s);
extern void		printordinal(long v, SYM *t);


/*maketypes.c*/
				/* initialize basic types */
extern void		maketypes(void);

				/* create a keyword in a given symbol table */
extern void		make_keyword(SYMTAB *table, char *name, int tnum);


