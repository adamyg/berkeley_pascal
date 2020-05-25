/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
/*-
 * Copyright (c) 1980 The Regents of the University of California.
 * All rights reserved.
 */

#ifndef lint
static char sccsid[] = "@(#)ex_subr.c	7.15 (Berkeley) 4/17/91";
#endif /* not lint */

#include "ex.h"
#include "ex_re.h"
#include "ex_tty.h"
#include "ex_vis.h"
#include "pathnames.h"

/*
 * Random routines, in alphabetical order.
 */

any(c, s)
	int c;
	register char *s;
{
	register int x;

	while (x = *s++)
		if (x == c)
			return (1);
	return (0);
}

backtab(i)
	register int i;
{
	register int j;

	j = i % value(SHIFTWIDTH);
	if (j == 0)
		j = value(SHIFTWIDTH);
	i -= j;
	if (i < 0)
		i = 0;
	return (i);
}

void
change()
{

	tchng++;
	chng = tchng;
}

/*
 * Column returns the number of
 * columns occupied by printing the
 * characters through position cp of the
 * current line.
 */
column(cp)
	register char *cp;
{

	if (cp == 0)
		cp = &linebuf[LBSIZE - 2];
	return (qcolumn(cp, (char *) 0));
}

/*
 * Ignore a comment to the end of the line.
 * This routine eats the trailing newline so don't call newline().
 */
void
comment()
{
	register int c;

	do {
		c = ex_getchar();
	} while (c != '\n' && c != EOF);
	if (c == EOF)
		ungetchar(c);
}

void
Copy(to, from, size)
	register char *from, *to;
	register int size;
{

	if (size > 0)
		do
			*to++ = *from++;
		while (--size > 0);
}

void
copyw(to, from, size)
	register line *from, *to;
	register int size;
{
	if (size > 0)
		do
			*to++ = *from++;
		while (--size > 0);
}

void
copywR(to, from, size)
	register line *from, *to;
	register int size;
{

	while (--size >= 0)
		to[size] = from[size];
}

ctlof(c)
	int c;
{

	return (c == TRIM ? '?' : c | ('A' - 1));
}

void
dingdong()
{

	if (VB)
		putpad(VB);
	else if (value(ERRORBELLS))
		putch('\207');
}

fixindent(indent)
	int indent;
{
	register int i;
	register char *cp;

	i = whitecnt(genbuf);
	cp = vpastwh(genbuf);
	if (*cp == 0 && i == indent && linebuf[0] == 0) {
		genbuf[0] = 0;
		return (i);
	}
	CP(genindent(i), cp);
	return (i);
}

void
filioerr(cp)
	char *cp;
{
	register int oerrno = errno;

	lprintf("\"%s\"", cp);
	errno = oerrno;
	syserror();
}

char *
genindent(indent)
	register int indent;
{
	register char *cp;

	for (cp = genbuf; indent >= value(TABSTOP); indent -= value(TABSTOP))
		*cp++ = '\t';
	for (; indent > 0; indent--)
		*cp++ = ' ';
	return (cp);
}

void
getDOT()
{

	getline(*dot);
}

line *
getmark(c)
	register int c;
{
	register line *addr;
	
	for (addr = one; addr <= dol; addr++)
		if (names[c - 'a'] == (*addr &~ 01)) {
			return (addr);
		}
	return (0);
}

getn(cp)
	register char *cp;
{
	register int i = 0;

	while (isdigit(*cp))
		i = i * 10 + *cp++ - '0';
	if (*cp)
		return (0);
	return (i);
}

void
ignnEOF()
{
	register int c = ex_getchar();

	if (c == EOF)
		ungetchar(c);
	else if (c=='"')
		comment();
}

iswhite(c)
	int c;
{

	return (c == ' ' || c == '\t');
}

junk(c)
	register int c;
{

	if (c && !value(BEAUTIFY))
		return (0);
	if (c >= ' ' && c != TRIM)
		return (0);
	switch (c) {

	case '\t':
	case '\n':
	case '\f':
		return (0);

	default:
		return (1);
	}
}

void
killed()
{

	killcnt(addr2 - addr1 + 1);
}

void
killcnt(cnt)
	register int cnt;
{

	if (inopen) {
		notecnt = cnt;
		notenam = notesgn = "";
		return;
	}
	if (!notable(cnt))
		return;
	ex_printf("%d lines", cnt);
	if (value(TERSE) == 0) {
		ex_printf(" %c%s", Command[0] | ' ', Command + 1);
		if (Command[strlen(Command) - 1] != 'e')
			ex_putchar('e');
		ex_putchar('d');
	}
	putNFL();
}

