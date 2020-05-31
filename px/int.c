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

#if !defined(lint) && defined(SCCS)
const char px_copyright[] =
"@(#) Copyright (c) 1980, 1993\n\
	The Regents of the University of California.  All rights reserved.\n"\
"@(#) Copyright (c) 2020, Adam Young.\n";
#endif /* not lint */

#if !defined(lint) && defined(SCCS)
static char sccsid[] = "@(#)int.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

/*
 * px - interpreter for Berkeley Pascal
 * Version 3.0 Winter 1979
 *
 * Original version for the PDP 11/70 authored by:
 * Bill Joy, Charles Haley, Ken Thompson
 *
 * Rewritten for VAX 11/780 by Kirk McKusick
 */

#if defined(_MSC_VER)
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define  _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable:4996)
#endif

#include "whoami.h"
#include <common.h>
#include <setjmp.h>
#include <libpc.h>
#include <objfmt.h>
#include "pxvars.h"
#include "px.h"

static void complete(int code);
static int compare(const char *, const char *);


/*
 * New stuff for pdx
 */
static jmp_buf _exitjmp;

#if defined(PDX_TRAP)
extern char *end;
extern loopaddr();
extern union progcntr pdx_pc; /* address of interpreter program cntr */
static void inittrap(union disply *dispaddr, struct disp *dpaddr, char *endaddr, union progcntr *pcaddr, char **loopaddrp);
#endif

int
px_main(int argc, const char **argv)
{
	register char *objprog;
	const char *file, *name;
	register long bytesread, bytestoread, block;
	register FILE *prog;
	struct pxhdr pxhd;
	int ret;
#define pipe		3

	/*
	 * Initialize everything
	 */
	_pcargc = argc;
	_pcargv = argv;
	_nodump = FALSE;

	/*
	 * Determine how PX was invoked, and how to process the program
	 */
	file = _pcargv[1];
	if (compare(_pcargv[0], "pdx")) { /* debugger */
		_mode = PDX;
		_pcargv += 2; _pcargc -= 2;
		name = _pcargv[0];
	} else if (compare(_pcargv[0], "pix")) { /* compile and run */
		_mode = PIX;
		_pcargv++; _pcargc--;
		name = _pcargv[0];
	} else if (compare(_pcargv[0], "pipe")) { /* px_header */
		_mode = PIPE;
		file = "PIPE";
		_pcargv++; _pcargc--;
		name = _pcargv[0];
	} else if (compare(_pcargv[0], "px")) { /* run */
		_mode = PX;
		if (_pcargc <= 1)
			file = "obj";
		else {
			_pcargv++; _pcargc--; /* strip 'px' */
		}
		name = file;
	} else { /* embedded */
		_mode = PX;
		if (_pcargc <= 1)
			file = "obj";
		name = file;
	}

	/*
	 * kludge to check for old style objs.
	 */
	if (_mode == PX && !strcmp(file, "-")) {
		fprintf(stderr, "%s is obsolete and must be recompiled\n",
		    _pcargv[0]);
		return(1);
	}

	/*
	 * Process program header information
	 */
	if (_mode == PIPE) {
		read(pipe, &pxhd, sizeof(struct pxhdr));
	} else {
		prog = fopen(file, "rb");
		if (prog == (FILE *)NULL) {
			perror(file);
			return(1);
		}
		fread(&pxhd, sizeof(struct pxhdr), 1, prog);
		if (pxhd.magicnum != MAGICNUM) {
			fseek(prog, (long)(HEADER_BYTES-sizeof(struct pxhdr)), 0);
			fread(&pxhd, sizeof(struct pxhdr), 1, prog);
		}
	}
	if (pxhd.magicnum != MAGICNUM) {
		fprintf(stderr, "%s is not a Pascal interpreter file\n",name);
		fclose(prog);
		return(1);
	}
	if (pxhd.maketime < _createtime) {
		fprintf(stderr, "%s is obsolete and must be recompiled\n",name);
		fclose(prog);
		return(1);
	}

	/*
	 * Load program into memory
	 */
	objprog = malloc((int)pxhd.objsize);
	if (objprog == (char *)NULL) {
		fprintf(stderr,"Memory allocation error occurred while loading %s\n",file);
		fclose(prog);
		return(1);
	}
	if (_mode == PIPE) {
		bytestoread = pxhd.objsize;
		bytesread = 0;
		do {
			block = read(pipe,objprog+bytesread,bytestoread);
			if (block > 0) {
				bytesread += block;
				bytestoread -= block;
			}
		} while (block > 0);
	} else {
		bytesread = fread(objprog, 1, (int)pxhd.objsize, prog);
		fclose(prog);
	}
	if (bytesread != pxhd.objsize) {
		fprintf(stderr, "Read error occurred while loading %s\n",file);
		return(1);
	}
	if (_mode == PIX)
		fputs("Execution begins...\n",stderr);

	/*
	 * set interpreter to catch expected signals and begin interpretation
	 */
	signal(SIGILL,px_syserr);
#ifdef SIGBUS
	signal(SIGBUS,px_syserr);
#endif
#ifdef SIGSYS
	signal(SIGSYS,px_syserr);
#endif
	if (signal(SIGINT,SIG_IGN) != SIG_IGN)
		signal(SIGINT,px_intr);
	signal(SIGSEGV,px_memsize);
	signal(SIGFPE,EXCEPT);
#ifdef SIGTRAP
	signal(SIGTRAP,px_liberr);
#endif
#if defined(SIGUSR2)
	signal(SIGUSR2,px_liberr);
#endif

	/*
	 * execute
	 */
#if defined(PDX_TRAP)
	if (_mode == PDX || (_mode == PIX && pxhd.symtabsize > 0)) {
		inittrap(&_display, &_dp, objprog, &pdx_pc, loopaddr);
	}
#endif

	if (0 == (ret = setjmp(_exitjmp))) {
		px_interpreter(objprog);
		complete(0);
	}

	/*
	 * release object storage
	 */
	free(objprog);

	/*
	 * reset signals, deallocate memory, and exit
	 */
	signal(SIGINT,SIG_IGN);
	signal(SIGSEGV,SIG_DFL);
	signal(SIGFPE,SIG_DFL);
#ifdef SIGTRAP
	signal(SIGTRAP,SIG_DFL);
#endif
	signal(SIGILL,SIG_DFL);
#ifdef SIGBUS
	signal(SIGBUS,SIG_DFL);
#endif
#ifdef SIGSYS
	signal(SIGSYS,SIG_DFL);
#endif
	return(ret);
}

