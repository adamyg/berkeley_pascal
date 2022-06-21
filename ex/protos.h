/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
#ifndef PROTO_H_INCLUDED
#define PROTO_H_INCLUDED

#if defined(__STDC__) || defined(__cplusplus) || \
		defined(_MSC_VER) || defined(__WATCOMC__)
# define __P(s) s
#else
# define __P(s) ()
#endif

#if defined(__WATCOMC__)
#pragma disable_message(107)  // Missing return value for function 'xxx'
#pragma disable_message(131)  // No prototype found for function 'xxx'
#pragma disable_message(202)  // Symbol 'x' has been defined, but not referenced
#pragma disable_message(304)  // Return type 'int' assumed for function 'xxx'
#endif

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable:4013) // 'xxx' undefined; assuming extern returning int
#pragma warning(disable:4033) // 'xxx' must return a value
#pragma warning(disable:4101) // 'xxx': unreferenced local variable
#pragma warning(disable:4716) // 'xxx': must return a value
#pragma warning(disable:4996) // 'xxx': The POSIX name for this item is deprecated.

#endif

#ifdef EXSTRINGS
#define MSGSTR	int
#else
#define MSGSTR	char *
#endif
		
#if defined(unix)
#define 	signal_t sig_t
#else
typedef void	(*signal_t)(int);		/* signal handlers */
#endif
typedef void	(*out_t)(int);			/* character output */
typedef void	(*opf_t)(int);			/* operation callback */
typedef int	(*io_t)(int, void *, unsigned); /* i/O callback */

extern out_t	Outchar;
extern out_t	Pline;
extern out_t	Put_char;
extern signal_t	oldhup;

#include "ex_tty.h"				/* ttymode */

/* ex.c */
int		main __P((int ac, char *av[]));
void		init __P((void));
char *		tailpath __P((char *p));
int		iownit __P((char *file));

/* ex_addr.c */
void		setdot __P((void));
void		setdot1 __P((void));
void		setcount __P((void));
int		getnum __P((void));
void		setall __P((void));
void		setnoaddr __P((void));
line *		address __P((char *inputline));
void		setCNL __P((void));
void		setNAEOL __P((void));

/* ex_cmds.c */
void		commands __P((bool noprompt, bool exitoneof));

/* ex_cmds2.c */
int		cmdreg __P((void));
int		endcmd __P((int ch));
void		eol __P((void));
void		error __P((MSGSTR, ...));
void		serror __P((MSGSTR, ...));
void		erewind __P((void));
void		error0 __P((void));
void		error1 __P((MSGSTR));
void		fixol __P((void));
int		exclam __P((void));
void		makargs __P((void));
void		next __P((void));
void		newline __P((void));
void		nomore __P((void));
int		quickly __P((void));
void		resetflav __P((void));
void		setflav __P((void));
int		skipend __P((void));
void		tailspec __P((int c));
void		tail __P((char *comm));
void		tail2of __P((char *comm));
void		tailprim __P((char *comm, int i, bool notinvis));
void		vcontin __P((bool ask));
void		vnfl __P((void));

/* ex_cmdsub.c */
int		append __P((int (*f )(), line *a));
void		appendnone __P((void));
void		pargs __P((void));
void		ex_delete __P((int hush));
void		deletenone __P((void));
void		squish __P((void));
void		join __P((int c));
void		move __P((void));
void		move1 __P((int cflag, line *addrt));
int		getcopy __P((void));
int		getput __P((void));
void		put __P((int unused));
void		pragged __P((bool kill));
void		shift __P((int c, int cnt));
void		yank __P((int unused));
void		tagfind __P((bool quick));
void		zop __P((int hadpr));
void		zop2 __P((int ines, int op));
void		plines __P((line *adr1, line *adr2, bool movedot));
void		pofix __P((void));
void		undo __P((bool c));
void		somechange __P((void));
void		mapcmd __P((int un, int ab));
void		addmac __P((char *src, char *dest, char *dname, struct maps *mp));
void		cmdmac __P((char c));

/* ex_data.c */

/* ex_extern.c */

/* ex_get.c */
void		ignchar __P((void));
int		ex_getchar __P((void));
int		getcd __P((void));
int		peekchar __P((void));
int		peekcd __P((void));
int		getach __P((void));
int		gettty __P((void));
int		smunch __P((int col, char *ocp));
void		checkjunk __P((int c));
void		setin __P((line *addr));

/* ex_io.c */
struct glob;

