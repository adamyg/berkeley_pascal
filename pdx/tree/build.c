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
 */

#if !defined(lint) && defined(SCCSID)
static char sccsid[] = "@(#)build.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * parse tree building routines
 *
 * Semantics is not checked here, this is done by the "treetype" routine
 * in the SYM directory which returns the type of the newly built tree.
 */

#include "defs.h"
#include "tree.h"
#include "sym.h"
#include "source.h"
#include "tree.rep"

/*
 * build a tree
 */

/*VARARGS1*/
NODE *
build(OP op, ...)
{
	register NODE *p;
	va_list ap;

	p = alloc(1, NODE);
	p->op = op;
	va_start(ap, op);
	switch(degree(op)) {
		case BINARY:
			p->left = va_arg(ap, NODE *);
			p->right = va_arg(ap, NODE *);
			break;

		case UNARY:
			p->left = va_arg(ap, NODE *);
			p->right = NIL;
			break;

	}
	switch(op) {
		case O_NAME:
		case O_WHICH:
			p->nameval = va_arg(ap, SYM *);
			break;

		case O_LCON:
			p->lconval = va_arg(ap, long);
			break;

		case O_FCON:
			p->fconval = va_arg(ap, double);
			break;

		case O_SCON:
			p->sconval = va_arg(ap, char *);
			break;

		case O_CALL:
			p->left = va_arg(ap, NODE *);
			p->right = va_arg(ap, NODE *);
			break;

		case O_CHFILE:
			p->sconval = va_arg(ap, char *);
			break;

		case O_EDIT:
			p->sconval = va_arg(ap, char *);
			if (p->sconval == NIL) {
				p->sconval = (char *)cursource;
			}
			break;

		case O_SOURCE:
			p->sconval = va_arg(ap, char *);
			break;

		case O_PRINT:
		case O_WHATIS:
		case O_LIST:
		case O_XI:
		case O_XD:
			p->left = va_arg(ap, NODE *);
			break;

		case O_TRACE:
		case O_TRACEI:
		case O_STOP:
		case O_STOPI:
			p->what = va_arg(ap, NODE *);
			p->where = va_arg(ap, NODE *);
			p->cond = va_arg(ap, NODE *);
			break;

		case O_DELETE:
		case O_STEP:
		case O_NEXT:
			p->left = build(O_LCON, va_arg(ap, long));
			break;

		case O_QLINE: {
			char *s;

			s = va_arg(ap, char *);
			p->left = alloc(1, NODE);
			p->left->op = O_SCON;
			if (s != cursource) {
				p->left->sconval = s;
				s[strlen(s) - 1] = '\0';
			} else {
				p->left->sconval = strdup(s);
			}
			p->right = va_arg(ap, NODE *);
			break;
		}

		case O_ALIAS:
			p->left = alloc(1, NODE);
			p->left->op = O_SCON;
			p->left->sconval = va_arg(ap, char *);
			p->right = alloc(1, NODE);
			p->right->op = O_SCON;
			p->right->sconval = va_arg(ap, char *);
			break;
			
		default:
			if (op < O_NOP || op > O_LASTOP) {
				panic("build: bad op %d", op);
			}
			break;
	}
	va_start(ap, op);
	p->nodetype = treetype(p, ap);
	return(p);
}
