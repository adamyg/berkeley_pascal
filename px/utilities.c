/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
/*-
 * Copyright (c) 1991, 1993
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

#if !defined(lint) && defined(SCCS)
static char sccsid[] = "@(#)utilities.c	8.1 (Berkeley) 6/6/93";
#endif  /* not lint */

#include "whoami.h"
#include <common.h>
#include <libpc.h>
#include <objfmt.h>
#include "h02opcs.h"
#include "pxvars.h"
#include "px.h"

void
px_stats(void)
{
#if defined(unix)
	struct rusage ru;
#else
	clock_t ru;
#endif

#ifdef PROFILE
	register long count;
	struct cntrec {
		double	counts[NUMOPS]; 	/* instruction counts */
		long	runs;			/* number of interpreter runs */
		long	startdate;		/* date profile started */
		long	usrtime;		/* total user time consumed */
		long	systime;		/* total system time consumed */
		double	stmts;			/* number of pascal stmts executed */
        } profdata;
	FILE *datafile;
#endif /*PROFILE*/

	if (_nodump)
		return;
#if defined(unix)
	getrusage(RUSAGE_SELF, &ru);
#else
	ru = clock();
#endif

#ifdef PROFILE
	datafile = fopen(PROFFILE, "rb");
	if (datafile == NULL)
		goto skipprof;
	count = fread(&profdata,1,sizeof(profdata),datafile);
	if (count != sizeof(profdata))
		goto skipprof;
	for (count = 0;  count < NUMOPS;  count++)
		profdata.counts[count] += _profcnts[count];
	profdata.runs += 1;
	profdata.stmts += _stcnt;
	profdata.usrtime += ru.ru_utime.tv_sec;
	profdata.systime += ru.ru_stime.tv_sec;
	datafile = freopen(proffile, "wb", datafile);
	if (datafile == NULL)
		goto skipprof;
	count = fwrite(&profdata,1,sizeof(profdata),datafile);
	if (count != sizeof(profdata))
		goto skipprof;
	fclose(datafile);
skipprof:
#endif /*PROFILE*/

#if defined(unix)
	fprintf(stderr,
		"\n%1ld statements executed in %d.%02d seconds cpu time.\n",
		_stcnt, ru.ru_utime.tv_sec, ru.ru_utime.tv_usec / 10000);
#else
	fprintf(stderr,
		"\n%1ld statements executed in %d.%02d seconds cpu time.\n",
		_stcnt, ru / CLOCKS_PER_SEC, ru % CLOCKS_PER_SEC);
#endif
	fflush(stderr);
}


void
px_backtrace(type)
	const char *type;
{
	register struct dispsave *mydp;
	register struct blockmark *ap;
	register long i, linum;
	union display disp;

	fprintf(stderr, "\nProgram %s", type);
	if (_lino < 0) {
		fputs("\n\tProgram was not executed.\n",stderr);
		return;
	}
	if (_lino == 0) {
		fputs("\n\t[no backtrace].\n",stderr);
		return;
	}
	disp = _display;
	fprintf(stderr, "\n\t%s in \"", type);
	mydp = _dp;
	linum = _lino;
	for (;;) {
		ap = mydp->stp;
		i = linum - (((ap)->entry)->offset & 0177777);
		fprintf(stderr,"%s\"",(ap->entry)->name);
		if (_nodump == FALSE)
			fprintf(stderr,"+%d near line %d.",i,linum);
		fputc('\n',stderr);
		*mydp = (ap)->odisp;
		if (mydp <= &_display.frame[1]){
			_display = disp;
			return;
		}
		mydp = (ap)->dp;
		linum = (ap)->lino;
		fputs("\tCalled by \"",stderr);
	}
}


/*
 * Routines to field various types of signals
 *
 * catch a library error and generate a backtrace
 */
void
px_liberr(signum)
	int signum;
{

	(void) signum;
	px_backtrace("Library error");
	px_exit(2);
}


/*
 * catch an interrupt and generate a backtrace
 */
void
px_intr(signum)
	int signum;
{

	(void) signum;
	signal(SIGINT, px_intr);
	px_backtrace("Interrupted");
	px_exit(1);
}


/*
 * catch memory faults
 */
void
px_memsize(signum)
	int signum;
{

	(void) signum;
	signal(SIGSEGV, px_memsize);
	ERROR("Run time stack overflow\n");
}


/*
 * catch random system faults
 */
void
px_syserr(signum)
	int signum;
{

	signal(signum, px_syserr);
	ERROR("Panic: Computational error in interpreter\n");
}
