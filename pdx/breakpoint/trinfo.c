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
static char sccsid[] = "@(#)trinfo.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * Trace information management.
 *
 * The trace information is a list of variables that are being
 * traced or whose value changing should cause a stop.
 */

#include "defs.h"
#include "breakpoint.h"
#include "process.h"
#include "machine.h"
#include "sym.h"
#include "tree.h"
#include "source.h"
#include "object.h"
#include "tree/tree.rep"
#include "process/process.rep"
#include "process/pxinfo.h"

/*
 * When tracing variables we keep a copy of their most recent value
 * and compare it to the current one each time a breakpoint occurs.
 * MAXTRSIZE is the maximum size variable we allow.
 */

#define MAXTRSIZE 512

/*
 * The tracing structure is a list of information about all the
 * variables that are being traced.
 */

typedef struct trinfo {
    TRTYPE trtype;
    ADDRESS traddr;
    SYM *trblock;
    NODE *trvar;
    NODE *trcond;
    char *trvalue;
    struct trinfo *trnext;
} TRINFO;

LOCAL TRINFO *trhead;

/*
 * add a variable to be traced
 */

addvar(trtype, node, cond)
TRTYPE trtype;
NODE *node;
NODE *cond;
{
    register TRINFO *tp;

    tp = alloc(1, TRINFO);
    tp->trtype = trtype;
    tp->traddr = (ADDRESS) -1;
    tp->trblock = curfunc;
    tp->trvar = node;
    tp->trcond = cond;
    tp->trvalue = NIL;
    tp->trnext = trhead;
    trhead = tp;
}

/*
 * remove a variable from the trace list
 */

delvar(trtype, node, cond)
TRTYPE trtype;
NODE *node;
NODE *cond;
{
    register TRINFO *tp, *last;

    last = NIL;
    for (tp = trhead; tp != NIL; tp = tp->trnext) {
	if (tp->trtype == trtype &&
	    tr_equal(tp->trvar, node) &&
	    tr_equal(tp->trcond, cond)) {
	    break;
	}
    }
    if (tp == NIL) {
	trerror("can't delete term %t", node);
    }
    if (last == NIL) {
	trhead = tp->trnext;
    } else {
	last->trnext = tp->trnext;
    }
    if (tp->trvalue != NIL) {
	free(tp->trvalue);
    }
    free(tp);
}

/*
 * Print out any news about variables in the list whose
 * values have changed.
 */

prvarnews()
{
    register TRINFO *tp;
    register NODE *p;
    register int n;
    SYM *s;
    char buff[MAXTRSIZE];
    static LINENO prevline;

    for (tp = trhead; tp != NIL; tp = tp->trnext) {
	if (tp->trcond != NIL && !cond(tp->trcond)) {
	    continue;
	}
	s = curfunc;
	while (s != NIL && s != tp->trblock) {
	    s = container(s);
	}
	if (s == NIL) {
	    continue;
	}
	p = tp->trvar;
	if (tp->traddr == (ADDRESS) -1) {
	    tp->traddr = lval(p->left);
	}
	n = size(p->nodetype);
	dread(buff, tp->traddr, n);
	if (tp->trvalue == NIL) {
	    tp->trvalue = alloc(n, char);
	    mov(buff, tp->trvalue, n);
	    mov(buff, sp, n);
	    sp += n;
#ifdef tahoe
	    alignstack();
#endif
	    if (tp->trtype == TRPRINT) {
		printf("initially (at ");
		printwhere(curline, srcfilename(pc));
		printf("):\t");
		prtree(p);
		printf(" = ");
		printval(p->nodetype);
		putchar('\n');
	    }
	} else if (cmp(tp->trvalue, buff, n) != 0) {
	    mov(buff, tp->trvalue, n);
	    mov(buff, sp, n);
	    sp += n;
#ifdef tahoe
	    alignstack();
#endif
	    printf("after ");
	    printwhere(prevline, srcfilename(pc));
	    printf(":\t");
	    prtree(p);
	    printf(" = ");
	    printval(p->nodetype);
	    putchar('\n');
	    if (tp->trtype == TRSTOP) {
		isstopped = TRUE;
		curline = srcline(pc);
		printstatus();
	    }
	}
    }
    prevline = curline;
}

/*
 * Free the table.  Note that trvar and trcond fields are not freed,
 * this is because they are the same as in the breakpoint table and
 * are freed by the bpfree routine.
 */

trfree()
{
    register TRINFO *tp, *next;

    for (tp = trhead; tp != NIL; tp = next) {
	next = tp->trnext;
	if (tp->trvalue != NIL) {
	    free(tp->trvalue);
	}
	free(tp);
    }
    trhead = NIL;
}
