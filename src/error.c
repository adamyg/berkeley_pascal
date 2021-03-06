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

#if !defined(lint) && defined(sccs)
static char sccsid[] = "@(#)error.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

#include <whoami.h>
#include <0.h>
#ifndef PI1
#include <tree_ty.h>            /* must be included for yy.h */
#include <yy.h>
#endif

extern char	errpfx  = 'E';
extern int	yyline;


/*
 * Panic is called when impossible
 * (supposedly, anyways) situations
 * are encountered.
 * Panic messages should be short
 * as they do not go to the message
 * file.
 */
void
panic(s)
	const char *s;
{
#ifdef DEBUG
#ifdef PI1
	printf("Snark (%s) line=%d\n", s, line);
	abort();
#else
	printf("Snark (%s) line=%d, yyline=%d\n", s, line, yyline);
	abort();				/* die horribly */
#endif
#endif

#ifdef PXP
	Perror("Snark in pxp", s);
#elif defined(PI1)
	Perror("Snark in pi1", s);
#else
	Perror("Snark in pi", s);
#endif
	pexit(DIED);
}


/*
 * Error is called for
 * semantic errors and
 * prints the error and
 * a line number.
 */
void
verror(const char *s, va_list ap)
{
#ifdef EXSTRINGS
	char errbuf[256];			/* was extern. why? ...pbk */
#endif

	if (errpfx == 'w' && opt('w') != 0) {
		errpfx = 'E';
		return;
	}

#ifdef EXSTRINGS
	geterr(s, errbuf);
	s = errbuf;
#endif

	if (line < 0)
		line = -line;

#ifdef PI
	Enocascade = FALSE;
	if (opt('l'))
		yyoutline();
#else
	yySsync();
#endif
	yysetfile(filename);

#ifdef PI
	if (errpfx == ' ') {
		register int i;

		printf("  ");
		for (i = line; i >= 10; i /= 10)
			pchr(' ');
		printf("... ");
	} else if (Enoline)
		printf("  %c - ", errpfx);
	else
		printf("%c %d - ", errpfx, line);
	vprintf(s, ap);
#else
	fprintf(stderr, "%c %d - ", errpfx, line);
	vfprintf(stderr, s, ap);
#endif

	if (errpfx == 'E')
#ifdef PI
		eflg = TRUE, codeoff();
#else
		eflg = TRUE;
#endif

	errpfx = 'E';
#ifdef PI
	if (Eholdnl)
		Eholdnl = FALSE;
	else
		pchr( '\n' );
#else
	putc('\n', stderr);              
#endif
}


void
error(const char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	verror(s, ap);
}


void
cerror(const char *s, ...)
{
	va_list ap;

	va_start(ap, s);
#ifdef PI
	if (Enocascade)
		return;
#endif
	setpfx(' ');
	verror(s, ap);
}



#ifdef PI1
void
derror(const char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	if (!holdderr)
		verror(s, ap);
	errpfx = 'E';
}

char    *lastname, printed, hadsome;

    /*
     *  this yysetfile for PI1 only.
     *  the real yysetfile is in yyput.c
     */
void
yysetfile(name)
	char *name;
{
	if (lastname == name)
		return;
	printed |= 1;
	gettime( name );
	printf("%s  %s:\n", myctime(&tvec), name);
	lastname = name;
}
#endif  /*PI1*/

