/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
#ifndef PROTO_H_INCLUDED
#define PROTO_H_INCLUDED
/*
 *	Public pi/pc/pxp prototypes ....
 */
#include <tree.h>
#include <tree_ty.h>
#include <objfmt.h>

struct yytok;
struct tmps;
struct nl;
struct tnode;

/* ato.c */
long            a8tol __P((char *));

/* error.c */
void            panic __P((const char *s));
void            error __P((const char *s, ...));
void            verror __P((const char *s, va_list ap));
void            cerror __P((const char *s, ...));
void            derror __P((const char *s, ...));

/* hash.c */
void            inithash __P((void));
int *		hash __P((char *s, int save));

/* mktemp.c */
#if defined(_WIN32) && (defined(_MSC_VER) || defined(__WATCOMC__))
char *		xmktemp __P((char *path, char *result, size_t pathlen));
#define mktemp(__x) mktemp(__x)
#endif				       
#if !defined(_MSC_VER)
char *		mktemp __P((char *path));
#endif
int             mkstemp __P((char *path));

/* nl.c */
void            initnl __P((void));
struct nl *	hdefnl __P((char *sym, int cls, struct nl *typ, int val));
void            nlfree __P((struct nl *p));
void            dumpnl __P((struct nl *to, const char *rout));
struct nl *	defnl __P((char *sym, int cls, struct nl *typ, int val));
struct nl *	nlcopy __P((struct nl *p));
int             nloff __P((struct nl *p));
struct nl *	enter __P((struct nl *np));

/* string.c */
int             initstring __P((void));
char *		savestr __P((char *cp));
char *		esavestr __P((char *cp));
int             soffset __P((char *cp));
int             sreloc __P((int i));

/* subr.c */
int             dotted __P((char *fp, char c));
void            togopt __P((char c));
void            gettime __P((const char *filename));
char *          myctime __P((time_t *tv));
int             inpflist __P((char *fp));
void            Perror __P((const char *file, const char *error));
void *          pcalloc __P((int num, int size));
int             pstrcmp __P((const char *s1, const char *s2));
char *          pstrcpy __P((char *s1, const char *s2));
void            copy __P((char *to, char *from, int bytes));
int             any __P((char *cp, char ch));
void            opush __P((CHAR c));
void            opop __P((CHAR c));

/* tree.c */
void            inittree __P((void));
struct tnode *	tree __P((int cnt, ...));
void            tralloc __P((int howmuch));
void            trfree __P((void));
char *          copystr __P((const char *token));

/* treen.c */
struct tnode *	tree1 __P((int arg1));
struct tnode *	tree2 __P((int arg1, int arg2));
struct tnode *	tree3 __P((int arg1, int arg2, struct tnode *arg3));
struct tnode *	tree4 __P((int arg1, int arg2, struct tnode *arg3, struct tnode *arg4));
struct tnode *	tree5 __P((int arg1, int arg2, struct tnode *arg3, struct tnode *arg4, struct tnode *arg5));

/* yycopy.c */
void            OYcopy __P((void));

/* ycosts.c */
int             inscost __P((int sy, int before));
int             repcost __P((int what, int with));
int             delcost __P((int what));
void            yycosts __P((void));
void            yydocost __P((int c));

/* yyerror.c */
void            yerror __P((const char *s, ...));
void            brerror __P((int where, const char *what));

/* yyget.c */
int             readch __P((void));
int             xgetline __P((void));
int             includ __P((void));
char            *skipbl __P((char *ocp));
int             uninclud __P((void));

/* yyid.c */
int             identis __P((char *cp, int kind));
struct nl       *yybaduse __P((char *cp, int line, int kindmask));
void            yybadref __P((struct nl *p, int line));
int             yyidok __P((struct nl *p, int kind));
int             yyidok1 __P((struct nl *p, int kind));
int             yyisvar __P((struct nl *p, int varclass));

/* yylex.c */
void            unyylex __P((struct yytok *y));
int             yylex __P((void));
void            yyset __P((void));
void            setuflg __P((void));

/* yymain.c */
void            yymain __P((void));

/* yyoption.c */
int             options __P((void));

/* yypanic.c */
void            yyPerror __P((char *cp, int kind));

/* yyparse.c */
void            yyparse __P((void));

/* yyprint.c */
char *		charname __P((int ch, int which));
char *		tokname __P((struct yytok *tp, int which));

/* yyput.c */
void            yyoutline __P((void));
void            yyflush __P((void));
void            yyoutfl __P((int toseqid));
void            yysync __P((void));
void            yySsync __P((void));
void            yygetline __P((char *efile, int seekp, int eline, int eseqid));
void            yyretrieve __P((void));
void            yyprline __P((char *buf, int line, char *file, int id));
void            yyprintf __P((char *cp, int line));
int             graphic __P((int ch));
void            yysetfile __P((const char *file));
void            yyputfn __P((const char *cp));

/* yyrecove.c */
int             yyrecover __P((int *Ps0, int idfail));
void            yyexeof __P((void));
void            yyunexeof __P((void));
void            trystate __P((int *Ps0, int *Pv0, int flag, char *insmult, char *delmult, char *repmult));
int             correct __P((int fchar, int origin, int c, char *multvec, int *Ps0, int *Pv0));
int *		loccor __P((int *ps, struct yytok *ntok));

/* yyseman.c */
int             nullsem __P((int ch));

/* yytree.c */
struct tnode *	newlist __P((struct tnode *new));
struct tnode *	addlist __P((struct tnode *vroot, struct tnode *new));
struct tnode *	fixlist __P((struct tnode *vroot));
struct tnode *	setupvar __P((char *var, struct tnode *init));
struct tnode *	setuptyrec __P((int line, struct tnode *fldlist));
struct tnode *	setupfield __P((struct tnode *field, struct tnode *other));

/* ytab.c */
void		yyactr __P((int));
int             yyEactr __P((int, char *));

#endif	/*PROTO_H_INCLUDED*/

