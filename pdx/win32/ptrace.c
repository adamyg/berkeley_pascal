/* -*- mode: c; tabs: 4; -*- */
/*
 *  Copyright (c) 2020, Adam Young.
 *  Routines for tracing the execution of a process ...
 */

#include "defs.h"
#include "object.h"
#include "main.h"
#include "mappings.h"
#include "process/process.h"
#include "../process/process.rep"
#include "../process/pxinfo.h"
#include "../px/pxvars.h"
#include "../px/px.h"

#include <process.h>

#define WMASK           (~(sizeof(WORD) - 1))
#define cachehash(addr) ((unsigned) ((addr >> 2) % CSIZE))
#define setrep(n)       (1 << ((n)-1))
#define istraced(p)     (p->sigset & setrep(p->signo))

static unsigned __stdcall pxthread(void *arg);
static void             pxhalt(PROCESS *p);
static void             pxcont(PROCESS *p);
static void             pxtrap(int signo, ...);

static WORD             fetch(PROCESS *p, PIO_SEG seg, ADDRESS addr);
static void             store(PROCESS *p, PIO_SEG seg, ADDRESS addr, WORD data);

static DWORD            proctls = -1;


/*
 * Start up a new process by forking and exec-ing the
 * given argument list, returning when the process is loaded
 * and ready to execute.  The PROCESS information (pointed to
 * by the first argument) is appropriately filled.
 *
 * If the given PROCESS structure is associated with an already running
 * process, we terminate it.
 */