void		filename __P((int comm));
int		getargs __P((void));
void		glob __P((struct glob *gp));
int		gscan __P((void));
void		getone __P((void));
void		rop __P((int c));
void		rop2 __P((void));
void		rop3 __P((int c));
int		samei __P((struct stat *sp, char *cp));
void		wop __P((bool dofname));
int		edfile __P((void));
int		getfile __P((void));
void		putfile __P((int isfilter));
void		wrerror __P((void));
void		source __P((char *fil, bool okfail));
void		clrstats __P((void));
int		iostats __P((void));
void		checkmodeline __P((char *line));

/* ex_put.c */
out_t		setlist __P((bool t));
out_t		setnumb __P((bool t));
void		listchar __P((int c));
void		normchar __P((int c));
void		numbline __P((int i));
void		normline __P((int i));
void		slobber __P((int c));
void		ex_putchar __P((int c));
void		termchar __P((int c));
void		flush __P((void));
void		flush1 __P((void));
void		flush2 __P((void));
void		fgoto __P((void));
void		tab __P((int col));
void		plodput __P((int c));
int		plod __P((int cnt));
void		noteinp __P((void));
void		termreset __P((void));
void		draino __P((void));
void		flusho __P((void));
void		putnl __P((void));
void		ex_putS __P((char *cp));
void		putch __P((int c));
void		putpad __P((char *cp));
void		setoutt __P((void));
void		lprintf __P((const char *cp, ...));
void		lvprintf __P((const char *cp, va_list));
void		putNFL __P((void));
void		pstart __P((void));
void		pstop __P((void));
ttymode 	ostart __P((void));
void		tostart __P((void));
void		ttcharoff __P((void));
void		ostop __P((ttymode f));
void		tostop __P((void));
void		vcook __P((void));
void		vraw __P((void));
void		normal __P((ttymode f));
ttymode 	setty __P((ttymode f));
void		ex_gTTY __P((int i));
void		ex_sTTY __P((int i));
void		noonl __P((void));

/* ex_re.c */
void		global __P((bool k));
void		gdelete __P((void));
int		substitute __P((int c));
int		compsub __P((int ch));
void		comprhs __P((int seof));
int		getsub __P((void));
int		dosubcon __P((bool f, line *a));
int		confirmed __P((line *a));
int		getch __P((void));
void		ugo __P((int cnt, int with));
void		dosub __P((void));
int		fixcase __P((int c));
char *		place __P((char *sp, char *l1, char *l2));
void		snote __P((int total, int lines));
int		compile __P((int eof, int oknl));
void		cerror __P((char *s));
int		same __P((int a, int b));
int		execute __P((int gf, line *addr));
int		advance __P((char *lp, char *ep));
int		cclass __P((char *set, int c, int af));

/* ex_set.c */
void		set __P((void));
int		setend __P((void));
void		prall __P((void));
void		propts __P((void));
void		propt __P((struct option *op));

/* ex_subr.c */
int		any __P((int c, char *s));
int		backtab __P((int i));
void		change __P((void));
int		column __P((char *cp));
void		comment __P((void));
void		Copy __P((char *to, char *from, int size));
void		copyw __P((line *to, line *from, int size));
void		copywR __P((line *to, line *from, int size));
int		ctlof __P((int c));
void		dingdong __P((void));
int		fixindent __P((int indent));
void		filioerr __P((char *cp));
char *		genindent __P((int indent));
void		getDOT __P((void));
line *		getmark __P((int c));
int		getn __P((char *cp));
void		ignnEOF __P((void));
int		iswhite __P((int c));
int		junk __P((int c));
void		killed __P((void));
void		killcnt __P((int cnt));
int		lineno __P((line *a));
int		lineDOL __P((void));
int		lineDOT __P((void));
void		markDOT __P((void));
void		markpr __P((line *which));
int		markreg __P((int c));
char		*mesg __P((char *str));
void		merror __P((MSGSTR, ...));
void		smerror __P((MSGSTR, ...));
void		mverror __P((MSGSTR, va_list));
void		smverror __P((MSGSTR, va_list));
void		merror1 __P((MSGSTR));
int		morelines __P((void));
void		nonzero __P((void));
int		notable __P((int i));
void		notempty __P((void));
void		netchHAD __P((int cnt));
void		netchange __P((int i));
void		putmark __P((line *addr));
void		putmk1 __P((line *addr, int n));
char *		plural __P((long i));
int		qcolumn __P((char *lim, char *gp));
void		qcount __P((int c));
void		reverse __P((line *a1, line *a2));
void		save __P((line *a1, line *a2));
void		save12 __P((void));
void		saveall __P((void));
int		span __P((void));
void		ex_sync __P((void));
int		skipwh __P((void));
char *		strend __P((char *cp));
void		strcLIN __P((char *dp));
void		syserror __P((void));
int		tabcol __P((int col, int ts));
char *		vfindcol __P((int i));
char *		vskipwh __P((char *cp));
char *		vpastwh __P((char *cp));
int		whitecnt __P((char *cp));
int		Ignore __P((char *a));
int		Ignorf __P((int (*a )()));
void		markit __P((line *addr));
void		onemt __P((int sig));
void		onhup __P((int));
void		onintr __P((int));
void		setrupt __P((void));
int		preserve __P((void));
void		ex_exit __P((int i));
void		onsusp __P((int));

