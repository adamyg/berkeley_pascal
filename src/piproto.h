/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
#ifndef PIPROTO_H_INCLUDED
#define PIPROTO_H_INCLUDED
/*
 *	pi/pc specific prototypes ....
 */

struct yytok;
struct tmps;
struct nl;
struct tnode;
struct entry_exit_cookie;

/* main.c */
void		pexit __P((int c));
void            pchr __P((int c));
#if defined(unix)
void            geterr __P((int seekpt, char *buf));
#endif
void		header __P((void));

/* call.c */
struct nl *	call __P((struct nl *p, struct tnode *argv_node, int porf, int psbn));
void            rvlist __P((struct tnode *al));
bool            fcompat __P((struct nl *formal, struct nl *actual));
const char *	parnam __P((int nltype));
struct nl *	plist __P((struct nl *p));
int             linenum __P((struct nl *p));

/* case.c */
void            caseop __P((WHI_CAS *rnode));

/* clas.c */
int             classify __P((struct nl *p1));
int             text __P((struct nl *p));
struct nl *	scalar __P((struct nl *p1));
int             isa __P((struct nl *p, char *s));
int             isnta __P((struct nl *p, char *s));
const char *	nameof __P((struct nl *p));
int             nowexp __P((struct tnode *r));
int             whereis __P((int offset, char other_flags));

/* const.c */
void            constbeg __P((int lineofyconst, int cline));
void            constant __P((int cline, char *cid, struct tnode *Cdecl));
void            constend __P((void));
void            gconst __P((struct tnode *c_node));
int             isconst __P((struct tnode *cn));

/* conv.c */
void            convert __P((struct nl *p1, struct nl *p2));
int             compat __P((struct nl *p1, struct nl *p2, struct tnode *t));
void            rangechk __P((struct nl *p, struct nl *q));
void            precheck __P((struct nl *p, char *name1, char *name2));
void            postcheck __P((struct nl *need, struct nl *have));
void            conv __P((int *dub));

/* cset.c */
bool            precset __P((struct tnode *r, struct nl *settype, struct csetstr *csetp));
void            postcset __P((struct tnode *r, struct csetstr *csetp));

/* fdec.c */
struct nl *	funcext __P((struct nl *fp));
struct nl *	funcbody __P((struct nl *fp));
void            funcfwd __P((struct nl *fp));
void            segend __P((void));

void            level1 __P((void));
void            pnums __P((struct udinfo *p));
void            nerror __P((const char *s, ...));

/* fend.c */
void            funcend __P((struct nl *fp, struct tnode *bundle, int endline));
#ifdef PC
void            sextname __P((char buffer[], char *name, int level));
#endif
void            codeformain __P((void));
void            prog_prologue __P((struct entry_exit_cookie *eecookiep));
void            fp_prologue __P((struct entry_exit_cookie *eecookiep));
void            fp_entrycode __P((struct entry_exit_cookie *eecookiep));
void            fp_exitcode __P((struct entry_exit_cookie *eecookiep));
void            fp_epilogue __P((struct entry_exit_cookie *eecookiep));
void            fp_formalentry __P((struct entry_exit_cookie *eecookiep));

/* fhdr.c */
struct nl *	funchdr __P((struct tnode *r));
void            fparams __P((struct nl *p, struct tnode *formal));
int             params __P((struct nl *p, struct tnode *formalist));

/* flvalue.c */
struct nl *	flvalue __P((struct tnode *r, struct nl *formalp));

/* forop.c */
void            forop __P((struct tnode *tree_node));

/* func.c */
struct nl *	funccod __P((struct tnode *r));

/* gen.c */
void            genmx __P((void));
struct nl *	gen __P((int p, int o, int w1, int w2));

/* getw.c */
#if defined(_MSC_VER)
#ifndef getw
#define getw(__f) _getw(__f)
#endif
#else
int             getw __P((FILE *fp));
#endif

/* lab.c */
void            label __P((struct tnode *r, int l));
void            gotoop __P((char *s));
void            labeled __P((char *s));
#if defined(PC)
void            extlabname __P((char buffer[], char *name, int level));
#endif

