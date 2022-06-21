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

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1980, 1993, The Regents of the University of California.  All rights reserved.\n"\
"@(#) Copyright (c) 2020, Adam Young.\n";
#endif /* not lint */

#if !defined(lint) && defined(sccs)
static char sccsid[] = "@(#)main.c	8.2 (Berkeley) 5/24/94";
#endif /* not lint */

#include "whoami.h"
#include "0.h"
#include "tree_ty.h"            /* must be included for yy.h */
#include "yy.h"
#include <signal.h>
#include "objfmt.h"
#include "config.h"

/*
 * This version of pi has been in use at Berkeley since May 1977
 * and is very stable. Please report any problems with the error
 * recovery to the second author at the address given in the file
 * READ_ME.  The second author takes full responsibility for any bugs
 * in the syntactic error recovery.
 */
char	piusage[]	= "pi [ -blnpstuw ] [ -i file ... ] name.p";
char	*usageis	= piusage;

#ifdef OBJ
char	pixusage[]	= "pix [ -blnpstuw ] [ -i file ... ] name.p [ arg ... ]";
char	*obj		= "obj";
#endif /*OBJ*/

#ifdef PC
char	*pcname 	= "pc.pc0";
char	pcusage[]	= "pc [ options ] [ -o file ] [ -i file ... ] name.p";
FILE	*pcstream	= NULL;
#endif /*PC*/

#ifdef PTREE
char	*pTreeName = "pi.pTree";
#endif /*PTREE*/

void onintr(int);

FILE *ibuf		= NULL;

/*
 * these are made real variables
 * so they can be changed
 * if you are compiling on a smaller machine
 */
double	XMAXINT		=  2147483647.;
double	XMININT		= -2147483648.;

/*
 * Main program for pi. 
 *
 * Process options, then call yymain to do all the real work.
 */
void
main(int argc, char *argv[])
{
	register char *cp;
	register int c;
#if defined(unix)
	int i;
#endif

#ifndef lint
	(void) copyright;
#endif

	if (argv[0][0] == 'a')
		err_file += err_pathlen, how_file += how_pathlen;

#ifdef OBJ
	ofil = -1;
	if (argv[0][0] == '-' && argv[0][1] == 'o') {
		const int how_len = strlen(how_file);
		char *new_how = malloc(how_len + 2);

		obj = &argv[0][2];
		usageis = pixusage;
		(void) memcpy(new_how, how_file, how_len);
		new_how[how_len] = 'x';
		new_how[how_len+1] = 0;
		how_file = new_how;
		ofil = 3;
	}
#endif /*OBJ*/

	argv++, argc--;
	if (argc == 0) {
#if !defined(unix)
		goto usage;
#else
		i = fork();
		if (i == -1)
			goto usage;
		if (i == 0) {
			execl("/bin/cat", "cat", how_file, 0);
			goto usage;
		}
		while (wait(&i) != -1)
			continue;
		pexit(NOSTART);
#endif
	}

#ifdef OBJ
		opt('p') = opt('t') = opt('b') = 1;
	#if defined(vax) || defined(tahoe)
		/* pdx is currently supported on the vax and the tahoe */
		opt('g') = 1;
	#endif
		while (argc > 0) {
			cp = argv[0];
			if (*cp++ != '-')
				break;
			while (c = *cp++) switch (c) {
		#ifdef DEBUG
				case 'k':	/*  code dump */
				case 'r':           
				case 'y':	/* name list dump */
				case 'x':	/* hex dump */
				case 'R':	/* record dump */
					togopt(c);
					continue;
				case 'K':
					yycosts();
					pexit(NOSTART);
				case 'A':
					testtrace = TRUE;
				case 'F':
					fulltrace = TRUE;
				case 'E':
					errtrace = TRUE;
					opt('r')++;
					continue;
				case 'U':
					yyunique = FALSE;
					continue;
		#endif
				case 'b':	/* block buffer */
					opt('b') = 2;
					continue;
				case 'i':	/* enable listing */
					pflist = argv + 1;
					pflstc = 0;
					while (argc > 1) {
						if (dotted(argv[1], 'p'))
							break;
						pflstc++, argc--, argv++;
					}
					if (pflstc == 0)
						goto usage;
					continue;
				case 'g':	/* debug */
				case 'l':	/* program listing */
				case 'n':	/* page banner */
				case 'p':	/* suppress post-mortem */
				case 's':	/* standard pascal */
				case 't':	/* suppress test */
				case 'u':	/* card image mode (limit line to 72) */
				case 'w':	/* suppress warnings */
					togopt(c);
					continue;
				case 'o':	/* output file */
					if (argc < 2)
						goto usage;
					argv++;
					argc--;
					obj = argv[0];
					break;
				case 'z':	/* profile */
					monflg = TRUE;
					continue;
				case 'L':
					togopt('L');
					continue;
				default:
usage:
					Perror( "Usage", usageis);
					pexit(NOSTART);
			}
			argc--, argv++;
		}
#endif /*OBJ*/
#ifdef PC
		opt( 'b' ) = 1;
		opt( 'g' ) = 0;
		opt( 't' ) = 0;
		opt( 'p' ) = 0;
		usageis = pcusage;
		while ( argc > 0 ) {
			cp = argv[0];
			if ( *cp++ != '-' ) {
			break;
			}
			c = *cp++;
			switch( c ) {
	#ifdef DEBUG
			case 'k':
			case 'r':
			case 'y':
				togopt(c);
				break;
			case 'K':
				yycosts();
				pexit(NOSTART);
			case 'A':
				testtrace = TRUE;
				/* and fall through */
			case 'F':
				fulltrace = TRUE;
				/* and fall through */
			case 'E':
				errtrace = TRUE;
				opt('r')++;
				break;
			case 'U':
				yyunique = FALSE;
				break;
	#endif /*DEBUG*/
			case 'b':
				opt('b') = 2;
				break;
			case 'i':
				pflist = argv + 1;
				pflstc = 0;
				while (argc > 1) {
					if (dotted(argv[1], 'p'))
						break;
					pflstc++, argc--, argv++;
				}
				if (pflstc == 0)
					goto usage;
				break;
				/*
				 * output file for the first pass
				 */
			case 'o':
				if ( argc < 2 ) {
					goto usage;
				}
				argv++;
				argc--;
				pcname = argv[0];
				break;      
			case 'J':
				togopt( 'J' );
				break;
			case 'C':
				/*
				 * since -t is an ld switch, use -C
				 * to turn on tests
				 */
				togopt( 't' );
				break;
			case 'g':
				/*
				 * sdb symbol table
				 */
				togopt( 'g' );
				break;
			case 'l':
			case 's':
			case 'u':
			case 'w':
				togopt(c);
				break;
			case 'p':
					/*
					*      -p on the command line means profile
					*/
				profflag = TRUE;
				break;
			case 'z':
				monflg = TRUE;
				break;
			case 'L':
				togopt( 'L' );
				break;
			default:
usage:				Perror( "Usage", usageis);
				pexit(NOSTART);
			}
			argc--;
			argv++;
		}
#endif /*PC*/
	if (argc != 1)
		goto usage;

#ifdef OBJ
		if (ofil == -1) {
			ofil = open( obj, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0775 );
			if (ofil < 0) {
				perror(obj);
				pexit(NOSTART);
			}
		}
#endif
#ifdef EXSTRINGS
	efil = open( err_file, O_RDONLY | O_BINARY );
	if ( efil < 0 )
		perror(err_file), pexit(NOSTART);
#endif

	filename = argv[0];
	if (!dotted(filename, 'p')) {
		Perror(filename, "Name must end in '.p'");
		pexit(NOSTART);
	}
	close(0);
	if ( ( ibuf = fopen( filename, "r" ) ) == NULL )
		perror(filename), pexit(NOSTART);
	ibp = ibuf;

#ifdef PC
		if ( ( pcstream = fopen( pcname , "wb" ) ) == NULL ) {
			perror( pcname );
			pexit( NOSTART );
		}
		stabsource( filename, TRUE );
#endif /*PC*/

#ifdef PTREE
#define MAXPPAGES	16
		if ( ! pCreate( pTreeName , MAXPPAGES ) ) {
			perror( pTreeName );
			pexit( NOSTART );
		}
#endif /*PTREE*/

	if ( signal( SIGINT , SIG_IGN ) != SIG_IGN )
		(void) signal( SIGINT , onintr );
	if (opt('l')) {
		opt('n')++;
		yysetfile(filename);
		opt('n')--;
	}

	yymain();
	/* No return */
}


void
pchr(c)
	int c;
{
	putc( c, stdout );
}

#ifdef PC
static const char ugh[] = "Fatal error in pc\n";
#endif
#ifdef OBJ
static const char ugh[] = "Fatal error in pi\n";
#endif


/*
 * Exit from the Pascal system.
 * We throw in an ungraceful termination message if c > 1 indicating 
 * a severe error such as running out of memory or an internal inconsistency.
 */
void
pexit(c)
	int c;
{
	if (opt('l') && c != DIED && c != NOSTART)
		while (xgetline() != -1)
			continue;
	yyflush();
	switch (c) {
		case DIED:
			write(2, ugh, sizeof ugh);
		case NOSTART:
		case ERRS:
#ifdef OBJ
			if ( ofil > 0 ) {
				close(ofil);
				unlink(obj);
			}

			/*
			 * remove symbol table temp files
			 */
			removenlfile();
#endif /*OBJ*/
#ifdef PC
			if ( pcstream != NULL ) {
				fclose( pcstream );
				unlink( pcname );
			}
#endif /*PC*/
			break;
		case AOK:
#ifdef OBJ
			pflush();

			/*
			 * copy symbol table temp files to obj file
			 */
			copynlfile();
			close(ofil);
#endif /*OBJ*/
#ifdef PC
			puteof();
#endif /*PC*/
			break;
	}

	/*
	 *      this to gather statistics on programs being compiled
	 *      taken 20 june 79        ... peter
	 *
	 *  if (fork() == 0) {
	 *      char *cp = "-0";
	 *      cp[1] += c;
	 *      execl("/usr/lib/gather", "gather", cp, filename, 0);
	 *      exit(1);
	 *  }
	 */
#ifdef PTREE
	    pFinish();
#endif
	exit(c);
}

void
onintr(int sig)
{

	(void) sig;
	(void) signal( SIGINT , SIG_IGN );
	pexit(NOSTART);
}


/*
 * Get an error message from the error message file
 */
#ifdef  EXSTRINGS
void
geterr(seekpt, buf)
	int seekpt;
	char *buf;
{

	if (lseek(efil, (long) seekpt, 0) == -1 ||
			read(efil, buf, 256) <= 0)
		perror(err_file), pexit(DIED);
}
#endif


void
header()
{
	static char anyheaders;

	gettime( filename );
	if (anyheaders && opt('n'))
		putc( '\f' , stdout );
	anyheaders++;
#ifdef OBJ
	printf("Berkeley Pascal PI -- Version %s\n\n%s  %s\n\n",
		version, myctime(&tvec), filename);
#endif /*OBJ*/
#ifdef PC
	printf("Berkeley Pascal PC -- Version %s\n\n%s  %s\n\n",
		version, myctime(&tvec), filename);
#endif /*PC*/
}