/* ex_tagio.c */
int		topen __P((char *file, char *buf));
int		tseek __P((int fd, long off));
int		tgets __P((char *buf, int cnt, int fd));
void		tclose __P((int fd));

/* ex_temp.c */
void		fileinit __P((void));
void		cleanup __P((bool all));
void		ex_getline __P((line tl));
int		putline __P((void));
char *		getblock __P((line atl, int iof));
void		blkio __P((int b, char *buf, io_t iofcn));
void		tlaste __P((void));
void		tflush __P((void));
void		synctmp __P((void));
void		TSYNC __P((void));
void		regio __P((short b, io_t iofcn));
int		REGblk __P((void));
struct strreg * mapreg __P((int c));
void		KILLreg __P((int c));
int		shread __P((int, void *, unsigned));
void		putreg __P((int c));
int		partreg __P((int c));
void		notpart __P((int c));
int		getREG __P((void));
void		YANKreg __P((int c));
void		kshift __P((void));
void		YANKline __P((void));
void		rbflush __P((void));
void		regbuf __P((char c, char *buf, int buflen));

/* ex_tty.c */
void		gettmode __P((void));
void		setterm __P((char *type));
void		setsize __P((void));
void		zap __P((void));
char *		longname __P((char *bp, char *def));
char *		fkey __P((int i));
int		cost __P((char *str));
void		countnum __P((int ch));

/* ex_un	ix.c */
void		unix0 __P((bool warn));
ttymode 	unixex __P((char *opt, char *up, int newstdin, int mode));
void		unixwt __P((bool c, ttymode f));
void		filter __P((int mode));
void		recover __P((void));
void		waitfor __P((void));
void		revocer __P((void));

/* ex_v.c */
void		oop __P((void));
void		ovbeg __P((void));
void		ovend __P((ttymode f));
void		vop __P((void));
void		fixzero __P((void));
void		savevis __P((void));
void		undvis __P((void));
void		setwind __P((void));
void		vok __P((char *atube));
void		vonintr __P((int sig));
void		vsetsiz __P((int size));
void		onwinch __P((int sig));

/* ex_vadj.c */
void		vopen __P((line *tp, int p));
int		vreopen __P((int p, int lineno, int l));
int		vglitchup __P((int l, int o));
void		vinslin __P((int p, int cnt, int l));
void		vopenup __P((int cnt, bool could, int l));
void		vadjAL __P((int p, int cnt));
void		vrollup __P((int dl));
void		vup1 __P((void));
void		vmoveitup __P((int cnt, bool doclr));
void		vscroll __P((int cnt));
void		vscrap __P((void));
void		vrepaint __P((char *curs));
void		vredraw __P((int p));
void		vdellin __P((int p, int cnt, int l));
void		vadjDL __P((int p, int cnt));
void		vsyncCL __P((void));
void		vsync __P((int p));
void		vsync1 __P((int p));
void		vcloseup __P((int l, int cnt));
void		vreplace __P((int l, int cnt, int newcnt));
void		sethard __P((void));
void		vdirty __P((int base, int i));

/* ex_vget.c */
void		ungetkey __P((int c));
int		getkey __P((void));
int		peekbr __P((void));
int		getbr __P((void));
int		getesc __P((void));
int		peekkey __P((void));
int		readecho __P((char c));
void		setLAST __P((void));
void		addtext __P((char *cp));
void		setDEL __P((void));
void		ex_setBUF __P((char *BUF));
void		addto __P((char *buf, char *str));
int		noteit __P((bool must));
void		obeep __P((void));
void		obeepop __P((int unused_arg));
int		map __P((int c, struct maps *maps));
void		macpush __P((char *st, int canundo));
void		visdump __P((char *s));
void		vudump __P((char *s));
int		vgetcnt __P((void));
int		fastpeekkey __P((void));
void		trapalarm __P((int));