/* lookup.c */
struct nl *	lookup __P((char *s));
struct nl *	lookup1 __P((char *s));
int             nlfund __P((char *sp));

/* lval.c */
struct nl *	lvalue __P((struct tnode *var, int modflag, int required));
int             lptr __P((struct tnode *c));
int             arycod __P((struct nl *np, struct tnode *el, int n));
void            putcbnds __P((struct nl *ctype, int i));

/* p2put.c */
#if defined(PC)
void            puttext __P((char *string));
int             str4len __P((char *string));
void            putprintf __P((const char *format, int incomplete, ...));
void            putlbracket __P((int ftnno, struct om *sizesp));
void            putrbracket __P((int ftnno));
void            puteof __P((void));
void            putdot __P((char *filename, int line));
void            putleaf __P((int op, int lval, int rval, int type, char *name));
void            putRV __P((char *name, int level, int offset, char other_flags, int type));
void            putLV __P((char *name, int level, int offset, char other_flags, int type));
void            putCON8 __P((double val));
void            putCONG __P((char *string, int length, int required));
int             p2type __P((struct nl *np));
int             typerecur __P((struct nl *np, int quals));
void            putop __P((int op, int type));
void            putstrop __P((int op, int type, int size, int alignment));
char *		p2opname __P((int op));
void            p2word __P((int word));
void            p2string __P((char *string));
void            p2name __P((char *name));
void            putjbr __P((long label));
void            printjbr __P((char *prefix, long label));
#endif

/* pccaseop.c */
#if defined(PC)
void            pccaseop __P((WHI_CAS *tcase));
void            directsw __P((struct ct *ctab, int count));
void            binarysw __P((struct ct *ctab, int count));
void            bsrecur __P((int deflabel, struct ct *ctab, int count));
void            itesw __P((struct ct *ctab, int count));
int             casecmp __P((struct ct *this, struct ct *that));
#endif

/* pcfunc.c */
#if defined(PC)
struct nl *	pcfunccod __P((struct tnode *r));
#endif

/* pclval.c */
#if defined(PC)
struct nl *	pclvalue __P((struct tnode *var, int modflag, int required));
bool            nilfnil __P((struct nl *p, struct tnode *c, int modflag, struct nl *firstp, char *r2));
#endif

/* pcproc.c */
#if defined(PC)
void            pcproc __P((struct tnode *r));
#endif

/* proc.c */
void            proc __P((struct tnode *r));

/* put.c */
#if defined(OBJ)
int             put __P((int n, int opt, ...));
#endif
int             listnames __P((struct nl *ap));
char *		getnext __P((struct nl *next, struct nl **new));
#if defined(OBJ)
void            putspace __P((int n));
void            putstr __P((char *sptr, int padding));
int             lenstr __P((char *sptr, int padding));
#endif
void            patch __P((PTR_DCL loc));
#if defined(OBJ)
void            patch4 __P((PTR_DCL loc));
void            patchfil __P((PTR_DCL loc, long jmploc, int words));
void            dumphex __P((long, const void *pBuf, long));
void            word __P((int o));
void            dword __P((long o));
void            qword __P((double o));
void            pflush __P((void));
#endif
char *		getlab __P((void));
char *		putlab __P((char *l));

/* putw.c */
#if defined(_MSC_VER)
#ifndef putw
#define putw(__w, __f) _putw(__w, __f)
#endif
#else
int             putw __P((int w, FILE *fp));
#endif

/* rec.c */
struct nl *	tyrec __P((struct tnode *r, int off));
struct nl *	tyrec1 __P((struct tnode *r, bool first));
void            fields __P((struct nl *p, struct tnode *r));
void            variants __P((struct nl *p, struct tnode *r));
struct nl *	deffld __P((struct nl *p, char *s, struct nl *t));
struct nl *	defvnt __P((struct nl *p, struct tnode *t, struct nl *vr, struct nl *ct));
void            uniqv __P((struct nl *p));
struct nl *	reclook __P((struct nl *p, char *s));
void            rec_offsets __P((struct nl *recp, long offset));

