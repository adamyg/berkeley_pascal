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
 *	@(#)yy.h	8.1 (Berkeley) 6/6/93
 */

#include "y.tab.h"
#undef CBSIZE	/* from paramsys/param.h */
/*
 * INPUT/OUTPUT 
 */

/*
 * The buffer for the input file is normally "ibuf".
 * When files are included, however, this may be
 * pushed down in the stack of currently active
 * files. For this reason, the pointer ibp always
 * references the i/o buffer of the current input file.
 */
FILE		*ibuf, *ibp;

/*
 * Line and token buffers.  Charbuf is the character buffer for
 * input lines, token the buffer for tokens returned
 * by the scanner.  CBSIZE defines the maximum line
 * length allowed on input and is doubtless too small.
 * The token buffer should be a local array in yylex.
 */
#ifdef ADDR16
#define CBSIZE 161
#endif ADDR16
#ifdef ADDR32
#define CBSIZE 1024
#endif ADDR32

char	charbuf[CBSIZE], *bufp, token[CBSIZE];

#define digit(c)	(c >= '0' && c <= '9')
#define alph(c)		((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

/*
 * Flag to prevent reprinting current line after
 * an error.
 */
char	yyprtd;

/*
 * The following variables are maintained by
 * the scanner in the file lex and used in scanning
 * and in parsing.
 *
 * The variable yychar is the current scanner character.
 * Currently, the scanner must be called as
 *	yychar = yylex()
 * even though it should set yychar itself.
 * Yychar has value YEOF at end of file, and negative value if
 * there is no yychar, e.g. after a shift in the parser.
 *
 * The variable yycol is the current column in the line whose number
 * is given by yyline.  Yyecol and yyeline give the position for an
 * error message to flag, usually the start of an input token.
 * Yylval is the semantic return from the scanner.
 *
 * In fact all of these variables are "per token".
 * In the usual case, only the copies in the scanner token structure
 * 'Y' are used, and the #defines below serve to make them look
 * like variables.
 *
 * For the purposes of the error recovery, however, they are copied
 * and restored quite freely.  For the error recovery also, the
 * file name which the input line this token is on and the seek
 * pointer of this line in its source file are saved as yyefile
 * and yyseekp.  The global variable yylinpt is the seek pointer
 * of the current input line.
 */
int	yycol;
int	yyline;
int	yyseqid;
int	yysavc;
int	yylinpt;

/* *** NOTE ***
 * It would be much better to not have the Yyeline and Yyefile
 * in the scanner structure and to have a mechanism for mapping
 * seqid's to these globally.
 */
struct yytok {
	int	Yychar;
	int	Yylval;
	int	Yyecol;
	int	Yyeline;
	int	Yyseekp;
	char	*Yyefile;
	int	Yyeseqid;
} Y, OY;

#define	yychar	Y.Yychar
#define	yylval	Y.Yylval
#define	yyecol	Y.Yyecol
#define	yyeline	Y.Yyeline
#define	yyseekp	Y.Yyseekp
#define	yyefile	Y.Yyefile
#define	yyeseqid Y.Yyeseqid

/* Semantic Stack so that y.tab.c will lint */

union semstack
{
    int		  i_entry;
    struct nl	 *nl_entry;
    struct tnode *tr_entry;
    char	 *cptr;
} yyval;

/*
 * Yyval is the semantic value returned by a reduction.
 * It is what "$$" is expanded to by yacc.
 */

int	*Ps;

/*
 * N is the length of a reduction.
 * Used externally by "lineof" to get the left and
 * right margins for a reduction.
 */
int	N;
/*
 * Definitions for looking up keywords.
 * The keyword array is called yykey, and
 * lastkey points at the end of it.
 */
char	*lastkey;

struct kwtab {
	char	*kw_str;
	int	kw_val;
} yykey[];

/*
 * ERROR RECOVERY EXTERNALS
 */

#define	CLIMIT	40	/* see yyrecover.c */
char	*tokname();
char	*charname();

char	*classes[];

/*
 * Tokens which yacc doesn't define
 */
#define	YEOF	0
#define	ERROR	256

/*
 * Limit on the number of syntax errors
 */
#define	MAXSYNERR	100

/*
 * Big costs
 */
#define	HUGE		50
#define	INFINITY	100

/*
 * Kinds of panics
 */
#define	PDECL	0
#define	PSTAT	1
#define	PEXPR	2
#define	PPROG	3

#define	yyresume()	yyResume = 1;

char	yyResume;

char	dquote;

#ifndef PC
#ifndef OBJ
char	errout;
#endif OBJ
#endif PC

/*
 * Yyidwant and yyidhave are the namelist classes
 * of identifiers associated with a identifier reduce
 * error, set before the recovery is called.
 * Since they may be set again during the forward move
 * they must be saved by yyrecover, which uses them in printing
 * error messages.
 */
int	yyidhave, yyidwant;

/*
 * The variables yy*shifts are used to prevent looping and the printing
 * of spurious messages in the parser.  Yyshifts gives the number of
 * true input shifts since the last corrective action.  YyOshifts
 * is the value of yyshifts before it was last cleared, and is used
 * by yyPerror in yypanic.c to suppress messages.
 *
 * Yytshifts counts true input shifts.  It is used to prevent looping
 * inserting unique symbols.  If yytshifts == yyTshifts (local to
 * yyrecover.c) then there has been no shift over true input since
 * the last unique symbol insertion.  We refuse, in this case,
 * to insert more unique symbols so as to prevent looping.
 *
 * The recovery cannot loop because it guarantees the progress of the
 * parse, i.e.:
 *
 *	1) Any insertion guarantees to shift over 2 symbols, a replacement
 *	   over one symbol.
 *
 *	2) Unique symbol insertions are limited to one for each true
 *	   symbol of input, or "safe" insertion of the keywords "end"
 *	   and "until" at zero cost (safe since these are know to match
 *	   stack that cannot have been generated - e.g. "begin" or "repeat")
 *
 *	3) We never panic more than once from a given state without
 *	   shifting over input, i.e. we force the parse stack to shrink
 *	   after each unsuccessful panic.
 */
int	yyshifts, yyOshifts;
unsigned yytshifts;

#ifdef PXP

/*
 * Identifier class definitions
 */
#define	UNDEF	0
#define	CONST	1
#define	TYPE	2
#define	VAR	3
#define	ARRAY	4
#define	PTRFILE	5
#define	RECORD	6
#define	FIELD	7
#define	PROC	8
#define	FUNC	9
#define	FVAR	10
#define	REF	11
#define	PTR	12
#define	FILET	13
#define	SET	14
#define	RANGE	15
#define	LABEL	16
#define	WITHPTR 17
#define	SCAL	18
#define	STR	19
#define	PROG	20
#define	IMPROPER 21

/*
 * COMMENT FORMATTING DEFINITIONS
 */

/*
 * Count of tokens on this input line
 * Note that this can be off if input is not syntactically correct.
 */
int	yytokcnt;
int	yywhcnt;

/*
 * Types of comments
 */
#define	CLMARG	0
#define	CALIGN	1
#define	CTRAIL	2
#define	CRMARG	3
#define	CSRMARG	4
#define	CNL	5
#define	CNLBL	6
#define	CFORM	7
#define	CINCLUD	8

/*
 * Comment structure
 * Cmhp is the head of the current list of comments
 */
struct comment {
	struct	comment *cmnext;
	int	cmdelim;
	struct	commline *cml;
	int	cmjust;
	int	cmseqid;
} *cmhp;

/*
 * Structure for holding a comment line
 */
struct commline {
	char	*cmtext;
	int	cmcol;	/* Only used for first line of comment currently */
	struct	commline *cml;
};

struct W {
	int	Wseqid;
	int	Wcol;
} yyw[MAXDEPTH + 1], *yypw;

#define	commform()	quickcomm(CFORM)
#define	commnl()	quickcomm(CNL)
#define	commnlbl()	quickcomm(CNLBL)
#endif
