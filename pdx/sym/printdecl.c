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
static char sccsid[] = "@(#)printdecl.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * Print out the type of a symbol.
 */

#include "defs.h"
#include "sym.h"
#include "symtab.h"
#include "tree.h"
#include "btypes.h"
#include "classes.h"
#include "sym.rep"

printdecl(s)
SYM *s;
{
    register SYM *t;
    BOOLEAN semicolon;

    semicolon = TRUE;
    switch(s->class) {
	case CONST:
	    t = rtype(s->type);
	    if (t->class == SCAL) {
		printf("(enumeration constant, ord %ld)", s->symvalue.iconval);
	    } else {
		printf("const %s = ", s->symbol);
		if (t == t_real) {
		    printf("%g", s->symvalue.fconval);
		} else {
		    printordinal(s->symvalue.iconval, t);
		}
	    }
	    break;

	case TYPE:
	    printf("type %s = ", s->symbol);
	    printtype(s, s->type);
	    break;

	case VAR:
	    if (isparam(s)) {
		printf("(parameter) %s : ", s->symbol);
	    } else {
		printf("var %s : ", s->symbol);
	    }
	    printtype(s, s->type);
	    break;

	case REF:
	    printf("(var parameter) %s : ", s->symbol);
	    printtype(s, s->type);
	    break;

	case RANGE:
	case ARRAY:
	case RECORD:
	case VARNT:
	case PTR:
	    printtype(s, s);
	    semicolon = FALSE;
	    break;

	case FVAR:
	    printf("(function variable) %s : ", s->symbol);
	    printtype(s, s->type);
	    break;

	case FIELD:
	    printf("(field) %s : ", s->symbol);
	    printtype(s, s->type);
	    break;

	case PROC:
	    printf("procedure %s", s->symbol);
	    listparams(s);
	    break;

	case PROG:
	    printf("program %s", s->symbol);
	    t = s->chain;
	    if (t != NIL) {
		printf("(%s", t->symbol);
		for (t = t->chain; t != NIL; t = t->chain) {
		    printf(", %s", t->symbol);
		}
		printf(")");
	    }
	    break;

	case FUNC:
	    printf("function %s", s->symbol);
	    listparams(s);
	    printf(" : ");
	    printtype(s, s->type);
	    break;

	default:
	    error("class %s in printdecl", classname(s));
    }
    if (semicolon) {
	putchar(';');
    }
    putchar('\n');
}

/*
 * Recursive whiz-bang procedure to print the type portion
 * of a declaration.  Doesn't work quite right for variant records.
 *
 * The symbol associated with the type is passed to allow
 * searching for type names without getting "type blah = blah".
 */

LOCAL printtype(s, t)
SYM *s;
SYM *t;
{
    register SYM *tmp;
    long r0, r1;

    tmp = findtype(t);
    if (tmp != NIL && tmp != s) {
	printf("%s", tmp->symbol);
	return;
    }
    switch(t->class) {
	case VAR:
	case CONST:
	case FUNC:
	case PROC:
	    panic("printtype: class %s", classname(t));
	    break;

	case ARRAY:
	    printf("array[");
	    tmp = t->chain;
	    for (;;) {
		printtype(tmp, tmp);
		tmp = tmp->chain;
		if (tmp == NIL) {
		    break;
		}
		printf(", ");
	    }
	    printf("] of ");
	    printtype(t, t->type);
	    break;

	case RECORD:
	    printf("record\n");
	    if (t->chain != NIL) {
		printtype(t->chain, t->chain);
	    }
	    printf("end");
	    break;

	case FIELD:
	    if (t->chain != NIL) {
		printtype(t->chain, t->chain);
	    }
	    printf("\t%s : ", t->symbol);
	    printtype(t, t->type);
	    printf(";\n");
	    break;

	case RANGE:
	    r0 = t->symvalue.rangev.lower;
	    r1 = t->symvalue.rangev.upper;
	    printordinal(r0, rtype(t->type));
	    printf("..");
	    printordinal(r1, rtype(t->type));
	    break;

	case PTR:
	    putchar('^');
	    printtype(t, t->type);
	    break;

	case TYPE:
	    if (t->symbol != NIL) {
		printf("%s", t->symbol);
	    } else {
		printtype(t, t->type);
	    }
	    break;

	case SCAL:
	    printf("(");
	    t = t->type->chain;
	    if (t != NIL) {
		printf("%s", t->symbol);
		t = t->chain;
		while (t != NIL) {
		    printf(", %s", t->symbol);
		    t = t->chain;
		}
	    } else {
		panic("empty enumeration");
	    }
	    printf(")");
	    break;

	default:
	    printf("(class %d)", t->class);
	    break;
    }
}

/*
 * List the parameters of a procedure or function.
 * No attempt is made to combine like types.
 */

listparams(s)
SYM *s;
{
    SYM *t;

    if (s->chain != NIL) {
	putchar('(');
	for (t = s->chain; t != NIL; t = t->chain) {
	    switch (t->class) {
		case REF:
		    printf("var ");
		    break;

		case FPROC:
		    printf("procedure ");
		    break;

		case FFUNC:
		    printf("function ");
		    break;

		case VAR:
		    break;

		default:
		    panic("unexpected class %d for parameter", t->class);
	    }
	    printf("%s : ", t->symbol);
	    printtype(t, t->type);
	    if (t->chain != NIL) {
		printf("; ");
	    }
	}
	putchar(')');
    }
}