static void
complete(int code)
{
	if (_pcpcount != 0)
		PMFLUSH(_cntrs, _rtns, _pcpcount);
	if (_mode == PIX) {
		fputs("Execution terminated",stderr);
		if (code)
			fputs(" abnormally",stderr);
		fputc('.',stderr);
		fputc('\n',stderr);
	}
	px_stats();
	PFLUSH();
}


void
px_exit(int code)
{
	complete(code);
	longjmp(_exitjmp, code ? code : 1);
}



static int
compare(const char *arg0, const char *name)
{
#if defined(unix)
	const char *p;

	if ((p = strrchr(argv, '/')) != NULL) /* strip path */
		arg0 = p + 1;
	return (strcmp(arg0, name) == 0 ? 1 : 0);

#else
	const char *p1, *p2, *d;
	int len;

	p1 = strrchr(arg0, '/'); /* strip path */
	p2 = strrchr(arg0, '\\');
	if (p1 || p2)
		arg0 = (p2 > p1 ? p2 : p1) + 1;
	else if (NULL != (p1 = strrchr(arg0, ':')))
		arg0 = p1 + 1;

	if ((d = strrchr(arg0, '.')) != NULL) /* strip extension */
		len = d - arg0;
	else len = strlen(arg0);

	return (strncmp(arg0, name, len) == 0 ? 1 : 0);
#endif
}


#if defined(PDX_TRAP)
/*
 * Generate an IOT trap to tell the debugger that the object code
 * has been read in.  Parameters are there for debugger to look at,
 * not the procedure.
 */
static void
inittrap(dispaddr, dpaddr, endaddr, pcaddr, loopaddrp)
	union disply *dispaddr;
	struct disp *dpaddr;
	char *endaddr;
	union progcntr *pcaddr;
	char **loopaddrp;
{
	kill(getpid(), SIGIOT);
}
#endif
