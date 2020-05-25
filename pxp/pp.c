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
static char sccsid[] = "@(#)pp.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * pxp - Pascal execution profiler
 *
 * Bill Joy UCB
 * Version 1.2 January 1979
 */

#include "0.h"

#define noprint() nopflg

int	pplev[3];	/* STAT, DECL, PRFN */
int	nopflg;

setprint()
{

	if (profile == 0) {
		if (table)
			nopflg = 1;
		else
			nopflg = 0;
		return;
	}
	nopflg = !all && nowcnt() == 0 || !opt('z');
}

printon()
{

	if (profile == 0) {
		if (table)
			nopflg = 1;
		return;
	}
	nopflg = 0;
}

printoff()
{

	nopflg = 1;
}

ppkw(s)
	register char *s;
{
	register char *cp, i;

	if (noprint())
		return;
	/*
	 * First real thing printed
	 * is always a keyword
	 * or includes an "id" (if a comment)
	 * (See ppnl below)
	 */
	hadsome = 1;
	if (underline) {
		for (cp = s; *cp; cp++)
			putchar('_');
		for (cp = s; *cp; cp++)
			putchar('\b');
	}
	printf(s);
}

ppid(s)
	register char *s;
{

	if (noprint())
		return;
	hadsome = 1;
	if (s == NIL)
		s = "{identifier}";
	printf(s);
}

ppbra(s)
	char *s;
{

	if (noprint())
		return;
	if (s != NIL)
		printf(s);
}

ppsep(s)
	char *s;
{

	if (noprint())
		return;
	printf(s);
}

ppket(s)
	char *s;
{

	if (noprint())
		return;
	if (s != NIL)
		printf(s);
}

char	killsp;

ppunspac()
{

	killsp = 1;
}

ppspac()
{

	if (killsp) {
		killsp = 0;
		return;
	}
	if (noprint())
		return;
	putchar(' ');
}

ppitem()
{

	if (noprint())
		return;
	ppnl();
	indent();
}

int	owenl, owenlb;

ppsnlb()
{

	if (nopflg)
		return;
	owenlb++;
}

ppsnl()
{

	if (nopflg)
		return;
	owenl++;
}

pppay()
{

	while (owenl || owenlb) {
		putchar('\n');
		if (owenlb) {
			putchar(' ');
			owenlb--;
		} else
			owenl--;
	}
}

ppnl()
{

	if (noprint())
		return;
	if (hadsome == 0)
		return;
	pppay();
	putchar('\n');
}

indent()
{
	register i;

	if (noprint())
		return;
	linopr();
	if (profile == 0) {
		indent1(pplev[PRFN] + pplev[DECL] + pplev[STAT]);
		return;
	}
	indent1(pplev[PRFN] + pplev[STAT]);
	switch (i = shudpcnt()) {
		case 1:
			printf("%7ld.", nowcnt());
			dashes('-');
			putchar('|');
			break;
		case 0:
		case -1:
			printf("        ");
			dashes(' ');
			putchar(i == 0 ? '|' : ' ');
			break;
	}
	indent1(pplev[DECL]);
}

dashes(c)
	char c;
{
	register i;

	for (i = unit - 1; i != 0; i--)
		putchar(c);
}

indent1(in)
	int in;
{
	register i;

	if (noprint())
		return;
	i = in;
	if (profile == 0)
		while (i >= 8) {
			putchar('\t');
			i -= 8;
		}
	while (i > 0) {
		putchar(' ');
		i--;
	}
}

linopr()
{

	if (noprint())
		return;
	if (profile) {
		if (line < 0)
			line = -line;
		printf("%6d  ", line);
	}
}

indentlab()
{

	indent1(pplev[PRFN]);
}

ppop(s)
	char *s;
{

	if (noprint())
		return;
	printf(s);
}

ppnumb(s)
	char *s;
{

	if (noprint())
		return;
	if (s == NIL)
		s = "{number}";
	printf(s);
}

ppgoin(lv)
{

	pplev[lv] += unit;
}

ppgoout(lv)
{

	pplev[lv] -= unit;
	if (pplev[lv] < 0)
		panic("pplev");
}

ppstr(s)
	char *s;
{
	register char *cp;

	if (noprint())
		return;
	if (s == NIL) {
		printf("{string}");
		return;
	}
	putchar('\'');
	cp = s;
	while (*cp) {
		putchar(*cp);
		if (*cp == '\'')
			putchar('\'');
		cp++;
	}
	putchar('\'');
}

pplab(s)
	char *s;
{

	if (noprint())
		return;
	if (s == NIL)
		s = "{integer label}";
	printf(s);
}

int	outcol;


putchar(c)
	char c;
{

	putc(c, stdout);
	if (ferror(stdout))
		outerr();
	switch (c) {
		case '\n':
			outcol = 0;
			flush();
			break;
		case '\t':
			outcol += 8;
			outcol &= ~07;
			break;
		case '\b':
			if (outcol)
				outcol--;
			break;
		default:
			outcol++;
		case '\f':
			break;
	}
}

flush()
{

	fflush(stdout);
	if (ferror(stdout))
		outerr();
}

pptab()
{
	register int i;

	if (noprint())
		return;
	i = pplev[PRFN] + profile ? 44 + unit : 28;
/*
	if (outcol > i + 8) {
		ppnl();
		i += 8;
	}
*/
	do
		putchar('\t');
	while (outcol < i);
}

outerr()
{

	perror(stdoutn);
	pexit(DIED);
}