lineno(a)
	line *a;
{

	return (a - zero);
}

lineDOL()
{

	return (lineno(dol));
}

lineDOT()
{

	return (lineno(dot));
}

void
markDOT()
{

	markpr(dot);
}

void
markpr(which)
	line *which;
{

	if ((inglobal == 0 || inopen) && which <= endcore) {
		names['z'-'a'+1] = *which & ~01;
		if (inopen)
			ncols['z'-'a'+1] = cursor;
	}
}

markreg(c)
	register int c;
{

	if (c == '\'' || c == '`')
		return ('z' + 1);
	if (c >= 'a' && c <= 'z')
		return (c);
	return (0);
}

/*
 * Mesg decodes the terse/verbose strings. Thus
 *	'xxx@yyy' -> 'xxx' if terse, else 'xxx yyy'
 *	'xxx|yyy' -> 'xxx' if terse, else 'yyy'
 * All others map to themselves.
 */
char *
mesg(str)
	register char *str;
{
	register char *cp;

	str = strcpy(genbuf, str);
	for (cp = str; *cp; cp++)
		switch (*cp) {

		case '@':
			if (value(TERSE))
				*cp = 0;
			else
				*cp = ' ';
			break;

		case '|':
			if (value(TERSE) == 0)
				return (cp + 1);
			*cp = 0;
			break;
		}
	return (str);
}

/*VARARGS2*/
void
merror(MSGSTR seekpt, ...)
{
	register char *cp = linebuf;
	va_list ap;

	if (seekpt == 0)
		return;
	merror1(seekpt);
	if (*cp == '\n')
		putnl(), cp++;
	if (inopen > 0 && CE)
		vclreol();
	if (SO && SE)
		putpad(SO);
	va_start(ap, seekpt);
	ex_vprintf(mesg(cp), ap);
	va_end(ap);
	if (SO && SE)
		putpad(SE);
}

void
merror1(MSGSTR seekpt)
{

#ifndef EXSTRINGS
	strcpy(linebuf, seekpt);
#else
	lseek(erfile, (long) seekpt, SEEK_SET);
	if (read(erfile, linebuf, 128) < 2)
		CP(linebuf, "ERROR");
#endif
}

morelines()
{
#ifdef UNIX_SBRK
	char *sbrk();

	if ((int) sbrk(1024 * sizeof (line)) == -1)
		return (-1);
	endcore += 1024;
	return (0);
#else
	/*
	 * We can never be guaranteed that we can get more memory
	 * beyond "endcore".  So we just punt every time.
	 */
	return -1;
#endif
}

void
nonzero()
{

	if (addr1 == zero) {
		notempty();
		error("Nonzero address required@on this command");
	}
}

notable(i)
	int i;
{

	return (hush == 0 && !inglobal && i > value(REPORT));
}


void
notempty()
{

	if (dol == zero)
		error("No lines@in the buffer");
}


void
netchHAD(cnt)
	int cnt;
{

	netchange(lineDOL() - cnt);
}

void
netchange(i)
	register int i;
{
	register char *cp;

	if (i > 0)
		notesgn = cp = "more ";
	else
		notesgn = cp = "fewer ", i = -i;
	if (inopen) {
		notecnt = i;
		notenam = "";
		return;
	}
	if (!notable(i))
		return;
	ex_printf(mesg("%d %slines@in file after %s"), i, cp, Command);
	putNFL();
}

void
putmark(addr)
	line *addr;
{

	putmk1(addr, putline());
}

void
putmk1(addr, n)
	register line *addr;
	int n;
{
	register line *markp;
	register int oldglobmk;

	oldglobmk = *addr & 1;
	*addr &= ~1;
	for (markp = (anymarks ? names : &names['z'-'a'+1]);
	  markp <= &names['z'-'a'+1]; markp++)
		if (*markp == *addr)
			*markp = n;
	*addr = n | oldglobmk;
}

char *
plural(i)
	long i;
{

	return (i == 1 ? "" : "s");
}

short	vcntcol = 0;

qcolumn(lim, gp)
	register char *lim, *gp;
{
	register int x;
	out_t OO;

	OO = Outchar;
	Outchar = qcount;
	vcntcol = 0;
	if (lim != NULL)
		x = lim[1], lim[1] = 0;
	pline(0);
	if (lim != NULL)
		lim[1] = x;
	if (gp)
		while (*gp)
			ex_putchar(*gp++);
	Outchar = OO;
	return (vcntcol);
}