/* rval.c */
struct nl *	rvalue __P((struct tnode *r, struct nl *contype, int required));
int             nocomp __P((int c));
bool            constval __P((struct tnode *r));

/* savenl.c*/
void            startnlfile __P((void));
void            copynlfile __P((void));
void            cat __P((int fd));
void            removenlfile __P((void));
int             nlhdrsize __P((void));
void            savenl __P((struct nl *to, int rout));
void            lineno __P((int line));
void            newfile __P((char *s, int line));

/* sconv.c */
void            tuac __P((struct nl *thistype, struct nl *thattype, struct nl **resulttypep, int *resultp2typep));
int             sconv __P((int fromp2type, int top2type));

/* stab.c */
#if defined(PC)
void            stabgvar __P((struct nl *p, int length, int line));
void            stablvar __P((struct nl *p, int offset, int length));
void            oldstabgvar __P((char *name, int type, int offset, int length, int line));
void            oldstablvar __P((char *name, int type, int level, int offset, int length));
void            stabparam __P((struct nl *p, int offset, int length));
void            oldstabparam __P((char *name, int type, int offset, int length));
void            stablbrac __P((int level));
void            stabrbrac __P((int level));
void            stabfunc __P((struct nl *p, char *name, int line, int level));
void            oldstabfunc __P((char *name, int typeclass, int line, long level));
void            stabline __P((int line));
void            stabsource __P((char *filename, bool firsttime));
void            stabinclude __P((char *filename, bool firsttime));
long            checksum __P((char *filename));
void            stabglabel __P((char *label, int line));
void            stabgconst __P((char *constant, int line));
void            stabconst __P((struct nl *c));
void            stabgtype __P((char *name, struct nl *type, int line));
void            stabltype __P((char *name, struct nl *type));
void            stabefunc __P((char *name, int typeclass, int line));
void            fixfwdtype __P((struct nl *t));
#endif

/* stat.c */
void            statlist __P((struct tnode *r));
void            statement __P((struct tnode *r));
void            ungoto __P((void));
void            putcnt __P((void));
int             getcnt __P((void));
void            inccnt __P((int counter));
void            putline __P((void));
void            withop __P((WITH_NODE *s));
void            asgnop __P((ASG_NODE *r));
struct nl *	asgnop1 __P((ASG_NODE *r, struct nl *p));
struct nl *	pcasgconf __P((ASG_NODE *r, struct nl *p));
void            ifop __P((IF_NODE *if_n));
void            whilop __P((WHI_CAS *w_node));
void            repop __P((REPEAT *r));

/* stklval.c */
struct nl *	stklval __P((struct tnode *r, int modflag));

/* stkrval.c */
struct nl *	stkrval __P((struct tnode *r, struct nl *contype, long required));
struct nl *	stackRV __P((struct nl *p));

/* tmps.c */
void            tmpinit __P((int cbn));
struct nl *	tmpalloc __P((long size, struct nl *type, int mode));
void            tmpfree __P((struct tmps *restore));
int             savmask __P((void));

/* type.c */
void            typebeg __P((int lineofytype, int r));
void            type __P((int tline, char *tid, struct tnode *tdecl));
void            typeend __P((void));
struct nl *	gtype __P((struct tnode *r));
struct nl *	tyscal __P((struct tnode *r));
struct nl *	tycrang __P((struct tnode *r));
struct nl *	tyrang __P((struct tnode *r));
int		norange __P((struct nl *p));
struct nl *	tyary __P((struct tnode *r));
void            foredecl __P((void));

/* var.c */
void            varbeg __P((int lineofyvar, int r));
void            var __P((int vline, struct tnode *vidl, struct tnode *vtype));
void            varend __P((void));
int             width __P((struct nl *np));
long            lwidth __P((struct nl *np));
long            roundup __P((int x, long y));
int             align __P((struct nl *np));
#ifdef PC
void            aligndot __P((int alignment));
#endif
long            aryconst __P((struct nl *np, int n));
void            setran __P((struct nl *q));
int             bytes __P((long lb, long ub));

#endif	/*PIPROTO_H_INCLUDED*/