/* ex_vmain.c */
void		vmain __P((void));
void		grabtag __P((void));
void		prepapp __P((void));
void		vremote __P((int cnt, opf_t f, int arg));
void		vsave __P((void));
void		vzop __P((bool hadcnt, int cnt, int c));

/* ex_voper.c */
void		operate __P((int c, int cnt));
int		find __P((int c));
int		word __P((opf_t op, int cnt));
void		eend __P((opf_t op));
int		wordof __P((int which, char *wc));
int		wordch __P((char *wc));
int		edge __P((void));
int		margin __P((void));

/* ex_vops.c */
void		ex_vUndo __P((void));
void		vundo __P((bool show));
void		vmacchng __P((bool fromvis));
void		vnoapp __P((void));
void		vmove __P((void));
void		vmoveop __P((int c));
void		vdelete __P((int c));
void		vchange __P((int c));
void		voOpen __P((int c, int cnt));
void		vshftop __P((int unused_arg));
void		vfilter __P((void));
int		xdw __P((void));
void		vshift __P((int unused_arg));
void		vrep __P((int cnt));
void		vyankit __P((int unused_arg));
void		setpk __P((void));

/* ex_vops2.c */
void		bleep __P((int i, char *cp));
int		vdcMID __P((void));
void		takeout __P((char *BUF));
int		ateopr __P((void));
void		vappend __P((int ch, int cnt, int indent));
void		back1 __P((void));
char *		ex_vgetline __P((int cnt, char *gcursor, bool *aescaped, int commch));
void		vdoappend __P((char *lp));
int		vgetsplit __P((void));
int		vmaxrep __P((int ch, int cnt));

/* ex_vops3.c */
int		xlfind __P((bool pastatom, int cnt, opf_t f, line *limit));
int		endsent __P((bool pastatom));
int		endPS __P((void));
#ifdef LISPCODE
int		lindent __P((line *));
#endif
int		lmatchp __P((line *addr));
void		lsmatch __P((char *cp));
int		ltosolid __P((void));
int		ltosol1 __P((char *parens));
int		lskipbal __P((char *parens));
int		lskipatom __P((void));
int		lskipa1 __P((char *parens));
int		lnext __P((void));
int		lbrack __P((int c, opf_t f));
int		isa __P((char *cp));

/* ex_vput.c */
void		vclear __P((void));
void		vclrbyte __P((char *cp, int i));
void		vclrlin __P((int l, line *tp));
void		vclreol __P((void));
void		vclrech __P((bool didphys));
void		fixech __P((void));
void		vcursbef __P((char *cp));
void		vcursat __P((char *cp));
void		vcursaft __P((char *cp));
void		vfixcurs __P((void));
void		vsetcurs __P((char *nc));
void		vigoto __P((int y, int x));
void		vcsync __P((void));
void		vgotoCL __P((int x));
void		vigotoCL __P((int x));
void		vgoto __P((int y, int x));
void		vgotab __P((void));
void		vprepins __P((void));
void		vmaktop __P((int p, char *cp));
void		vinschar __P((int c));
void		vrigid __P((void));
void		vneedpos __P((int cnt));
void		vnpins __P((int dosync));
void		vishft __P((void));
void		viin __P((int c));
void		godm __P((void));
void		enddm __P((void));
void		goim __P((void));
void		endim __P((void));
void		vputchar __P((int c));
void		physdc __P((int stcol, int endcol));
#if defined(TRACE)
void		tfixnl __P((void));
void		tvliny __P((void));
void		tracec __P((int c));
#endif
void		vputch __P((int c));

/* ex_vwind.c */
void		vmoveto __P((line *addr, char *curs, int context));
void		vjumpto __P((line *addr, char *curs, int context));
void		vupdown __P((int cnt, char *curs));
void		vup __P((int cnt, int ind, bool scroll));
void		vdown __P((int cnt, int ind, bool scroll));
void		vcontext __P((line *addr, int where));
void		vclean __P((void));
void		vshow __P((line *addr, line *top));
void		vreset __P((bool inecho));
line *		vback __P((line *tp, int cnt));
int		vfit __P((line *tp, int cnt));
void		vroll __P((int cnt));
void		vrollR __P((int cnt));
int		vcookit __P((int cnt));
int		vdepth __P((void));
void		vnline __P((char *curs));

/* ex_printf.c */
void		ex_printf __P((const char *fmt0, ...));
void		ex_vprintf __P((const char *fmt0, va_list ap));

#endif	//PROTO_H_INCLUDED

