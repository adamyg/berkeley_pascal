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
static char sccsid[] = "@(#)yymain.c	8.2 (Berkeley) 5/24/94";
#endif /* not lint */

#include <whoami.h>
#include <0.h>
#include <tree_ty.h>	/* must be included for yy.h */
#include <yy.h>
#if defined(unix)
#include <a.out.h>
#else
#include <a_out.h>
#endif
#include <objfmt.h>
#include <signal.h>
#include <config.h>


#ifdef PXP
static void		copyfile();
#endif
#ifdef PI
#ifdef OBJ
static void		magic();
static void		magic2();
#endif
#endif


/*
 * Yymain initializes each of the utility clusters and then starts the 
 * processing by calling yyparse.
 */
void
yymain()
{
#ifdef OBJ
	/*
	 * initialize symbol table temp files
	 */
	startnlfile();
#endif
	/*
	 * Initialize the scanner
	 */
#ifdef PXP
	if (bracket == 0) {
#endif
		if (xgetline() == -1) {
			Perror(filename, "No lines in file");
			pexit(NOSTART);
		}
#ifdef PXP
	} else
		yyline = 0;
#endif

#ifdef PI
#ifdef OBJ
	magic();
#endif
#endif

	line = 1;
	errpfx = 'E';
	/*
	 * Initialize the clusters
	 *
	initstring();
	 */
	inithash();
	inittree();
#ifdef PI
	initnl();
#endif

	/*
	 * Process the input
	 */
	yyparse();


#ifdef PI
#ifdef OBJ
	/*
	 * save outermost block of namelist
	 */
	if ( CGENNING ) {
		savenl(NLNIL, 0);
		magic2();
	} else {
		fprintf(stderr, "File not written because of errors\n");
	}
#endif
#ifdef DEBUG
	dumpnl(NLNIL, NULL);
#endif
#endif

#ifdef PXP
	prttab();
	if (onefile) {
		extern int outcol;

		if (outcol)
			pputchar('\n');
		flush();
		if (eflg) {
			fprintf(stderr, "File not rewritten because of errors\n");
			pexit(ERRS);
		}
#ifdef SIGHUP
		(void) signal(SIGHUP, SIG_IGN);
#endif
#ifdef SIGINT
		(void) signal(SIGINT, SIG_IGN);
#endif
		copyfile();
	}
#endif /*PXP*/
	pexit(eflg ? ERRS : AOK);
}


#ifdef PXP
static void
copyfile()
{
	register int c;

	if (freopen(stdoutn, "r", stdin) == NULL) {
		perror(stdoutn);
		pexit(ERRS);
	}
	if (freopen(firstname, "w", stdout) == NULL) {
		perror(firstname);
		pexit(ERRS);
	}
	while ((c = getchar()) > 0)
		pputchar((char)c);
	if (ferror(stdout))
		perror("stdout");
}
#endif /*PXP*/


#ifdef PI
#ifdef OBJ
static struct exec magichdr = {0};

static void
magic()
{
#if !defined(unix)
	register int i;

	magichdr.a_magic   = 0x01020304; /*FIXME*/
	magichdr.a_text    = 512;
	magichdr.a_data    = 512;
	for (i = 0; i < HEADER_BYTES / sizeof(short); i++)
		word(0xffff);
#else
	short           buf[HEADER_BYTES/sizeof(short)];
	unsigned        *ubuf = (unsigned *) buf;
	register int    hf, i;

	hf = open(px_header, O_BINARY);
	if (hf >= 0) {
	    if (read(hf, (char *) buf, HEADER_BYTES) > sizeof(struct exec)) {
			magichdr.a_magic  = ubuf[0];
			magichdr.a_text   = ubuf[1];
			magichdr.a_data   = ubuf[2];
			magichdr.a_bss    = ubuf[3];
			magichdr.a_syms   = ubuf[4];
			magichdr.a_entry  = ubuf[5];
			magichdr.a_trsize = ubuf[6];
			magichdr.a_drsize = ubuf[7];
			for (i = 0; i < HEADER_BYTES / sizeof(short); i++)
				word(buf[i]);
		}
		(void) close(hf);
	}
#endif
}
#endif /*OBJ*/


#ifdef OBJ
static void
magic2()
{
	struct pxhdr pxhd = {0};
	time_t now;

#if !defined(unix)
	if (magichdr.a_magic != 0x01020304)
		panic( "magic2" );
#else
	if (magichdr.a_magic != 0407)
		panic( "magic2" );
#endif
	pflush();
	magichdr.a_data = (unsigned) lc - magichdr.a_text;
	magichdr.a_data -= sizeof(struct exec);
	pxhd.objsize    = ((unsigned) lc) - HEADER_BYTES;
	pxhd.symtabsize = nlhdrsize();
	magichdr.a_data += pxhd.symtabsize;

//	(void) time((time_t *) (&pxhd.maketime));
	now = time(NULL);
	if (now > 0x7ffffffff) { /*FIXME*/
		fprintf(stderr, "Timestamp overflow; rework required\n");
		pexit(ERRS);
	}
	pxhd.maketime   = (long) now;

	pxhd.magicnum   = MAGICNUM;
	(void) lseek(ofil, 0l, 0);
#ifdef DEBUG
	if (opt('x'))
		dumphex(tell(ofil), &magichdr, sizeof(struct exec));
#endif
	write(ofil, (char *) (&magichdr), sizeof(struct exec));
	if (lseek(ofil, (off_t)(HEADER_BYTES - sizeof(pxhd)), 0) == -1) {
		fprintf(stderr, "magic2: lseek\n");
		pexit(ERRS);
	}
#ifdef DEBUG
	if (opt('x'))
		dumphex(tell(ofil), &pxhd, sizeof (pxhd));
#endif
	write(ofil, (char *) (&pxhd), sizeof (pxhd));
}
#endif /*OBJ*/
#endif /*PI*/