void
pstart(PROCESS *p, const char *cmd, const char **argv, const char *infile, const char *outfile)
{
    unsigned thandle;

    if (p->pid != 0) {                          /* child already running? */
        p->pid = 0;
        pxcont(p);                              /* restart and exit */
        if (p->status != FINISHED)
            panic("interpreter process not finished");
        PCADDR = 0;                             /* interpreter */
    }

    /* process resources */
    if (proctls == -1) {                        /* thread local storage */
        proctls = TlsAlloc();
    }

    if (0 == p->sevent) {                       /* status and task events */
        p->sevent = CreateEvent(NULL, FALSE, FALSE, NULL);
        p->tevent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    /* arguments */
    p->argc = 0;
    p->argv = argv;
    while (p->argv[p->argc]) {
        ++p->argc;
    }

    /* start interpreter thread */
    px_settrap(pxtrap);
//  px_setio(infile, outfile);                  /* TODO: set i/o */

    p->tid = (HANDLE)_beginthreadex(NULL, 0,
                pxthread, (void *)p, CREATE_SUSPENDED, &thandle);
    if (p->tid == 0) {
        p->status = FINISHED;
    } else {
        p->pid = 1;                             /* process id */
        p->status = STARTING;                   /* start */
        pxcont(p);
    }
}


static unsigned __stdcall
pxthread(void *arg)
{
    PROCESS *p = (PROCESS *)arg;

    TlsSetValue(proctls, (LPVOID)p);            /* set thread local data */

    pxhalt(p);                                  /* start halt */

    if (p->pid) {
        p->exitval = px_main(p->argc, (char **)p->argv);
    }

    p->status = FINISHED;
    p->pid = 0;

    SetEvent(p->sevent);                        /* signal status change */
    return 0;
}


static void
pxhalt(PROCESS *p)
{
    if (p->status != RUNNING)
        panic("pxhalt() interpreter process not running");

    p->status = STOPPED;                        /* halt and wait */

    SignalObjectAndWait(p->sevent, p->tevent, INFINITE, FALSE);
//  SetEvent( p->sevent );
//  WaitForSingleObject( p->tevent, INFINITE );
}  


static void
pxcont(PROCESS *p)
{
    if (p->status != FINISHED) {
        if (p->status == STARTING) {
            p->status = RUNNING;

            ResumeThread(p->tid);               /* start and await halt */
            WaitForSingleObject(p->sevent, INFINITE);
            if (p->status == RUNNING)
                panic("interpreter process still running");

        } else {
            if (p->status != STOPPED) {
                panic("pxcont() interpreter process not stopped");
            }
            p->status = RUNNING;                           
                                                /* start and await halt */
            SignalObjectAndWait(p->tevent, p->sevent, INFINITE, FALSE);
//          SetEvent(p->tevent);
//          WaitForSingleObject(p->sevent, INFINITE);
        }

        if (p->status == RUNNING) {
            panic("pxcont() interpreter process still running");
        }
    }
}  


static void
pxtrap(int signo, ...)
{
    PROCESS *p;

    p = TlsGetValue( proctls );                 /* retrieve process */

    if ((p->signo = signo) == PXSIGIOT) {
        va_list ap;

        va_start(ap, signo);
        p->targs.disp = va_arg(ap,ADDRESS);
        p->targs.dp = va_arg(ap,ADDRESS);
        p->targs.objstart = va_arg(ap,ADDRESS); /* base address of 'obj' */
        p->targs.pcaddr = va_arg(ap,ADDRESS);   /* address of 'pc' storage */
        p->targs.loopaddr = va_arg(ap,ADDRESS);
        va_end(ap);
    }

    if (p->sigset) {
        pxhalt(p);                              /* halt and wait */
        if (p->pid == 0)
            px_exit(0);                         /* terminate process */
    }
}


void
ptrapargs(PROCESS *p, TRAPARGS *t)
{
    *t = p->targs;                              /* trap arguments */
}


/*
 * Continue a stopped process.  The argument points to a PROCESS structure.
 * Before the process is restarted it's user area is modified according to
 * the values in the structure.  When this routine finishes,
 * the structure has the new values from the process's user area.
 *
 * pcont terminates when the process stops with a signal pending that
 * is being traced (via psigtrace), or when the process terminates.
 */

void
pcont(PROCESS *p)
{
    pxcont(p);

    if (p->status == STOPPED && PCADDR) {
        dread(&p->ipc, PCADDR, sizeof(p->ipc)); /* get pcode pc */
        p->ipc -= ENDOFF;                       /* relative to ENDOFF */

                                                /* get pcode sp */
        dread(&p->isp, PCADDR+sizeof(p->ipc), sizeof(p->isp));
    }
}


void
pcont2(PROCESS *p, ADDRESS *pc)
{
    if (option('e')) {
        printf("execution resumes at pc 0x%lx, sp 0x%lx\n",
            (long)pc, (long)p->isp);
        fflush(stdout);
    }

    pcont(p);
    if (p->status == STOPPED) {
        *pc = p->ipc;                           /* pcode program counter */
    }

    if (option('e')) {
        printf("execution stops at pc 0x%lx, sp 0x%lx on sig %d\n",
            (long)pc, (long)p->isp, p->signo);
        fflush(stdout);
    }
}


/*
 * Structure for reading and writing by words, but dealing with bytes.
 */

typedef union {
    WORD pword;
    BYTE pbyte[sizeof(WORD)];
} WORDUNION;


/*
 * Read (write) from (to) the process' address space.  We must deal with
 * inability to look anywhere other than at a word boundary.
 */

void
pio(PROCESS *p, PIO_OP op, PIO_SEG seg, char *buff, ADDRESS addr, int nbytes)
{
    register int i, k;
    register ADDRESS newaddr;
    register char *cp;
    char *bufend;
    WORDUNION w;
    ADDRESS wordaddr;
    int byteoff;

    if (p->status != STOPPED) {
        error("program is not active");
    }
    cp = buff;
    newaddr = addr;
    wordaddr = (newaddr&WMASK);
    if (wordaddr != newaddr) {
        w.pword = fetch(p, seg, wordaddr);
        for (i = newaddr - wordaddr; i < sizeof(WORD) && nbytes>0; i++) {
            if (op == PREAD) {
                *cp++ = w.pbyte[i];
            } else {
                w.pbyte[i] = *cp++;
            }
            nbytes--;
        }
        if (op == PWRITE) {
            store(p, seg, wordaddr, w.pword);
        }
        newaddr = wordaddr + sizeof(WORD);
    }
    byteoff = (nbytes&(~WMASK));
    nbytes -= byteoff;
    bufend = cp + nbytes;
    while (cp < bufend) {
        if (op == PREAD) {
            w.pword = fetch(p, seg, newaddr);
            for (k = 0; k < sizeof(WORD); k++) {
                *cp++ = w.pbyte[k];
            }
        } else {
            for (k = 0; k < sizeof(WORD); k++) {
                w.pbyte[k] = *cp++;
            }
            store(p, seg, newaddr, w.pword);
        }
        newaddr += sizeof(WORD);
    }
    if (byteoff > 0) {
        w.pword = fetch(p, seg, newaddr);
        for (i = 0; i < byteoff; i++) {
            if (op == PREAD) {
                *cp++ = w.pbyte[i];
            } else {
                w.pbyte[i] = *cp++;
            }
        }
        if (op == PWRITE) {
            store(p, seg, newaddr, w.pword);
        }
    }
}


/*
 * Get a word from a process at the given address.
 * The address is assumed to be on a word boundary.
 *
 * We use a simple cache scheme to avoid redundant references to
 * the instruction space (which is assumed to be pure).  In the
 * case of px, the "instruction" space lies between ENDOFF and
 * ENDOFF + objsize.
 *
 * It is necessary to use a write-through scheme so that
 * breakpoints right next to each other don't interfere.
 */

static WORD
fetch(PROCESS *p, PIO_SEG seg, ADDRESS addr)
{
    register CACHEWORD *wp;
    WORD w;

    switch (seg) {
    case TEXTSEG:
        if (addr >= (ADDRESS)objsize) {
            panic("tried to fetch outside px i-space (%u)", addr);
        }
        addr += ENDOFF;
        wp = &p->word[cachehash(addr)];
        if (addr == 0 || wp->addr != addr) {
            memcpy( &w, (const void *)addr, sizeof(WORD) );
            wp->addr = addr;
            wp->val = w;
        } else {
            w = wp->val;
        }
        break;

    case DATASEG:
        memcpy( &w, (const void *)addr, sizeof(WORD) );
        break;

    default:
        panic("fetch: bad seg %d", seg);
        /*NOTREACHED*/
    }
    return(w);
}


/*
 * Put a word into the process' address space at the given address.
 * The address is assumed to be on a word boundary.
 */

LOCAL void
store(PROCESS *p, PIO_SEG seg, ADDRESS addr, WORD data)
{
    register CACHEWORD *wp;

    switch (seg) {
    case TEXTSEG:
        if (addr >= (ADDRESS)objsize) {
            panic("tried to store outside px i-space (%u)", addr);
        }
        addr += ENDOFF;
        wp = &p->word[cachehash(addr)];
        wp->addr = addr;
        wp->val = data;
        memcpy( (void *)addr, (const void *)&data, sizeof(WORD) );
        break;

    case DATASEG:
        memcpy( (void *)addr, (const void *)&data, sizeof(WORD) );
        break;

    default:
        panic("store: bad seg %d", seg);
        /*NOTREACHED*/
    }
}


/*
 * Initialize the instruction cache for a process.
 * This is particularly necessary after the program has been remade.
 */

void
initcache(PROCESS *p)
{
    register int i;

    for (i = 0; i < CSIZE; i++) {
        p->word[i].addr = 0;
    }
}


/*
 * Return from execution when the given signal is pending.
 */

void
psigtrace(PROCESS *p, int sig, int sw)
{
    if (sw) {
        p->sigset |= setrep(sig);
    } else {
        p->sigset &= ~setrep(sig);
    }
}


/*
 * Don't catch any signals.
 * Particularly useful when letting a process finish uninhibited (i.e. px).
 */

void
unsetsigtraces(PROCESS *p)
{
    p->sigset = 0;
}