void
qcount(c)
	int c;
{

	if (c == '\t') {
		vcntcol += value(TABSTOP) - vcntcol % value(TABSTOP);
		return;
	}
	vcntcol++;
}

void
reverse(a1, a2)
	register line *a1, *a2;
{
	register line t;

	for (;;) {
		t = *--a2;
		if (a2 <= a1)
			return;
		*a2 = *a1;
		*a1++ = t;
	}
}

void
save(a1, a2)
	line *a1;
	register line *a2;
{
	register int more;

	if (!FIXUNDO)
		return;
#ifdef TRACE
	if (trace)
		vudump("before save");
#endif
	undkind = UNDNONE;
	undadot = dot;
	more = (a2 - a1 + 1) - (unddol - dol);
	while (more > (endcore - truedol))
		if (morelines() < 0)
#ifdef UNIX_SBRK
			error("Out of memory@saving lines for undo - try using ed");
#else
			error("Out of memory@saving lines for undo - try increasing linelimit");
#endif
	if (more)
		(*(more > 0 ? copywR : copyw))(unddol + more + 1, unddol + 1,
		    (truedol - unddol));
	unddol += more;
	truedol += more;
	copyw(dol + 1, a1, a2 - a1 + 1);
	undkind = UNDALL;
	unddel = a1 - 1;
	undap1 = a1;
	undap2 = a2 + 1;
#ifdef TRACE
	if (trace)
		vudump("after save");
#endif
}

void
save12()
{

	save(addr1, addr2);
}

void
saveall()
{

	save(one, dol);
}

span()
{

	return (addr2 - addr1 + 1);
}

void
ex_sync()
{

	chng = 0;
	tchng = 0;
	xchng = 0;
}


skipwh()
{
	register int wh;

	wh = 0;
	while (iswhite(peekchar())) {
		wh++;
		ignchar();
	}
	return (wh);
}

/*VARARGS2*/
void
smerror(MSGSTR seekpt, ...)
{
	va_list ap;

	if (seekpt == 0)
		return;
	merror1(seekpt);
	if (inopen && CE)
		vclreol();
	if (SO && SE)
		putpad(SO);
	va_start(ap, seekpt);
	lvprintf(mesg(linebuf), ap);
	va_end(ap);
	if (SO && SE)
		putpad(SE);
}

char *
strend(cp)
	register char *cp;
{

	while (*cp)
		cp++;
	return (cp);
}

void
strcLIN(dp)
	char *dp;
{

	CP(linebuf, dp);
}

void
syserror()
{
	char *strerror();

	dirtcnt = 0;
	ex_putchar(' ');
	error(strerror(errno));
}

/*
 * Return the column number that results from being in column col and
 * hitting a tab, where tabs are set every ts columns.  Work right for
 * the case where col > COLUMNS, even if ts does not divide COLUMNS.
 */
tabcol(col, ts)
int col, ts;
{
	int offset, result;

	if (col >= COLUMNS) {
		offset = COLUMNS * (col/COLUMNS);
		col -= offset;
	} else
		offset = 0;
	result = col + ts - (col % ts) + offset;
	return (result);
}

char *
vfindcol(i)
	int i;
{
	register char *cp;
	out_t OO = Outchar;

	Outchar = qcount;
	ignore(qcolumn(linebuf - 1, NOSTR));
	for (cp = linebuf; *cp && vcntcol < i; cp++)
		ex_putchar(*cp);
	if (cp != linebuf)
		cp--;
	Outchar = OO;
	return (cp);
}

char *
vskipwh(cp)
	register char *cp;
{

	while (iswhite(*cp) && cp[1])
		cp++;
	return (cp);
}


char *
vpastwh(cp)
	register char *cp;
{

	while (iswhite(*cp))
		cp++;
	return (cp);
}

whitecnt(cp)
	register char *cp;
{
	register int i;

	i = 0;
	for (;;)
		switch (*cp++) {

		case '\t':
			i += value(TABSTOP) - i % value(TABSTOP);
			break;

		case ' ':
			i++;
			break;

		default:
			return (i);
		}
}

#ifdef lint
Ignore(a)
	char *a;
{

	a = a;
}

Ignorf(a)
	int (*a)();
{

	a = a;
}
#endif

void
markit(addr)
	line *addr;
{

	if (addr != dot && addr >= one && addr <= dol)
		markDOT();
}

