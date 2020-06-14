/*-
 * Copyright (c) 1982, 1993
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

#if !defined(lint) && defined(SCCSID)
static char sccsid[] = "@(#)library.c	8.2 (Berkeley) 5/27/94";
#endif /* not lint */

/*
 * General purpose routines.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <io.h>
#endif
#include <errno.h>
#include "defs.h"

#define public
#define private         static
#define nil(type)       ((type) 0)

private void            fswap(int oldfd, int newfd);

typedef char            Boolean;

const char *cmdname = NULL;         /* name of command for error messages */
const char *errfilename = NULL;     /* current file associated with error */
short errlineno = 0;                /* line number associated with error */

typedef struct {
    INTFUNC     *func;
} ERRINFO;

#define ERR_IGNORE      ((INTFUNC *) 0)
#define ERR_CATCH       ((INTFUNC *) 1)

/*
 * Call a program.
 *
 * Three entries:
 *
 *      call, callv - call a program and wait for it, returning status
 *      backv - call a program and don't wait, returning process id
 *
 * The command's standard input and output are passed as FILE's.
 */


#define MAXNARGS 100    /* unchecked upper limit on max num of arguments */
#define BADEXEC 127	/* exec fails */

#define ischild(pid)    ((pid) == 0)

/* VARARGS3 */
public int
call(const char * name, FILE *in, FILE *out, ...)
{
    const char *argv[ MAXNARGS ] = {0}, *arg;
    va_list ap;
    int i;

    argv[ 0 ] = name;
    va_start(ap, out);
    for (i = 1; i < MAXNARGS-1 &&
            (arg = va_arg(ap, const char *)) != nil(const char *); i++)
    {
        argv[ i++ ] = arg;
    }
    argv[ i ] = nil(char *);
    return callv(name, in, out, argv);
}

public int
callv(const char *name, FILE *in, FILE *out, const char **argv)
{
    int pid, status;

    pid = backv(name, in, out, argv);
#if defined(unix)
    pwait(pid, &status);
#else
    status = -1;
#endif
    return status;
}

public int
backv(const char *name, FILE *in, FILE *out, const char * *argv)
{
#if defined(unix)
    int pid;

    fflush(stdout);
    if (ischild(pid = fork())) {
        fswap(0, fileno(in));
        fswap(1, fileno(out));
        (void) onsyserr(EACCES, ERR_IGNORE);
        execvp(name, argv);
        _exit(BADEXEC);
    }
    return pid;

#else
    return -1;
#endif
}


/*
 *  Swap file numbers so as to redirect standard input and output.
 */
private void
fswap(int oldfd, int newfd)
{
    if (oldfd != newfd) {
        close(oldfd);
        dup(newfd);
        close(newfd);
    }
}


/*
 * Invoke a shell on a command line.
 */
#define DEF_SHELL       "csh"

public void
shell(const char * s)
{
    char * sh;

    if ((sh = getenv("SHELL")) == nil(char *)) {
        sh = DEF_SHELL;
    }
    call(sh, stdin, stdout, "-c", s, 0);
}


/*
 *  Wait for a process the right way.  We wait for a particular
 *  process and if any others come along in between, we remember them
 *  in case they are eventually waited for.
 *
 *  This routine is not very efficient when the number of processes
 *  to be remembered is large.
 */
#if defined(unix)
typedef struct pidlist {
    int             pid;
    int             status;
    struct pidlist  *next;
} Pidlist;

private Pidlist     *pidlist, *pfind();

public void
pwait(int pid, int *statusp)
{
    Pidlist *p;
    int pnum, status;

    p = pfind(pid);
    if (p != nil(Pidlist *)) {
        *statusp = p->status;
        dispose(p);
        return;
    }
    while ((pnum = wait(&status)) != pid && pnum >= 0) {
        p = alloc(1, Pidlist);
        p->pid = pnum;
        p->status = status;
        p->next = pidlist;
        pidlist = p;
    }
    if (pnum < 0) {
        p = pfind(pid);
        if (p == nil(Pidlist *)) {
            panic("pwait: pid %d not found", pid);
        }
        *statusp = p->status;
        dispose(p);
    } else {
        *statusp = status;
    }
#ifdef tahoe
    chkret(p, status);
#endif
}


/*
 *  Look for the given process id on the pidlist.
 *
 *  Unlink it from list if found.
 */

private Pidlist *
pfind(int pid)
{
    register Pidlist *p, *prev;

    prev = nil(Pidlist *);
    for (p = pidlist; p != nil(Pidlist *); p = p->next) {
        if (p->pid == pid) {
            break;
        }
        prev = p;
    }
    if (p != nil(Pidlist *)) {
        if (prev == nil(Pidlist *)) {
            pidlist = p->next;
        } else {
            prev->next = p->next;
        }
    }
    return p;
}
#endif  /*unix*/


/*
 *  System call error handler.
 *
 *  The syserr routine is called when a system call is about to
 *  set the c-bit to report an error.  Certain errors are caught
 *  and cause the process to print a message and immediately exit.
 */
#if !defined(_MSC_VER) && !defined(__WATCOMC__)
extern int sys_nerr;
extern char *sys_errlist[];
#endif


/*
 * default error handling
 */

