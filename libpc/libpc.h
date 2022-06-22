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
 *	@(#)libpc.h	1.9 (Berkeley) 4/9/90
 */
#ifndef LIBPC_H_INCLUDED
#define LIBPC_H_INCLUDED

#define PXPFILE 	"pmon.out"
#define PROFFILE	"pcnt.out"
#define BITSPERBYTE	8
#define BITSPERLONG	(BITSPERBYTE * sizeof(long))

#define LG2BITSBYTE	03
#define MSKBITSBYTE	07
#define LG2BITSLONG	05
#define MSKBITSLONG	037

#define HZ		100
#define MAXLVL          20
#define MAXERRS         75
#define NAMSIZ          76
#define MAXFILES        32
#define PREDEF          2

#if defined(unix)
# ifdef  ADDR32
#  ifndef tahoe
#define STDLVL		((struct iorec *)(0x7ffffff1))
#define GLVL		((struct iorec *)(0x7ffffff0))
#  else  /*tahoe*/
#define STDLVL		((struct iorec *)(0xbffffff1))
#define GLVL		((struct iorec *)(0xbffffff0))
#  endif /*tahoe*/
# endif /*ADDR32*/
#ifdef  ADDR16
# define STDLVL 	((struct iorec *)(0xfff1))
# define GLVL		((struct iorec *)(0xfff0))
#endif /*ADDR16*/
#else
#define STDLVL		((struct iorec *)(0x7ffffff1))
#define GLVL		((struct iorec *)(0x7ffffff0))
#endif

#define FILNIL		((struct iorec *)(0))
#define INPUT		((struct iorec *)(&input))
#define OUTPUT		((struct iorec *)(&output))
#define ERR		((struct iorec *)(&_err))


/*
 * The file i/o routines maintain a notion of a "current file".
 * A pointer to this file structure is kept in "curfile".
 *
 * file structures
 */
struct iorechd {
	char		*fileptr;		/* ptr to file window */
	long		lcount; 		/* number of lines printed */
	long		llimit; 		/* maximum number of text lines */
	FILE		*fbuf;			/* FILE ptr */
	struct iorec	*fchain;		/* chain to next file */
	struct iorec	*flev;			/* ptr to associated file variable */
	char		*pfname;		/* ptr to name of file */
	short		funit;			/* file status flags */
	short		fblk;			/* index into active file table */
	long		fsize;			/* size of elements in the file */
	char		fname[NAMSIZ];		/* name of associated UNIX file */
};

struct iorec {
	char		*fileptr;		/* ptr to file window */
	long		lcount; 		/* number of lines printed */
	long		llimit; 		/* maximum number of text lines */
	FILE		*fbuf;			/* FILE ptr */
	struct iorec	*fchain;		/* chain to next file */
	struct iorec	*flev;			/* ptr to associated file variable */
	char		*pfname;		/* ptr to name of file */
	short		funit;			/* file status flags */
	short		fblk;			/* index into active file table */
	long		fsize;			/* size of elements in the file */
	char		fname[NAMSIZ];		/* name of associated UNIX file */
	char		buf[BUFSIZ];		/* I/O buffer */
	char		window[1];		/* file window element */
};


/*
 * seek pointer struct for TELL, SEEK extensions
 */
struct seekptr {
        long    cnt;
};


/*
 * unit flags
 */
#define SPEOLN		0x100			/* 1 => pseudo EOLN char read at EOF */
#define FDEF		0x080			/* 1 => reserved file name */
#define FTEXT		0x040			/* 1 => text file, process EOLN */
#define FWRITE		0x020			/* 1 => open for writing */
#define FREAD		0x010			/* 1 => open for reading */
#define TEMP		0x008			/* 1 => temporary file */
#define SYNC		0x004			/* 1 => window is out of sync */
#define EOLN		0x002			/* 1 => at end of line */
#define EOFF		0x001			/* 1 => at end of file */

/*
 * file record variables
 */
extern struct iorechd	_fchain;		/* head of active file chain */
extern struct iorec *	_actfile[];		/* table of active files */
extern long		_filefre;		/* last used entry in _actfile */

/*
 * library variables
 */
extern char		*_maxptr;		/* maximum valid pointer */
extern char		*_minptr;		/* minimum valid pointer */
extern const char	**_pcargv;		/* values of passed args */
extern int		_pcargc;		/* number of passed args */
extern long		_stcnt; 		/* statement count */
extern long		_stlim; 		/* statement limit */
extern long		_seed;			/* random number seed */


/*
 * standard files
 */
extern struct iorechd	input;
extern struct iorechd	output;
extern struct iorechd	_err;

/*
 * Function prototypes
 */
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
#endif //__printf_attribute_

struct iorec;
struct formalrtn;

extern FILE	       *ACTFILE(struct iorec *);
extern long	       *ADDT(long *, long *, long *, long);
extern void		APPEND(struct iorec *);
extern void		ARGV(long, char *var, long);
extern void		ASRT(short cond);
extern void		ASRTS(short, char *);
extern double		ATAN(double);
extern void		BLKCLR(void *, long);
extern void		BLKCPY(const void *, void *, long);
extern void		BUFF(long);
extern long		CARD(unsigned char *, long);
extern void		CASERNG(int);
extern void		CATCHERR(long, struct formalrtn);
extern char		CHR(unsigned long);
extern long		CLCK(void);
extern double		COS(double);
extern long	       *CTTOT(long *, long, long, long, long, long);
extern long	       *CTTOTA(long *, long, long, long, long, const long *);
extern void		DATE(char *);
extern void		DEFNAME(struct iorec *, char *, long, long);
extern void		DFDISPOSE(char	**var, long size);
extern void		DISPOSE(char **, long );
extern long		ERROR(__printf_attribute__ const char *msg, ...) __attribute__ ((format (printf, 1, 2)));
extern void		EXCEPT(int);
extern double		EXP(double);
extern long		EXPO(double);
extern void		FLUSH(struct iorec *);
extern char	       *FNIL(struct iorec *);
extern void		GET(struct iorec *);
extern struct iorec    *GETNAME(struct iorec *, char *, long, long);
extern void		HALT(void);
extern bool		IN(long, long, long, char []);
extern bool		INCT(long, long, long, long);
extern void		IOSYNC(struct iorec *curfile);
extern void		LINO(void);
extern void		LLIMIT(struct iorec *, long);
extern double		LN(double);
extern long		MAX(long, long, long);
extern long	       *MULT(long *, long *, long *, long);
extern char	       *NAM(long, char *);
extern void		NEW(char **, long);
extern char	       *NILPTR(char *);
extern void		PACK(long, char *, char *, long, long, long, long);
extern void		PAGE(struct iorec *);
extern void		PCEXIT(int code);
extern void		PCLOSE(struct iorec *);
extern void		PCSTART(int);
extern long		PERROR(const char *, const char *);
extern struct iorec    *PFCLOSE(struct iorec *filep, bool);
extern void		PFLUSH(void);
extern void		PMFLUSH(long, long, long *);
extern long		PRED(long, long, long);
extern void		PUT(struct iorec *);
extern double		RANDOM(void);
extern long		RANG4(long, long, long);
extern long		READ4(struct iorec *);
extern double		READ8(struct iorec *);
extern char		READC(struct iorec *);
extern long		READE(struct iorec *, char *);
extern void		READLN(struct iorec *);
extern bool		RELEQ(long, char *, char *);
extern bool		RELNE(long, char *, char *);
extern bool		RELSGE(long, char *, char *);
extern bool		RELSGT(long, char *, char *);
extern bool		RELSLE(long, char *, char *);
extern bool		RELSLT(long, char *, char *);
extern bool		RELTGE(long, long *, long *);
extern bool		RELTGT(long, long *, long *);
extern bool		RELTLE(long, long *, long *);
extern bool		RELTLT(long, long *, long *);
extern void		REMOVE(char *name, long);
extern void		RESET(struct iorec *, char *, long, long);
extern void		REWRITE(struct iorec *, char *, long, long);
extern long		ROUND(double);
extern long		RSNG4(long, unsigned long);
extern long		SCLCK(void);
extern long		SEED(long);
extern void		SEEK(struct iorec *, struct seekptr *);
extern double		SIN(double);
extern double		SQRT(double);
extern void		STLIM(long);
extern long		SUBSC(long, long, long);
extern long		SUBSCZ(long, unsigned long);
extern long	       *SUBT(long *, long *, long *, long);
extern long		SUCC(long, long, long);
extern struct seekptr	TELL(struct iorec *);
extern bool		TEOF(struct iorec *);
extern bool		TEOLN(struct iorec *);
extern void		TIME(char *);
extern long		TRUNC(double);
extern struct iorec    *UNIT(struct iorec *);
extern void		UNPACK(long, char *, char *, long, long, long, long);
extern void		UNSYNC(struct iorec *);
extern void		VWRITEF(struct iorec *, FILE *, char *, va_list);
extern void		WRITEC(struct iorec *, char, FILE *);
#if defined(_MSC_VER) && defined(_Printf_format_string_)
extern void		WRITEF(struct iorec *, FILE *, _Printf_format_string_ const char *, ...);
#else
extern void		WRITEF(struct iorec *, FILE *, const char *, ...) __attribute__ ((format (printf, 3, 4)));
#endif
extern void		WRITES(struct iorec *, char *, int, int, FILE *);
extern void		WRITLN(struct iorec *);

#endif /*LIBPC_H_INCLUDED*/