/*
 * The following code is defensive programming against a bug in the
 * pdp-11 overlay implementation.  Sometimes it goes nuts and asks
 * for an overlay with some garbage number, which generates an emt
 * trap.  This is a less than elegant solution, but it is somewhat
 * better than core dumping and losing your work, leaving your tty
 * in a weird state, etc.
 */
int       _ovno = 0;

#if defined(unix)
void
onemt(int sig)
{
     	(void) sig;

	signal(SIGEMT, onemt);
	/* 2 and 3 are valid on 11/40 type vi, so */
	if (_ovno < 0 || _ovno > 3)
		_ovno = 0;
	error("emt trap, _ovno is %d @ - try again");
}
#endif


/*
 * When a hangup occurs our actions are similar to a preserve
 * command.  If the buffer has not been [Modified], then we do
 * nothing but remove the temporary files and exit.
 * Otherwise, we sync the temp file and then attempt a preserve.
 * If the preserve succeeds, we unlink our temp files.
 * If the preserve fails, we leave the temp files as they are
 * as they are a backup even without preservation if they
 * are not removed.
 */
void
onhup(int sig)
{
	(void) sig;

	/*
	 * USG tty driver can send multiple HUP's!!
	 */
	signal(SIGINT, SIG_IGN);
#if defined(SIGHUP)
	signal(SIGHUP, SIG_IGN);
#endif
	if (chng == 0) {
		cleanup(1);
		ex_exit(0);
	}
	if (setexit() == 0) {
		if (preserve()) {
			cleanup(1);
			ex_exit(0);
		}
	}
	ex_exit(1);
}


/*
 * An interrupt occurred.  Drain any output which
 * is still in the output buffering pipeline.
 * Catch interrupts again.  Unless we are in visual
 * reset the output state (out of -nl mode, e.g).
 * Then like a normal error (with the \n before Interrupt
 * suppressed in visual mode).
 */
void
onintr(int sig)
{
	(void) sig;

#ifndef CBREAK
	signal(SIGINT, onintr);
#else
	signal(SIGINT, inopen ? vintr : onintr);
#endif
#if defined(unix)
	alarm(0);	/* in case we were called from map */
#endif
	draino();
	if (!inopen) {
		pstop();
		setlastchar('\n');
#ifdef CBREAK
	}
#else
	} else
		vraw();
#endif
#ifdef WIN32
	inintr++;
#endif
	error("\nInterrupt" + inopen);
}

/*
 * If we are interruptible, enable interrupts again.
 * In some critical sections we turn interrupts off,
 * but not very often.
 */
void
setrupt()
{

	if (ruptible) {
#ifndef CBREAK
		signal(SIGINT, onintr);
#else
		signal(SIGINT, inopen ? vintr : onintr);
#endif
#ifdef SIGTSTP
		if (dosusp)
			signal(SIGTSTP, onsusp);
#endif
	}
}


#if defined(unix)
preserve()
{
#ifdef VMUNIX
	tflush();
#endif
	synctmp();
	pid = vfork();
	if (pid < 0)
		return (0);
	if (pid == 0) {
		close(0);
		dup(tfile);
		execl(_PATH_EXPRESERVE, "expreserve", (char *) 0);
		ex_exit(1);
	}
	waitfor();
	if (rpid == pid && status == 0)
		return (1);
	return (0);
}
#endif

#ifndef V6
void
ex_exit(i)
	int i;
{

# ifdef TRACE
	if (trace)
		fclose(trace);
# endif
	_exit(i);
}
#endif

#ifdef SIGTSTP
/*
 * We have just gotten a susp.  Suspend and prepare to resume.
 */
void
onsusp()
{
	ttymode f;
	struct winsize win;

	f = setty(normf);
	vnfl();
	putpad(TE);
	flush();

	(void) sigsetmask(0);
	signal(SIGTSTP, SIG_DFL);
	kill(0, SIGTSTP);

	/* the pc stops here */

	signal(SIGTSTP, onsusp);
	vcontin(0);
	ignore(setty(f));
	if (!inopen)
		error((char *) 0);
	else {
#ifdef	TIOCGWINSZ
		if (ioctl(0, TIOCGWINSZ, &win) >= 0)
			if (win.ws_row != winsz.ws_row ||
			    win.ws_col != winsz.ws_col)
				winch();
#endif
		if (vcnt < 0) {
			vcnt = -vcnt;
			if (state == VISUAL)
				vclear();
			else if (state == CRTOPEN)
				vcnt = 0;
		}
		vdirty(0, LINES);
		vrepaint(cursor);
	}
}
#endif