private ERRINFO errinfo[] ={  /*FIXME: enums are system dependent*/
/* no error */	ERR_IGNORE,
/* EPERM */	ERR_IGNORE,
/* ENOENT */	ERR_IGNORE,
/* ESRCH */	ERR_IGNORE,
/* EINTR */	ERR_CATCH,
/* EIO */	ERR_CATCH,
/* ENXIO */	ERR_CATCH,
/* E2BIG */	ERR_CATCH,
/* ENOEXEC */	ERR_CATCH,
/* EBADF */	ERR_IGNORE,
/* ECHILD */	ERR_CATCH,
/* EAGAIN */	ERR_CATCH,
/* ENOMEM */	ERR_CATCH,
/* EACCES */	ERR_CATCH,
/* EFAULT */	ERR_CATCH,
/* ENOTBLK */	ERR_CATCH,
/* EBUSY */	ERR_CATCH,
/* EEXIST */	ERR_CATCH,
/* EXDEV */	ERR_CATCH,
/* ENODEV */	ERR_CATCH,
/* ENOTDIR */	ERR_CATCH,
/* EISDIR */	ERR_CATCH,
/* EINVAL */	ERR_CATCH,
/* ENFILE */	ERR_CATCH,
/* EMFILE */	ERR_CATCH,
/* ENOTTY */	ERR_IGNORE,
/* ETXTBSY */	ERR_CATCH,
/* EFBIG */	ERR_CATCH,
/* ENOSPC */	ERR_CATCH,
/* ESPIPE */	ERR_CATCH,
/* EROFS */	ERR_CATCH,
/* EMLINK */	ERR_CATCH,
/* EPIPE */	ERR_CATCH,
/* EDOM */	ERR_CATCH,
/* ERANGE */	ERR_CATCH,
/* EQUOT */	ERR_CATCH,
};

public void
syserr(void)
{
    ERRINFO *e;

    e = &errinfo[errno];
    if (e->func == ERR_CATCH) {
        if (errno < sys_nerr) {
            panic(sys_errlist[errno]);
        } else {
            panic("errno %d", errno);
        }
    } else if (e->func != ERR_IGNORE) {
        (*e->func)();
    }
}

/*
 *  Change the action on receipt of an error.
 */
public INTFUNC *
onsyserr(int n, INTFUNC *f)
{
    INTFUNC *g = errinfo[n].func;

    errinfo[n].func = f;
    return(g);
}

/*
 * Main driver of error message reporting.
 */

/* VARARGS2 */
private void
errmsgv(const char *errname, Boolean shouldquit, const char *s, va_list ap)
{
    fflush(stdout);
    if (shouldquit && cmdname != nil(char *)) {
        fprintf(stderr, "%s: ", cmdname);
    }
    if (errfilename != nil(char *)) {
        fprintf(stderr, "%s: ", errfilename);
    }
    if (errlineno > 0) {
        fprintf(stderr, "%d: ", errlineno);
    }
    if (errname != nil(char *)) {
        fprintf(stderr, "%s: ", errname);
    }
    vfprintf(stderr, s, ap);
    putc('\n', stderr);
    if (shouldquit) {
        quit(1);
    }
}

private void
errmsg(const char *errname, Boolean shouldquit, const char *s, ...)
{
    va_list ap;

    va_start(ap, s);
    errmsgv(errname, shouldquit, s, ap);
}


/*
 *  Errors are a little worse, they mean something is wrong,
 *  but not so bad that processing can't continue.
 *
 *  The routine "erecover" is called to recover from the error,
 *  a default routine is provided that does nothing.
 */

/* VARARGS1 */
public void
errorv(const char *s, va_list ap)
{
    extern void erecover();

    errmsgv(nil(const char *), FALSE, s, ap);
    erecover();
}

public void
error(const char * s, ...)
{
    va_list ap;
    va_start(ap, s);
    errorv(s, ap);
}


/*
 * Non-recoverable user error.
 */

/* VARARGS1 */
public void
fatalv(const char * s, va_list ap)
{
    errmsg("fatal error", TRUE, s, ap);
}


public void
fatal(const char * s, ...)
{
    va_list ap;
    va_start(ap, s);
    fatalv(s, ap);
}


/*
 * Panics indicate an internal program error.
 */

/* VARARGS1 */
public void
panicv(const char * s, va_list ap)
{
    errmsgv("panic", TRUE, s, ap);
}


public void
panic(const char * s, ...)
{
    va_list ap;
    va_start(ap, s);
    panicv(s, ap);
}



/*
 * Compare n-byte areas pointed to by s1 and s2
 * if n is 0 then compare up until one has a null byte.
 */
public int
cmp(const void *p1, const void *p2, unsigned int n)
{
    register const char *s1 = p1;
    register const char *s2 = p2;

    if (s1 == nil(char *) || s2 == nil(char *)) {
        panic("cmp: nil pointer");
    }
    if (n == 0) {
        while (*s1 == *s2++) {
            if (*s1++ == '\0') {
                return(0);
            }
        }
        return(*s1 - *(s2-1));
    } else {
        for (; n != 0; n--) {
            if (*s1++ != *s2++) {
                return(*(s1-1) - *(s2-1));
            }
        }
        return(0);
    }
}

/*
 * Move n bytes from src to dest.
 * If n is 0 move until a null is found.
 */

public void
mov(const void *s, void *d, int n)
{
    register const char *src = s;
    register char *dest = d;

    if (src == nil(char *)) {
        panic("mov: nil source");
    }
    if (dest == nil(char *)) {
        panic("mov: nil destination");
    }
    if (n > 0) {
        for (; n != 0; n--) {
            *dest++ = *src++;
        }
    } else {
        while ((*dest++ = *src++) != '\0');
    }
}

