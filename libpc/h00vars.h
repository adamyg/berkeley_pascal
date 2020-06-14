/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
/*-
 * Copyright (c) 1979 The Regents of the University of California.
 * All rights reserved.
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
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
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
 *
 *      @(#)h00vars.h   1.12 (Berkeley) 4/9/90
 */

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "whoami.h"

#include "../src/common.h"			/* common definitions */
#include "../px/px.h"				/* px interface */
#include "libpc.h"				/* public interface */

#if defined(_MSC_VER)
#include <process.h>
#include <io.h>

#define unlink(__x)	_unlink(__x)
#define getpid()	_getpid()

#elif defined(__WATCOMC__)
#include <io.h>
#define unlink(__x)	_unlink(__x)

#endif

#if (0)
#define PXPFILE		"pmon.out"
#define	BITSPERBYTE	8
#define	BITSPERLONG	(BITSPERBYTE * sizeof(long))

#define LG2BITSBYTE	03
#define MSKBITSBYTE	07
#define LG2BITSLONG	05
#define MSKBITSLONG	037

#define HZ		60
#define	MAXLVL		20
#define MAXERRS		75
#define NAMSIZ		76
#define MAXFILES	32
#define PREDEF		2

#ifdef ADDR32
#ifndef tahoe
#define STDLVL		((struct iorec *)(0x7ffffff1))
#define GLVL		((struct iorec *)(0x7ffffff0))
#else tahoe
#define STDLVL		((struct iorec *)(0xbffffff1))
#define GLVL		((struct iorec *)(0xbffffff0))
#endif tahoe
#endif ADDR32
#ifdef ADDR16
#define STDLVL		((struct iorec *)(0xfff1))
#define GLVL		((struct iorec *)(0xfff0))
#endif ADDR16

#define FILNIL		((struct iorec *)(0))
#define INPUT		((struct iorec *)(&input))
#define OUTPUT		((struct iorec *)(&output))
#define ERR		((struct iorec *)(&_err))
typedef enum {FALSE, TRUE} bool;
#endif

/*
 * runtime display structure
 */
struct display {
	char		*ap;
	char		*fp;
};


/*
 * formal routine structure
 */
struct formalrtn {
	long		(*fentryaddr)();	/* formal entry point */
	long		fbn;			/* block number of function */
	struct display	fdisp[ MAXLVL ];	/* saved at first passing */
};


/*
 * program variables
 */
extern struct display	_display[MAXLVL];	/* runtime display */


/*
 * support functions
 */
extern void		FCALL(char *save, struct formalrtn *);
extern void		FRTN(struct formalrtn *, char *);
extern struct formalrtn *FSAV(long (*entryaddr)(), long, struct formalrtn *);

