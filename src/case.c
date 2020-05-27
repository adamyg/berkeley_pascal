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

#ifndef lint
static char sccsid[] = "@(#)case.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

#include "whoami.h"
#include "0.h"
#include "tree.h"
#include "opcode.h"
#include "tree_ty.h"

/*
 * The structure used to
 * hold information about
 * each case label.
 */
struct ct {
	long	clong;
	int	cline;
};

#ifdef OBJ
/*
 * Caseop generates the
 * pascal case statement code
 */
caseop(rnode)
	WHI_CAS *rnode;
{
	register struct nl *p;
	register struct ct *ctab;
	register struct tnode *cs;
	extern char *lc;
	double low, high;
	short *brtab;
	char *brtab0;
	char *csend;
	int w, j, m, n;
	int goc;
	bool nr;

	goc = gocnt;
	/*
	 * Obtain selector attributes:
	 *	p	type
	 *	w	width
	 *	low	lwb(p)
	 *	high	upb(p)
	 */
	p = rvalue(rnode->expr, NLNIL , RREQ );

	{
	    register struct nl	*cl;

	if (p != NLNIL) {
		if (isnta(p, "bcsi")) {
			error("Case selectors cannot be %ss", nameof(p));
			p = NLNIL;
		} else {
			cl = p;
			if (p->class != (char) RANGE)
				cl = p->type;
			if (cl == NLNIL)
				p = NLNIL;
			else {
				w = width(p);
#ifdef DEBUG
				if (hp21mx)
					w = 2;
#endif
				low = cl->range[0];
				high = cl->range[1];
			}
		}
	}
	} /* local declaration */
	{
	    struct tnode	*cl;	/* list node */
	/*
	 * Count # of cases
	 */
	n = 0;
	for (cl = rnode->stmnt_list; cl != TR_NIL;
		cl = cl->list_node.next) {
		cs = cl->list_node.list;;
		if (cs == TR_NIL)
			continue;
		for (cs = cs->c_stmnt.const_list; cs != TR_NIL;
				cs = cs->list_node.next)
			n++;
	}
	} /* local declaration */
	/*
	 * Allocate case table space
	 */
	{
		char *i;
	i = malloc((unsigned) n * sizeof *ctab);
	if (i == 0) {
		error("Ran out of memory (case)");
		pexit(DIED);
	}
	ctab = (struct ct *) i;
	}
	/*
	 * Check the legality of the
	 * labels and count the number
	 * of good labels
	 */
	{
	    register struct tnode *cl;
	m = 0;
	for (cl = rnode->stmnt_list; cl != TR_NIL;
		cl = cl->list_node.next) {
		cs = cl->list_node.list;
		if (cs == TR_NIL)
			continue;
		line = cs->c_stmnt.line_no;
		for (cs = cs->c_stmnt.const_list; cs != TR_NIL;
				cs =  cs->list_node.next) {
			gconst(cs->list_node.list);
			if (p == NLNIL || con.ctype == NIL)
				continue;
			if (incompat(con.ctype, p, TR_NIL )) {
				cerror("Case label type clashed with case selector expression type");
				continue;
			}
			if (con.crval < low || con.crval > high) {
				error("Case label out of range");
				continue;
			}
			ctab[m].clong = con.crval;
			ctab[m].cline = line;
			m++;
		}
	}
	} /* decl of cl */
	{
		register int i;
	/*
	 * Check for duplicate labels
	 */
	for (i = 0; i < m; i++)
		for (j = 0; j < m; j++)
			if (ctab[i].clong == ctab[j].clong) {
				if (i == j)
					continue;
				if (j < i)
					break;
				error("Multiply defined label in case, lines %d and %d", (char *) ctab[i].cline, (char *) ctab[j].cline);
			}
	}
	/*
	 * Put out case operator and
	 * leave space for the
	 * branch table
	 */
	if (p != NLNIL) {
		(void) put(2, O_CASE1OP + (w >> 1), n);
		brtab0 = lc;
		brtab = ((short *) brtab0);
		putspace(n * 2);
		(void) put(1, O_CASEBEG);
		{
		    int i;
		for (i=0; i<m; i++)
			if (w <= 2)
				(void) put(2 ,O_CASE1 + (w >> 1), (int)ctab[i].clong);
			else
				(void) put(2 ,O_CASE4, ctab[i].clong);
		}
		(void) put(1, O_CASEEND);
	}
	csend = getlab();
	(void) put(2, O_TRA, csend);
	/*
	 * Free the case
	 * table space.
	 */
	free((char *) ctab);
	/*
	 * Generate code for each
	 * statement. Patch branch
	 * table to beginning of each
	 * statement and follow each
	 * statement with a branch back
	 * to the TRA above.
	 */
	{
	    register struct tnode *cl;
	nr = TRUE;
	for (cl = rnode->stmnt_list; cl != TR_NIL;
			cl = cl->list_node.next) {
		cs = cl->list_node.list;
		if (cs == TR_NIL)
			continue;
		if (p != NLNIL)
			for (cs = cs->c_stmnt.const_list; cs != TR_NIL;
				cs =  cs->list_node.next) {
#ifdef ADDR16
				patchfil(((char *) (brtab - 1)),
					(long)(lc - brtab0), 1);
#endif ADDR16
#ifdef ADDR32
				
				patchfil( ((unsigned long) (brtab - 1)),
					(long)(lc - brtab0), 1);
#endif ADDR32
				brtab++;
			}
		cs = cl->list_node.list;
		putcnt();
		level++;
		statement(cs->c_stmnt.stmnt);
		nr = (bool)(noreach && nr);
		noreach = FALSE;
		(void) put(2, O_TRA, csend);
		level--;
		if (gotos[cbn])
			ungoto();
	}
	} /* decl of cl */
	/*
	 * Patch the termination branch
	 */
#ifdef ADDR16
	patch((char *) csend);
#endif ADDR16
#ifdef ADDR32
	patch((unsigned long) csend);
#endif ADDR32
	noreach = nr;
	if (goc != gocnt)
		putcnt();
}
#endif OBJ
