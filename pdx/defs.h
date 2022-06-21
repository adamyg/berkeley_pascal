#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED
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
 *
 *	@(#)defs.h	8.1 (Berkeley) 6/6/93
 */

/*
 *  Global debugger defines.
 *
 *  All files include this header.
 */

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#if defined(WIN32) || defined(_WIN32)
#define  WINDOWS_MEAN_AND_LEAN
#include <windows.h>

#undef CONST
#undef DELETE			/* yylex */
#undef IN
#undef MINCHAR			/* attribute.c */
#undef MAXCHAR
#undef MINSHORT
#undef MAXSHORT
#undef TRUE
#undef FALSE
#endif

#if defined(__WATCOMC__)
#pragma disable_message(131)  // No prototype found for function 'xxx'
#pragma disable_message(202)  // Missing return value for function 'xxx'
#endif
#if defined(_MSC_VER)
#pragma warning(disable:4996) // 'xxx': The POSIX name for this item is deprecated.
#endif

#include "..\src\common.h"	/* command definitions */

#define LOCAL			static
#ifndef NIL
#define NIL			0
#endif

/*
 *  Since C does not allow forward referencing of types,
 *  all the global types are declared here.
 */

typedef unsigned int ADDRESS;	/* object code addresses */
typedef short LINENO;		/* source file line numbers */
typedef struct sym SYM; 	/* symbol information structure */
typedef struct symtab SYMTAB;	/* symbol table */
typedef struct node NODE;	/* expression tree node */
typedef short OP;		/* tree operator */
typedef struct opinfo OPINFO;	/* tree operator information table */
typedef struct frame FRAME;	/* runtime activation record */
	
#if !defined(WIN32) && !defined(_WIN32)
typedef int BOOLEAN;
typedef unsigned int WORD;	/* machine word */
typedef unsigned char BYTE;	/* machine byte */
#endif

typedef int INTFUNC();


#ifndef PI
/*
 *  Definitions of library routines.
 */
extern const char *cmdname;	/* name of command for error messages */
extern const char *errfilename;	/* current file associated with error */
extern short errlineno; 	/* line number associated with error */

/*
 *  Definitions for doing memory allocation
 */
#define alloc(n, type)		((type *) malloc((unsigned) (n) * sizeof(type)))
#define dispose(p)		{free((char *) p); p = NIL;}


/*
 *  Macro for doing freads
 */
#define get(fp, var)		fread((char *) &(var), sizeof(var), 1, fp)


/*
 *  String definitions
 */
#define streq(s1, s2)		(strcmp(s1, s2) == 0)


/*
 *  Protypes ...
 */

/*main.c*/
extern void		quit(int r);


/*library.c*/
#if !defined(__attribute__)
#if !defined(__GNUC__) && !defined(__clang__)
#define __attribute__(__x)
#endif
#endif //__attribute__
#if !defined(__printf_attribute__)
#if !defined(_Printf_format_string_)
#define __printf_attribute__
#else
#define __printf_attribute__ _Printf_format_string_
#endif
#endif //__printf_attribute__

extern void		syserr(void);
extern int		call(const char *name, FILE *in, FILE *out, ...);
extern int		callv(const char *name, FILE *in, FILE *out, const char **argv);
extern int		backv(const char *name, FILE *in, FILE *out, const char **argv);
extern void		shell(const char * s);
extern void		pwait(int pid, int *statusp);
extern INTFUNC		*onsyserr(int n, INTFUNC *f);
extern void		error(__printf_attribute__ const char *s, ...) __attribute__ ((format (printf, 1, 2)));
extern void		errorv(__printf_attribute__ const char *s, va_list);
extern void		fatal(__printf_attribute__ const char *s, ...) __attribute__ ((format (printf, 1, 2)));
extern void		fatalv(__printf_attribute__ const char *s, va_list);
extern void		panic(__printf_attribute__ const char *s, ...) __attribute__ ((format (printf, 1, 2)));
extern void		panicv(__printf_attribute__ const char *s, va_list);
extern int		cmp(const void *s1, const void *s2, unsigned int n);
extern void		mov(const void *src, void *dest, int n);

/*start.c*/
extern void		start(const char **argv, const char *infile, const char *outfile);
extern void		endprogram(void);

/*misc.c*/
extern void		gripe(void);
extern void		help(void);
extern void		setout(const char *filename);
extern void		unsetout(void);
#endif

#endif /*DEFS_H_INCLUDED*/
