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
static char sccsid[] = "@(#)symtab.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * Symbol table implementation.
 */

#include "defs.h"
#include "symtab.h"
#include "sym.h"
#include "sym/classes.h"
#include "sym/sym.rep"

/*
 * The symbol table structure is currently assumes no deletions.
 */

#define MAXHASHSIZE 1009    /* largest allowable hash table */

struct symtab {
	int	size;
	int	hshsize;
	SYM	**symhsh;
	SYM	*symarray;
	int	symindex;
};

/*
 * Macro to hash a string.
 *
 * The hash value is returned through the "h" parameter which should
 * an unsigned integer.  The other parameters are the symbol table, "st",
 * and a pointer to the string to be hashed, "name".
 */

#define hash(h, st, name) { \
    register const char *cp; \
    h = 0; \
    for (cp = name; *cp != '\0'; cp++) { \
	h = (h << 1) | (*cp); \
    } \
    h %= st->hshsize; \
}

LOCAL SYM *search(SYM *t, SYM *first, SYM *last);

SYMTAB *symtab = 0;


/*
 * To create a symbol table, we allocate space for the symbols and
 * for a hash table that's twice as big (+1 to make it odd).
 */
SYMTAB *
st_creat(int size)
{
	register SYMTAB *st;
	register int i;

	st = alloc(1, SYMTAB);
	st->size = size;
	st->hshsize = 2*size + 1;
	if (st->hshsize > MAXHASHSIZE) {
		st->hshsize = MAXHASHSIZE;
	}
	st->symhsh = alloc(st->hshsize, SYM *);
	st->symarray = alloc(st->size, SYM);
	st->symindex = 0;
	for (i = 0; i < st->hshsize; i++) {
		st->symhsh[i] = NIL;
	}
	return(st);
}

void
st_destroy(SYMTAB *st)
{
	dispose(st->symhsh);
	dispose(st->symarray);
	dispose(st);
}

/*
 * insert a symbol into a table
 */

SYM *
st_insert(register SYMTAB *st, const char *name)
{
	register SYM *s;
	register unsigned int h;
	static SYM zerosym;

	if (st == NIL) {
		panic("tried to insert into NIL table");
	}
	if (st->symindex >= st->size) {
		panic("too many symbols");
	}
	if (name == NIL)
		h = 0; /*APY*/
	else {
		hash(h, st, name);
	}
	s = &(st->symarray[st->symindex++]);
	*s = zerosym;
	s->symbol = name;
	s->next_sym = st->symhsh[h];
	st->symhsh[h] = s;
	return(s);
}

/*
 * symbol lookup
 */

SYM *
st_lookup(SYMTAB *st, const char *name)
{
	register SYM *s;
	register unsigned int h;

	if (st == NIL) {
		panic("tried to lookup in NIL table");
	}
	hash(h, st, name);
	for (s = st->symhsh[h]; s != NIL; s = s->next_sym) {
		if (strcmp(s->symbol, name) == 0) {
			break;
		}
	}
	return(s);
}

/*
 * Dump out all the variables associated with the given
 * procedure, function, or program at the given recursive level.
 *
 * This is quite inefficient.  We traverse the entire symbol table
 * each time we're called.  The assumption is that this routine
 * won't be called frequently enough to merit improved performance.
 */
void
dumpvars(SYM *f, FRAME *frame)
{
	register SYM *s;
	SYM *first, *last;

	first = symtab->symarray;
	last = first + symtab->symindex - 1;
	for (s = first; s <= last; s++) {
		if (should_print(s, f)) {
		printv(s, frame);
		putchar('\n');
		}
	}
}

/*
 * Create an alias for a command.
 *
 * We put it into the given table with block 1, which is how it
 * is distinguished for printing purposes.
 */
void
enter_alias(SYMTAB *table, char *new, char *old)
{
	SYM *s, *t;

	if ((s = st_lookup(table, old)) == NIL) {
		error("%s is not a known command", old);
	}
	if (st_lookup(table, new) != NIL) {
		error("cannot alias command names");
	}
	make_keyword(table, new, s->symvalue.token.toknum);
	t = st_insert(table, new);
	t->blkno = 1;
	t->symvalue.token.toknum = s->symvalue.token.toknum;
	t->type = s;
}

/*
 * Print out the currently active aliases.
 * The kludge is that the type pointer for an alias points to the
 * symbol it is aliased to.
 */
void
print_alias(SYMTAB *table, char *name)
{
	SYM *s;
	SYM *first, *last;

	if (name != NIL) {
		s = st_lookup(table, name);
		if (s == NIL) {
		error("\"%s\" is not an alias", name);
		}
		printf("%s\n", s->type->symbol);
	} else {
		first = table->symarray;
		last = first + table->symindex - 1;
		for (s = first; s <= last; s++) {
		if (s->blkno == 1) {
			printf("%s\t%s\n", s->symbol, s->type->symbol);
		}
		}
	}
}

/*
 * Find a named type that points to t; return NIL if there is none.
 * This is necessary because of the way pi keeps symbols.
 */

#define NSYMS_BACK      20      /* size of local context to try */

LOCAL SYM *search(SYM *t, SYM *first, SYM *last);

SYM *
findtype(SYM *t)
{
	SYM *s;
	SYM *first, *last;
	SYM *lower;

	first = symtab->symarray;
	last = first + symtab->symindex - 1;
	if ((lower = t - NSYMS_BACK) < first) {
		lower = first;
	}
	if ((s = search(t, lower, last)) == NIL) {
		s = search(t, first, last);
	}
	return(s);
}

/*
 * Search the symbol table from first to last, looking for a
 * named type pointing to the given type symbol.
 */

LOCAL SYM *
search(SYM *t, register SYM *first, register SYM *last)
{
	register SYM *s;

	for (s = first; s <= last; s++) {
		if (s->class == TYPE && s->type == t && s->symbol != NIL) {
		return(s);
		}
	}
	return(NIL);
}
