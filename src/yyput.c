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
static char sccsid[] = "@(#)yyput.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

#include "whoami.h"
#include "0.h"
#include "tree.h"
#include "tree_ty.h"	/* must be included for yy.h */
#include "yy.h"

/*
 * Structure describing queued listing lines during the forward move
 * of error recovery.  These lines will be stroed by yyoutline during
 * the forward move and flushed by yyoutfl or yyflush when an
 * error occurs or a program termination.
 */
struct	B {
	int	Bmagic;
	int	Bline;
	int	Bseekp;
	char	*Bfile;
	int	Bseqid;
	struct	B *Bnext;
} *bottled;

/*
 * Filename gives the current input file, lastname is
 * the last filename we printed, and lastid is the seqid of the last line
 * we printed, to help us avoid printing
 * multiple copies of lines.
 */
extern	char *filename;
char	*lastname;
int	lastid;

char	hadsome;
char	holdbl;

/*
 * Print the current line in the input line
 * buffer or, in a forward move of the recovery, queue it for printing.
 */
yyoutline()
{
	register struct B *bp;

	if (Recovery) {
		bp = (struct B *) tree(6, T_BOTTLE, yyline, yylinpt, filename,
				yyseqid);
		if (bottled != NIL)
			bp->Bnext = bottled->Bnext, bottled->Bnext = bp;
		else
			bp->Bnext = bp;
		bottled = bp;
		return;
	}
	yyoutfl(yyseqid);
	if (yyseqid != lastid)
		yyprline(charbuf, yyline, filename, yyseqid);
}

/*
 * Flush all the bottled output.
 */
yyflush()
{

	yyoutfl(32767);
}

/*
 * Flush the listing to the sequence id toseqid
 */
yyoutfl(toseqid)
	int toseqid;
{
	register struct B *bp;

	bp = bottled;
	if (bp == NIL)
		return;
	bp = bp->Bnext;
	while (bp->Bseqid <= toseqid) {
		yygetline(bp->Bfile, bp->Bseekp, bp->Bline, bp->Bseqid);
		if (bp->Bnext == bp) {
			bottled = NIL;
			break;
		}
		bp = bp->Bnext;
		bottled->Bnext = bp;
	}
}

FILE	*yygetunit = NULL;
char	*yygetfile;

/*
 * Yysync guarantees that the line associated
 * with the current token was the last line
 * printed for a syntactic error message.
 */
yysync()
{

	yyoutfl(yyeseqid);
	if (lastid != yyeseqid)
		yygetline(yyefile, yyseekp, yyeline, yyeseqid);
}

yySsync()
{

	yyoutfl(OY.Yyeseqid);
}

/*
 * Yygetline gets a line from a file after we have
 * lost it.  The pointer efile gives the name of the file,
 * seekp its offset in the file, and eline its line number.
 * If this routine has been called before the last file
 * it worked on will be open in yygetunit, with the files
 * name being given in yygetfile.  Note that this unit must
 * be opened independently of the unit in use for normal i/o
 * to this file; if it were a dup seeks would seek both files.
 */
yygetline(efile, seekp, eline, eseqid)
	char *efile;
	int seekp, eline, eseqid;
{
	register int cnt;
	register char *bp;
	char buf[CBSIZE + 1];

	if (lastid == eseqid)
		return;
	if (eseqid == yyseqid) {
		bp = charbuf;
		yyprtd++;
	} else {
		bp = buf;
		if (efile != yygetfile) {
			if ( yygetunit != NULL )
			    (void) fclose( yygetunit );
			yygetfile = efile;
			yygetunit = fopen( yygetfile , "r" );
			if (yygetunit == 0)
oops:
				perror(yygetfile), pexit(DIED);
		} 
		if ( fseek( yygetunit , (long) seekp , 0 ) < 0)
			goto oops;
		cnt = fread( bp , sizeof( * bp ) , CBSIZE , yygetunit );
		if (cnt < 0)
			goto oops;
		bp[cnt] = 0;
	}
	yyprline(bp, eline, efile, eseqid);
}

yyretrieve()
{

	yygetline(OY.Yyefile, OY.Yyseekp, OY.Yyeline, OY.Yyeseqid);
}

/*
 * Print the line in the character buffer which has
 * line number line.  The buffer may be terminated by a new
 * line character or a null character.  We process
 * form feed directives, lines with only a form feed character, and
 * suppress numbering lines which are empty here.
 */
yyprline(buf, line, file, id)
	register char *buf;
	int line;
	char *file;
	int id;
{

	lastid = id;
	if (buf[0] == '\f' && buf[1] == '\n') {
		printf("\f\n");
		hadsome = 0;
		holdbl = 0;
		return;
	}
	if (holdbl) {
		pchr('\n');
		holdbl = 0;
	}
	if (buf[0] == '\n')
		holdbl = 1;
	else {
		yysetfile(file);
		yyprintf(buf, line);
	}
	hadsome = 1;
}

yyprintf(cp, line)
	register char *cp;
	int line;
{

	printf("%6d  ", line);
	while (*cp != 0 && *cp != '\n')
		pchr(graphic(*cp++));
	pchr('\n');
}

graphic(ch)
	register CHAR ch;
{

	switch (ch) {
		default:
			if (ch >= ' ')
				return (ch);
		case 0177:
			return ('?');
		case '\n':
		case '\t':
			return (ch);
	}
}

extern	int nopflg;

char	printed = 1;
/*
 * Set the current file name to be file,
 * printing the name, or a header on a new
 * page if required.
 * there is another yysetfile in error.c
 * this one is for PI and PXP that one is for PI1
 */
yysetfile(file)
	register char *file;
{

#ifdef PXP
	if (nopflg == 1)
		return;
#endif

	if (lastname == file)
		return;
	if (file == filename && opt('n') && (printed & 02) == 0) {
		printed |= 02;
		header();
	} else
		yyputfn(file);
	lastname = file;
}

/*
 * Put out an include file name
 * if an error occurs but the name has
 * not been printed (or if another name
 * has been printed since it has).
 */
yyputfn(cp)
	register char *cp;
{
	extern int outcol;

	if (cp == lastname && printed)
		return;
	lastname = cp;
	printed = 1;
#ifdef PXP
	if (outcol)
		pchr('\n');
#endif
	gettime( cp );
	printf("%s  %s:\n" , myctime( (int *) (&tvec) ) , cp );
	hadsome = 1;
}
