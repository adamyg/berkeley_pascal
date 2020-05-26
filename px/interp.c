/*-
 * Copyright (c) 1991, 1993
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

#ifndef lint
static char sccsid[] = "@(#)interp.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

#include <math.h>
#include <signal.h>
#include "whoami.h"
#include "vars.h"
#include "objfmt.h"
#include "h02opcs.h"
#include "machdep.h"
#include "libpc.h"

/*
 * program variables
 */
union display _display;
struct dispsave	*_dp;
long	_lino = 0;
int	_argc;
char	**_argv;
long	_mode;
long	_runtst = (long)TRUE;
bool	_nodump = FALSE;
long	_stlim = 500000;
long	_stcnt = 0;
long	_seed = 1;
#ifdef ADDR32
char	*_minptr = (char *)0x7fffffff;
#endif ADDR32
#ifdef ADDR16
char	*_minptr = (char *)0xffff;
#endif ADDR16
char	*_maxptr = (char *)0;
long	*_pcpcount = (long *)0;
long	_cntrs = 0;
long	_rtns = 0;

/*
 * standard files
 */
char		_inwin, _outwin, _errwin;
struct iorechd	_err = {
	&_errwin,		/* fileptr */
	0,			/* lcount  */
	0x7fffffff,		/* llimit  */
	stderr,			/* fbuf    */
	FILNIL,			/* fchain  */
	STDLVL,			/* flev    */
	"Message file",		/* pfname  */
	FTEXT | FWRITE | EOFF,	/* funit   */
	2,			/* fblk    */
	1			/* fsize   */
};
struct iorechd	output = {
	&_outwin,		/* fileptr */
	0,			/* lcount  */
	0x7fffffff,		/* llimit  */
	stdout,			/* fbuf    */
	ERR,			/* fchain  */
	STDLVL,			/* flev    */
	"standard output",	/* pfname  */
	FTEXT | FWRITE | EOFF,	/* funit   */
	1,			/* fblk    */
	1			/* fsize   */
};
struct iorechd	input = {
	&_inwin,		/* fileptr */
	0,			/* lcount  */
	0x7fffffff,		/* llimit  */
	stdin,			/* fbuf    */
	OUTPUT,			/* fchain  */
	STDLVL,			/* flev    */
	"standard input",	/* pfname  */
	FTEXT|FREAD|SYNC|EOLN,	/* funit   */
	0,			/* fblk    */
	1			/* fsize   */
};

/*
 * file record variables
 */
long		_filefre = PREDEF;
struct iorechd	_fchain = {
	0, 0, 0, 0,		/* only use fchain field */
	INPUT			/* fchain  */
};
struct iorec	*_actfile[MAXFILES] = {
	INPUT,
	OUTPUT,
	ERR
};

/*
 * stuff for pdx to watch what the interpreter is doing.
 * The .globl is #ifndef DBX since it breaks DBX to have a global
 * asm label in the middle of a function (see _loopaddr: below).
 */

union progcntr pdx_pc;
#ifndef DBX
asm(".globl _loopaddr");
#endif DBX

/*
 * Px profile array
 */
#ifdef PROFILE
long _profcnts[NUMOPS];
#endif PROFILE

/*
 * debugging variables
 */
#ifdef PXDEBUG
char opc[10];
long opcptr = 9;
#endif PXDEBUG

void
interpreter(base)
	char *base;
{
	/* register */ union progcntr pc;	/* interpreted program cntr */
	struct iorec *curfile;		/* active file */
	register struct blockmark *stp;	/* active stack frame ptr */
	/*
	 * the following variables are used as scratch
	 */
	register char *tcp;
	register short *tsp;
	register long tl, tl1, tl2, tl3;
	char *tcp2;
	long tl4;
	double td, td1;
	struct sze8 t8;
	register short *tsp1;
	long *tlp;
	char *tcp1;
	bool tb;
	struct blockmark *tstp;
	register struct formalrtn *tfp;
	struct iorec **ip;
	int mypid;
	int ti, ti2;
	short ts;
	FILE *tf;
	/* register */ union progcntr stack;	/* Interpreted stack */

	mypid = getpid();

	/*
	 * Setup sets up any hardware specific parameters before
	 * starting the interpreter. Typically this is macro- or inline-
	 * replaced by "machdep.h" or interp.sed.
	 */
	setup();
	/*
	 * necessary only on systems which do not initialize
	 * memory to zero
	 */
	for (ip = &_actfile[3]; ip < &_actfile[MAXFILES]; *ip++ = FILNIL)
		/* void */;
	/*
	 * set up global environment, then ``call'' the main program
	 */
	STACKALIGN(tl, 2 * sizeof(struct iorec *));
	_display.frame[0].locvars = pushsp(tl);
	_display.frame[0].locvars += 2 * sizeof(struct iorec *);
	*(struct iorec **)(_display.frame[0].locvars + OUTPUT_OFF) = OUTPUT;
	*(struct iorec **)(_display.frame[0].locvars + INPUT_OFF) = INPUT;
	STACKALIGN(tl, sizeof(struct blockmark));
	stp = (struct blockmark *)pushsp(tl);
	_dp = &_display.frame[0];
	pc.cp = base;

	for(;;) {
#		ifdef PXDEBUG
		if (++opcptr == 10)
			opcptr = 0;
		opc[opcptr] = *pc.ucp;
#		endif PXDEBUG
#		ifdef PROFILE
		_profcnts[*pc.ucp]++;
#		endif PROFILE

		/*
		 * Save away the program counter to a fixed location for pdx.
		 */
		pdx_pc = pc;

		/*
		 * Having the label below makes dbx not work
		 * to debug this interpreter,
		 * since it thinks a new function called loopaddr()
		 * has started here, and it won't display the local
		 * variables of interpreter().  You have to compile
		 * -DDBX to avoid this problem...
		 */
#		ifndef DBX
	;asm("_loopaddr:");
#		endif DBX

		switch (*pc.ucp++) {
		case O_BPT:			/* breakpoint trap */
			PFLUSH();
			kill(mypid, SIGILL);
			pc.ucp--;
			continue;
		case O_NODUMP:
			_nodump = TRUE;
			/* and fall through */
		case O_BEG:
			_dp += 1;		/* enter local scope */
			stp->odisp = *_dp;	/* save old display value */
			tl = *pc.ucp++;		/* tl = name size */
			stp->entry = pc.hdrp;	/* pointer to entry info */
			tl1 = pc.hdrp->framesze;/* tl1 = size of frame */
			_lino = pc.hdrp->offset;
			_runtst = pc.hdrp->tests;
			disableovrflo();
			if (_runtst)
				enableovrflo();
			pc.cp += (int)tl;	/* skip over proc hdr info */
			stp->file = curfile;	/* save active file */
			STACKALIGN(tl2, tl1);
			tcp = pushsp(tl2);	/* tcp = new top of stack */
			if (_runtst)		/* zero stack frame */
				blkclr(tcp, tl1);
			tcp += (int)tl1;	/* offsets of locals are neg */
			_dp->locvars = tcp;	/* set new display pointer */
			_dp->stp = stp;
			stp->tos = pushsp((long)0); /* set tos pointer */
			continue;
		case O_END:
			PCLOSE(_dp->locvars);	/* flush & close local files */
			stp = _dp->stp;
			curfile = stp->file;	/* restore old active file */
			*_dp = stp->odisp;	/* restore old display entry */
			if (_dp == &_display.frame[1])
				return;		/* exiting main proc ??? */
			_lino = stp->lino;	/* restore lino, pc, dp */
			pc.cp = stp->pc;
			_dp = stp->dp;
			_runtst = stp->entry->tests;
			disableovrflo();
			if (_runtst)
				enableovrflo();
			STACKALIGN(tl, stp->entry->framesze);
			STACKALIGN(tl1, sizeof(struct blockmark));
			popsp(tl +		/* pop local vars */
			     tl1 +		/* pop stack frame */
			     stp->entry->nargs);/* pop parms */
			continue;
		case O_CALL:
			tl = *pc.cp++;
			PCLONGVAL(tl1);
			tcp = base + tl1 + sizeof(short);/* new entry point */
			GETLONGVAL(tl1, tcp);
			tcp = base + tl1;
			STACKALIGN(tl1, sizeof(struct blockmark));
			stp = (struct blockmark *)pushsp(tl1);
			stp->lino = _lino;	/* save lino, pc, dp */
			stp->pc = pc.cp;
			stp->dp = _dp;
			_dp = &_display.frame[tl]; /* set up new display ptr */
			pc.cp = tcp;
			continue;
		case O_FCALL:
			pc.cp++;
 			tcp = popaddr(); /* ptr to display save area */
			tfp = (struct formalrtn *)popaddr();
			STACKALIGN(tl, sizeof(struct blockmark));
			stp = (struct blockmark *)pushsp(tl);
			stp->lino = _lino;	/* save lino, pc, dp */
			stp->pc = pc.cp;
			stp->dp = _dp;
			pc.cp = (char *)(tfp->fentryaddr);/* new entry point */
			_dp = &_display.frame[tfp->fbn];/* new display ptr */
 			blkcpy(&_display.frame[1], tcp,
				tfp->fbn * sizeof(struct dispsave));
			blkcpy(&tfp->fdisp[0], &_display.frame[1],
				tfp->fbn * sizeof(struct dispsave));
			continue;
		case O_FRTN:
			tl = *pc.cp++;		/* tl = size of return obj */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = pushsp((long)(0));
			tfp = *(struct formalrtn **)(tcp + tl);
 			tcp1 = *(char **)
 			    (tcp + tl + sizeof(struct formalrtn *));
			if (tl != 0) {
				blkcpy(tcp, tcp + sizeof(struct formalrtn *)
				    + sizeof(char *), tl);
			}
			STACKALIGN(tl,
				sizeof(struct formalrtn *) + sizeof (char *));
 			popsp(tl);
 			blkcpy(tcp1, &_display.frame[1],
			    tfp->fbn * sizeof(struct dispsave));
			continue;
		case O_FSAV:
			tfp = (struct formalrtn *)popaddr();
			tfp->fbn = *pc.cp++;	/* blk number of routine */
			PCLONGVAL(tl);
			tcp = base + tl + sizeof(short);/* new entry point */
			GETLONGVAL(tl, tcp);
			tfp->fentryaddr = (long (*)())(base + tl);
			blkcpy(&_display.frame[1], &tfp->fdisp[0],
				tfp->fbn * sizeof(struct dispsave));
			pushaddr(tfp);
			continue;
		case O_SDUP2:
			pc.cp++;
			tl = pop2();
			push2((short)(tl));
			push2((short)(tl));
			continue;
		case O_SDUP4:
			pc.cp++;
			tl = pop4();
			push4(tl);
			push4(tl);
			continue;
		case O_TRA:
			pc.cp++;
			pc.cp += *pc.sp;
			continue;
		case O_TRA4:
			pc.cp++;
			PCLONGVAL(tl);
			pc.cp = base + tl;
			continue;
		case O_GOTO:
			tstp = _display.frame[*pc.cp++].stp; /* ptr to
								exit frame */
			PCLONGVAL(tl);
			pc.cp = base + tl;
			stp = _dp->stp;
			while (tstp != stp) {
				if (_dp == &_display.frame[1])
					ERROR("Active frame not found in non-local goto\n", 0); /* exiting prog ??? */
				PCLOSE(_dp->locvars); /* close local files */
				curfile = stp->file;  /* restore active file */
				*_dp = stp->odisp;    /* old display entry */
				_dp = stp->dp;	      /* restore dp */
				stp = _dp->stp;
			}
			/* pop locals, stack frame, parms, and return values */
			popsp((long)(stp->tos - pushsp((long)(0))));
			continue;
		case O_LINO:
			if (_dp->stp->tos != pushsp((long)(0)))
				ERROR("Panic: stack not empty between statements\n");
			_lino = *pc.cp++;	/* set line number */
			if (_lino == 0)
				_lino = *pc.sp++;
			if (_runtst) {
				LINO();		/* inc statement count */
				continue;
			}
			_stcnt++;
			continue;
		case O_PUSH:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			STACKALIGN(tl1, -tl);
			tcp = pushsp(tl1);
			if (_runtst)
				blkclr(tcp, tl1);
			continue;
		case O_IF:
			pc.cp++;
			if (pop2()) {
				pc.sp++;
				continue;
			}
			pc.cp += *pc.sp;
			continue;
		case O_REL2:
			tl = pop2();
			tl1 = pop2();
			goto cmplong;
		case O_REL24:
			tl = pop2();
			tl1 = pop4();
			goto cmplong;
		case O_REL42:
			tl = pop4();
			tl1 = pop2();
			goto cmplong;
		case O_REL4:
			tl = pop4();
			tl1 = pop4();
		cmplong:
			switch (*pc.cp++) {
			case releq:
				push2(tl1 == tl);
				continue;
			case relne:
				push2(tl1 != tl);
				continue;
			case rellt:
				push2(tl1 < tl);
				continue;
			case relgt:
				push2(tl1 > tl);
				continue;
			case relle:
				push2(tl1 <= tl);
				continue;
			case relge:
				push2(tl1 >= tl);
				continue;
			default:
				ERROR("Panic: bad relation %d to REL4*\n",
				    *(pc.cp - 1));
				continue;
			}
		case O_RELG:
			tl2 = *pc.cp++;		/* tc has jump opcode */
			tl = *pc.usp++;		/* tl has comparison length */
			STACKALIGN(tl1, tl);	/* tl1 has arg stack length */
			tcp = pushsp((long)(0));/* tcp pts to first arg */
			switch (tl2) {
			case releq:
				tb = RELEQ(tl, tcp + tl1, tcp);
				break;
			case relne:
				tb = RELNE(tl, tcp + tl1, tcp);
				break;
			case rellt:
				tb = RELSLT(tl, tcp + tl1, tcp);
				break;
			case relgt:
				tb = RELSGT(tl, tcp + tl1, tcp);
				break;
			case relle:
				tb = RELSLE(tl, tcp + tl1, tcp);
				break;
			case relge:
				tb = RELSGE(tl, tcp + tl1, tcp);
				break;
			default:
				ERROR("Panic: bad relation %d to RELG*\n", tl2);
				break;
			}
			popsp(tl1 << 1);
			push2((short)(tb));
			continue;
		case O_RELT:
			tl2 = *pc.cp++;		/* tc has jump opcode */
			tl1 = *pc.usp++;	/* tl1 has comparison length */
			tcp = pushsp((long)(0));/* tcp pts to first arg */
			switch (tl2) {
			case releq:
				tb = RELEQ(tl1, tcp + tl1, tcp);
				break;
			case relne:
				tb = RELNE(tl1, tcp + tl1, tcp);
				break;
			case rellt:
				tb = RELTLT(tl1, tcp + tl1, tcp);
				break;
			case relgt:
				tb = RELTGT(tl1, tcp + tl1, tcp);
				break;
			case relle:
				tb = RELTLE(tl1, tcp + tl1, tcp);
				break;
			case relge:
				tb = RELTGE(tl1, tcp + tl1, tcp);
				break;
			default:
				ERROR("Panic: bad relation %d to RELT*\n", tl2);
				break;
			}
			STACKALIGN(tl, tl1);
			popsp(tl << 1);
			push2((short)(tb));
			continue;
		case O_REL28:
			td = pop2();
			td1 = pop8();
			goto cmpdbl;
		case O_REL48:
			td = pop4();
			td1 = pop8();
			goto cmpdbl;
		case O_REL82:
			td = pop8();
			td1 = pop2();
			goto cmpdbl;
		case O_REL84:
			td = pop8();
			td1 = pop4();
			goto cmpdbl;
		case O_REL8:
			td = pop8();
			td1 = pop8();
		cmpdbl:
			switch (*pc.cp++) {
			case releq:
				push2(td1 == td);
				continue;
			case relne:
				push2(td1 != td);
				continue;
			case rellt:
				push2(td1 < td);
				continue;
			case relgt:
				push2(td1 > td);
				continue;
			case relle:
				push2(td1 <= td);
				continue;
			case relge:
				push2(td1 >= td);
				continue;
			default:
				ERROR("Panic: bad relation %d to REL8*\n",
				    *(pc.cp - 1));
				continue;
			}
		case O_AND:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push2(tl1 & tl);
			continue;
		case O_OR:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push2(tl1 | tl);
			continue;
		case O_NOT:
			pc.cp++;
			tl = pop2();
			push2(tl ^ 1);
			continue;
		case O_AS2:
			pc.cp++;
			tl = pop2();
			*(short *)popaddr() = tl;
			continue;
		case O_AS4:
			pc.cp++;
			tl = pop4();
			*(long *)popaddr() = tl;
			continue;
		case O_AS24:
			pc.cp++;
			tl = pop2();
			*(long *)popaddr() = tl;
			continue;
		case O_AS42:
			pc.cp++;
			tl = pop4();
			*(short *)popaddr() = tl;
			continue;
		case O_AS21:
			pc.cp++;
			tl = pop2();
			*popaddr() = tl;
			continue;
		case O_AS41:
			pc.cp++;
			tl = pop4();
			*popaddr() = tl;
			continue;
		case O_AS28:
			pc.cp++;
			tl = pop2();
			*(double *)popaddr() = tl;
			continue;
		case O_AS48:
			pc.cp++;
			tl = pop4();
			*(double *)popaddr() = tl;
			continue;
		case O_AS8:
			pc.cp++;
			t8 = popsze8();
			*(struct sze8 *)popaddr() = t8;
			continue;
		case O_AS:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			STACKALIGN(tl1, tl);
			tcp = pushsp((long)(0));
			blkcpy(tcp, *(char **)(tcp + tl1), tl);
			popsp(tl1 + sizeof(char *));
			continue;
		case O_VAS:
			pc.cp++;
			tl = pop4();
			tcp1 = popaddr();
			tcp = popaddr();
			blkcpy(tcp1, tcp, tl);
			continue;
		case O_INX2P2:
			tl = *pc.cp++;		/* tl has shift amount */
			tl1 = pop2();
			tl1 = (tl1 - *pc.sp++) << tl;
			tcp = popaddr();
			pushaddr(tcp + tl1);
			continue;
		case O_INX4P2:
			tl = *pc.cp++;		/* tl has shift amount */
			tl1 = pop4();
			tl1 = (tl1 - *pc.sp++) << tl;
			tcp = popaddr();
			pushaddr(tcp + tl1);
			continue;
		case O_INX2:
			tl = *pc.cp++;		/* tl has element size */
			if (tl == 0)
				tl = *pc.usp++;
			tl1 = pop2();		/* index */
			tl2 = *pc.sp++;
			tcp = popaddr();
			pushaddr(tcp + (tl1 - tl2) * tl);
			tl = *pc.usp++;
			if (_runtst)
				SUBSC(tl1, tl2, tl); /* range check */
			continue;
		case O_INX4:
			tl = *pc.cp++;		/* tl has element size */
			if (tl == 0)
				tl = *pc.usp++;
			tl1 = pop4();		/* index */
			tl2 = *pc.sp++;
			tcp = popaddr();
			pushaddr(tcp + (tl1 - tl2) * tl);
			tl = *pc.usp++;
			if (_runtst)
				SUBSC(tl1, tl2, tl); /* range check */
			continue;
		case O_VINX2:
			pc.cp++;
			tl = pop2();		/* tl has element size */
			tl1 = pop2();		/* upper bound */
			tl2 = pop2();		/* lower bound */
			tl3 = pop2();		/* index */
			tcp = popaddr();
			pushaddr(tcp + (tl3 - tl2) * tl);
			if (_runtst)
				SUBSC(tl3, tl2, tl1); /* range check */
			continue;
		case O_VINX24:
			pc.cp++;
			tl = pop2();		/* tl has element size */
			tl1 = pop2();		/* upper bound */
			tl2 = pop2();		/* lower bound */
			tl3 = pop4();		/* index */
			tcp = popaddr();
			pushaddr(tcp + (tl3 - tl2) * tl);
			if (_runtst)
				SUBSC(tl3, tl2, tl1); /* range check */
			continue;
		case O_VINX42:
			pc.cp++;
			tl = pop4();		/* tl has element size */
			tl1 = pop4();		/* upper bound */
			tl2 = pop4();		/* lower bound */
			tl3 = pop2();		/* index */
			tcp = popaddr();
			pushaddr(tcp + (tl3 - tl2) * tl);
			if (_runtst)
				SUBSC(tl3, tl2, tl1); /* range check */
			continue;
		case O_VINX4:
			pc.cp++;
			tl = pop4();		/* tl has element size */
			tl1 = pop4();		/* upper bound */
			tl2 = pop4();		/* lower bound */
			tl3 = pop4();		/* index */
			tcp = popaddr();
			pushaddr(tcp + (tl3 - tl2) * tl);
			if (_runtst)
				SUBSC(tl3, tl2, tl1); /* range check */
			continue;
		case O_OFF:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			tcp = popaddr();
			pushaddr(tcp + tl);
			continue;
		case O_NIL:
			pc.cp++;
			tcp = popaddr();
			NIL(tcp);
			pushaddr(tcp);
			continue;
		case O_ADD2:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push4(tl1 + tl);
			continue;
		case O_ADD4:
			pc.cp++;
			tl = pop4();
			tl1 = pop4();
			push4(tl1 + tl);
			continue;
		case O_ADD24:
			pc.cp++;
			tl = pop2();
			tl1 = pop4();
			push4(tl1 + tl);
			continue;
		case O_ADD42:
			pc.cp++;
			tl = pop4();
			tl1 = pop2();
			push4(tl1 + tl);
			continue;
		case O_ADD28:
			pc.cp++;
			tl = pop2();
			td = pop8();
			push8(td + tl);
			continue;
		case O_ADD48:
			pc.cp++;
			tl = pop4();
			td = pop8();
			push8(td + tl);
			continue;
		case O_ADD82:
			pc.cp++;
			td = pop8();
			td1 = pop2();
			push8(td1 + td);
			continue;
		case O_ADD84:
			pc.cp++;
			td = pop8();
			td1 = pop4();
			push8(td1 + td);
			continue;
		case O_SUB2:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push4(tl1 - tl);
			continue;
		case O_SUB4:
			pc.cp++;
			tl = pop4();
			tl1 = pop4();
			push4(tl1 - tl);
			continue;
		case O_SUB24:
			pc.cp++;
			tl = pop2();
			tl1 = pop4();
			push4(tl1 - tl);
			continue;
		case O_SUB42:
			pc.cp++;
			tl = pop4();
			tl1 = pop2();
			push4(tl1 - tl);
			continue;
		case O_SUB28:
			pc.cp++;
			tl = pop2();
			td = pop8();
			push8(td - tl);
			continue;
		case O_SUB48:
			pc.cp++;
			tl = pop4();
			td = pop8();
			push8(td - tl);
			continue;
		case O_SUB82:
			pc.cp++;
			td = pop8();
			td1 = pop2();
			push8(td1 - td);
			continue;
		case O_SUB84:
			pc.cp++;
			td = pop8();
			td1 = pop4();
			push8(td1 - td);
			continue;
		case O_MUL2:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push4(tl1 * tl);
			continue;
		case O_MUL4:
			pc.cp++;
			tl = pop4();
			tl1 = pop4();
			push4(tl1 * tl);
			continue;
		case O_MUL24:
			pc.cp++;
			tl = pop2();
			tl1 = pop4();
			push4(tl1 * tl);
			continue;
		case O_MUL42:
			pc.cp++;
			tl = pop4();
			tl1 = pop2();
			push4(tl1 * tl);
			continue;
		case O_MUL28:
			pc.cp++;
			tl = pop2();
			td = pop8();
			push8(td * tl);
			continue;
		case O_MUL48:
			pc.cp++;
			tl = pop4();
			td = pop8();
			push8(td * tl);
			continue;
		case O_MUL82:
			pc.cp++;
			td = pop8();
			td1 = pop2();
			push8(td1 * td);
			continue;
		case O_MUL84:
			pc.cp++;
			td = pop8();
			td1 = pop4();
			push8(td1 * td);
			continue;
		case O_ABS2:
		case O_ABS4:
			pc.cp++;
			tl = pop4();
			push4(tl >= 0 ? tl : -tl);
			continue;
		case O_ABS8:
			pc.cp++;
			td = pop8();
			push8(td >= 0.0 ? td : -td);
			continue;
		case O_NEG2:
			pc.cp++;
			ts = -pop2();
			push4((long)ts);
			continue;
		case O_NEG4:
			pc.cp++;
			tl = -pop4();
			push4(tl);
			continue;
		case O_NEG8:
			pc.cp++;
			td = -pop8();
			push8(td);
			continue;
		case O_DIV2:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push4(tl1 / tl);
			continue;
		case O_DIV4:
			pc.cp++;
			tl = pop4();
			tl1 = pop4();
			push4(tl1 / tl);
			continue;
		case O_DIV24:
			pc.cp++;
			tl = pop2();
			tl1 = pop4();
			push4(tl1 / tl);
			continue;
		case O_DIV42:
			pc.cp++;
			tl = pop4();
			tl1 = pop2();
			push4(tl1 / tl);
			continue;
		case O_MOD2:
			pc.cp++;
			tl = pop2();
			tl1 = pop2();
			push4(tl1 % tl);
			continue;
		case O_MOD4:
			pc.cp++;
			tl = pop4();
			tl1 = pop4();
			push4(tl1 % tl);
			continue;
		case O_MOD24:
			pc.cp++;
			tl = pop2();
			tl1 = pop4();
			push4(tl1 % tl);
			continue;
		case O_MOD42:
			pc.cp++;
			tl = pop4();
			tl1 = pop2();
			push4(tl1 % tl);
			continue;
		case O_ADD8:
			pc.cp++;
			td = pop8();
			td1 = pop8();
			push8(td1 + td);
			continue;
		case O_SUB8:
			pc.cp++;
			td = pop8();
			td1 = pop8();
			push8(td1 - td);
			continue;
		case O_MUL8:
			pc.cp++;
			td = pop8();
			td1 = pop8();
			push8(td1 * td);
			continue;
		case O_DVD8:
			pc.cp++;
			td = pop8();
			td1 = pop8();
			push8(td1 / td);
			continue;
		case O_STOI:
			pc.cp++;
			ts = pop2();
			push4((long)ts);
			continue;
		case O_STOD:
			pc.cp++;
			td = pop2();
			push8(td);
			continue;
		case O_ITOD:
			pc.cp++;
			td = pop4();
			push8(td);
			continue;
		case O_ITOS:
			pc.cp++;
			tl = pop4();
			push2((short)tl);
			continue;
		case O_DVD2:
			pc.cp++;
			td = pop2();
			td1 = pop2();
			push8(td1 / td);
			continue;
		case O_DVD4:
			pc.cp++;
			td = pop4();
			td1 = pop4();
			push8(td1 / td);
			continue;
		case O_DVD24:
			pc.cp++;
			td = pop2();
			td1 = pop4();
			push8(td1 / td);
			continue;
		case O_DVD42:
			pc.cp++;
			td = pop4();
			td1 = pop2();
			push8(td1 / td);
			continue;
		case O_DVD28:
			pc.cp++;
			td = pop2();
			td1 = pop8();
			push8(td1 / td);
			continue;
		case O_DVD48:
			pc.cp++;
			td = pop4();
			td1 = pop8();
			push8(td1 / td);
			continue;
		case O_DVD82:
			pc.cp++;
			td = pop8();
			td1 = pop2();
			push8(td1 / td);
			continue;
		case O_DVD84:
			pc.cp++;
			td = pop8();
			td1 = pop4();
			push8(td1 / td);
			continue;
		case O_RV1:
			tcp = _display.raw[*pc.ucp++];
			push2((short)(*(tcp + *pc.sp++)));
			continue;
		case O_RV14:
			tcp = _display.raw[*pc.ucp++];
			push4((long)(*(tcp + *pc.sp++)));
			continue;
		case O_RV2:
			tcp = _display.raw[*pc.ucp++];
			push2(*(short *)(tcp + *pc.sp++));
			continue;
		case O_RV24:
			tcp = _display.raw[*pc.ucp++];
			push4((long)(*(short *)(tcp + *pc.sp++)));
			continue;
		case O_RV4:
			tcp = _display.raw[*pc.ucp++];
			push4(*(long *)(tcp + *pc.sp++));
			continue;
		case O_RV8:
			tcp = _display.raw[*pc.ucp++];
			pushsze8(*(struct sze8 *)(tcp + *pc.sp++));
			continue;
		case O_RV:
			tcp = _display.raw[*pc.ucp++];
			tcp += *pc.sp++;
			tl = *pc.usp++;
			STACKALIGN(tl1, tl);
			tcp1 = pushsp(tl1);
			blkcpy(tcp, tcp1, tl);
			continue;
		case O_LV:
			tcp = _display.raw[*pc.ucp++];
			pushaddr(tcp + *pc.sp++);
			continue;
		case O_LRV1:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			push2((short)(*(tcp + tl)));
			continue;
		case O_LRV14:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			push4((long)(*(tcp + tl)));
			continue;
		case O_LRV2:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			push2(*(short *)(tcp + tl));
			continue;
		case O_LRV24:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			push4((long)(*(short *)(tcp + tl)));
			continue;
		case O_LRV4:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			push4(*(long *)(tcp + tl));
			continue;
		case O_LRV8:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			pushsze8(*(struct sze8 *)(tcp + tl));
			continue;
		case O_LRV:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			tcp += tl;
			tl = *pc.usp++;
			STACKALIGN(tl1, tl);
			tcp1 = pushsp(tl1);
			blkcpy(tcp, tcp1, tl);
			continue;
		case O_LLV:
			tcp = _display.raw[*pc.ucp++];
			PCLONGVAL(tl);
			pushaddr(tcp + tl);
			continue;
		case O_IND1:
			pc.cp++;
			ts = *popaddr();
			push2(ts);
			continue;
		case O_IND14:
			pc.cp++;
			ti = *popaddr();
			push4((long)ti);
			continue;
		case O_IND2:
			pc.cp++;
			ts = *(short *)(popaddr());
			push2(ts);
			continue;
		case O_IND24:
			pc.cp++;
			ts = *(short *)(popaddr());
			push4((long)ts);
			continue;
		case O_IND4:
			pc.cp++;
			tl = *(long *)(popaddr());
			push4(tl);
			continue;
		case O_IND8:
			pc.cp++;
			t8 = *(struct sze8 *)(popaddr());
			pushsze8(t8);
			continue;
		case O_IND:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			tcp = popaddr();
			STACKALIGN(tl1, tl);
			tcp1 = pushsp(tl1);
			blkcpy(tcp, tcp1, tl);
			continue;
		case O_CON1:
			push2((short)(*pc.cp++));
			continue;
		case O_CON14:
			push4((long)(*pc.cp++));
			continue;
		case O_CON2:
			pc.cp++;
			push2(*pc.sp++);
			continue;
		case O_CON24:
			pc.cp++;
			push4((long)(*pc.sp++));
			continue;
		case O_CON4:
			pc.cp++;
			PCLONGVAL(tl);
			push4(tl);
			continue;
		case O_CON8:
			pc.cp++;
			tcp = pushsp(sizeof(double));
			blkcpy(pc.cp, tcp, sizeof(double));
			pc.dbp++;
			continue;
		case O_CON:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			STACKALIGN(tl1, tl);
			tcp = pushsp(tl1);
			blkcpy(pc.cp, tcp, tl);
			pc.cp += (int)tl;
			continue;
		case O_CONG:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			STACKALIGN(tl1, tl);
			tcp = pushsp(tl1);
			blkcpy(pc.cp, tcp, tl1);
			pc.cp += (int)((tl + 2) & ~1);
			continue;
		case O_LVCON:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			pushaddr(pc.cp);
			tl = (tl + 1) & ~1;
			pc.cp += (int)tl;
			continue;
		case O_RANG2:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop2();
			push2((short)(RANG4(tl1, tl, (long)(*pc.sp++))));
			continue;
		case O_RANG42:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop4();
			push4(RANG4(tl1, tl, (long)(*pc.sp++)));
			continue;
		case O_RSNG2:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop2();
			push2((short)(RSNG4(tl1, tl)));
			continue;
		case O_RSNG42:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop4();
			push4(RSNG4(tl1, tl));
			continue;
		case O_RANG4:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			tl1 = pop4();
			PCLONGVAL(tl2);
			push4(RANG4(tl1, tl, tl2));
			continue;
		case O_RANG24:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			tl1 = pop2();
			PCLONGVAL(tl2);
			push2((short)(RANG4(tl1, tl, tl2)));
			continue;
		case O_RSNG4:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			tl1 = pop4();
			push4(RSNG4(tl1, tl));
			continue;
		case O_RSNG24:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			tl1 = pop2();
			push2((short)(RSNG4(tl1, tl)));
			continue;
		case O_STLIM:
			pc.cp++;
			tl = pop4();
			STLIM(tl);
			continue;
		case O_LLIMIT:
			pc.cp++;
			tcp = popaddr();
			tl = pop4();
			LLIMIT(tcp, tl);
			continue;
		case O_BUFF:
			BUFF((long)(*pc.cp++));
			continue;
		case O_HALT:
			pc.cp++;
			if (_nodump == TRUE)
				psexit(0);
			fputs("\nCall to procedure halt\n", stderr);
			backtrace("Halted");
			psexit(0);
			continue;
		case O_PXPBUF:
			pc.cp++;
			PCLONGVAL(tl);
			_cntrs = tl;
			PCLONGVAL(tl);
			_rtns = tl;
			NEW(&_pcpcount, (_cntrs + 1) * sizeof(long));
			blkclr(_pcpcount, (_cntrs + 1) * sizeof(long));
			continue;
		case O_COUNT:
			pc.cp++;
			_pcpcount[*pc.usp++]++;
			continue;
		case O_CASE1OP:
			tl = *pc.cp++;		/* tl = number of cases */
			if (tl == 0)
				tl = *pc.usp++;
			tsp = pc.sp + tl;	/* ptr to end of jump table */
			tcp = (char *)tsp;	/* tcp = ptr to case values */
			tl1 = pop2();		/* tl1 = element to find */
			for(; tl > 0; tl--)	/* look for element */
				if (tl1 == *tcp++)
					break;
			if (tl == 0)		/* default case => error */
				CASERNG(tl1);
			pc.cp += *(tsp - tl);
			continue;
		case O_CASE2OP:
			tl = *pc.cp++;		/* tl = number of cases */
			if (tl == 0)
				tl = *pc.usp++;
			tsp = pc.sp + tl;	/* ptr to end of jump table */
			tsp1 = tsp;		/* tsp1 = ptr to case values */
			tl1 = (unsigned short)pop2();/* tl1 = element to find */
			for(; tl > 0; tl--)	/* look for element */
				if (tl1 == *tsp++)
					break;
			if (tl == 0)		/* default case => error */
				CASERNG(tl1);
			pc.cp += *(tsp1 - tl);
			continue;
		case O_CASE4OP:
			tl = *pc.cp++;		/* tl = number of cases */
			if (tl == 0)
				tl = *pc.usp++;
			tsp1 = pc.sp + tl;	/* ptr to end of jump table */
			tlp = (long *)tsp1;	/* tlp = ptr to case values */
			tl1 = pop4();		/* tl1 = element to find */
			for(; tl > 0; tl--) {	/* look for element */
				GETLONGVAL(tl2, tlp++);
				if (tl1 == tl2)
					break;
			}
			if (tl == 0)		/* default case => error */
				CASERNG(tl1);
			pc.cp += *(tsp1 - tl);
			continue;
		case O_ADDT:
			tl = *pc.cp++;		/* tl has comparison length */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = pushsp((long)(0));/* tcp pts to first arg */
			ADDT(tcp + tl, tcp + tl, tcp, tl >> 2);
			popsp(tl);
			continue;
		case O_SUBT:
			tl = *pc.cp++;		/* tl has comparison length */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = pushsp((long)(0));/* tcp pts to first arg */
			SUBT(tcp + tl, tcp + tl, tcp, tl >> 2);
			popsp(tl);
			continue;
		case O_MULT:
			tl = *pc.cp++;		/* tl has comparison length */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = pushsp((long)(0));/* tcp pts to first arg */
			MULT(tcp + tl, tcp + tl, tcp, tl >> 2);
			popsp(tl);
			continue;
		case O_INCT:
			tl = *pc.cp++;		/* tl has number of args */
			if (tl == 0)
				tl = *pc.usp++;
			tb = INCT();
			popsp(tl*sizeof(long));
			push2((short)(tb));
			continue;
		case O_CTTOT:
			tl = *pc.cp++;		/* tl has number of args */
			if (tl == 0)
				tl = *pc.usp++;
			tl1 = tl * sizeof(long);	/* Size of all args */
			tcp = pushsp((long)(0)) + tl1; /* tcp pts to result */
			tl1 = pop4();		/* Pop the 4 fixed args */
			tl2 = pop4();
			tl3 = pop4();
			tl4 = pop4();
			tcp2 = pushsp((long)0);	/* tcp2 -> data values */
			CTTOTA(tcp, tl1, tl2, tl3, tl4, tcp2);
			popsp(tl*sizeof(long) - 4*sizeof(long)); /* Pop data */
			continue;
		case O_CARD:
			tl = *pc.cp++;		/* tl has comparison length */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = pushsp((long)(0));/* tcp pts to set */
			tl1 = CARD(tcp, tl);
			popsp(tl);
			push2((short)(tl1));
			continue;
		case O_IN:
			tl = *pc.cp++;		/* tl has comparison length */
			if (tl == 0)
				tl = *pc.usp++;
			tl1 = pop4();		/* tl1 is the element */
			tcp = pushsp((long)(0));/* tcp pts to set */
			tl2 = *pc.sp++;	/* lower bound */
			tb = IN(tl1, tl2, (long)(*pc.usp++), tcp);
			popsp(tl);
			push2((short)(tb));
			continue;
		case O_ASRT:
			pc.cp++;
			tl = pop4();
			tcp = popaddr();
			ASRTS(tl, tcp);
			continue;
		case O_FOR1U:
			tl1 = *pc.cp++;		/* tl1 loop branch */
			if (tl1 == 0)
				tl1 = *pc.sp++;
			tcp = popaddr();	/* tcp = ptr to index var */
			tl = pop4();		/* tl upper bound */
			if (*tcp == tl)		/* loop is done, fall through */
				continue;
			*tcp += 1;		/* inc index var */
			pc.cp += tl1;		/* return to top of loop */
			continue;
		case O_FOR2U:
			tl1 = *pc.cp++;		/* tl1 loop branch */
			if (tl1 == 0)
				tl1 = *pc.sp++;
			tsp = (short *)popaddr(); /* tsp = ptr to index var */
			tl = pop4();		/* tl upper bound */
			if (*tsp == tl)		/* loop is done, fall through */
				continue;
			*tsp += 1;		/* inc index var */
			pc.cp += tl1;		/* return to top of loop */
			continue;
		case O_FOR4U:
			tl1 = *pc.cp++;		/* tl1 loop branch */
			if (tl1 == 0)
				tl1 = *pc.sp++;
			tlp = (long *)popaddr(); /* tlp = ptr to index var */
			tl = pop4();		/* tl upper bound */
			if (*tlp == tl)		/* loop is done, fall through */
				continue;
			*tlp += 1;		/* inc index var */
			pc.cp += tl1;		/* return to top of loop */
			continue;
		case O_FOR1D:
			tl1 = *pc.cp++;		/* tl1 loop branch */
			if (tl1 == 0)
				tl1 = *pc.sp++;
			tcp = popaddr();	/* tcp = ptr to index var */
			tl = pop4();		/* tl upper bound */
			if (*tcp == tl)		/* loop is done, fall through */
				continue;
			*tcp -= 1;		/* dec index var */
			pc.cp += tl1;		/* return to top of loop */
			continue;
		case O_FOR2D:
			tl1 = *pc.cp++;		/* tl1 loop branch */
			if (tl1 == 0)
				tl1 = *pc.sp++;
			tsp = (short *)popaddr(); /* tsp = ptr to index var */
			tl = pop4();		/* tl upper bound */
			if (*tsp == tl)		/* loop is done, fall through */
				continue;
			*tsp -= 1;		/* dec index var */
			pc.cp += tl1;		/* return to top of loop */
			continue;
		case O_FOR4D:
			tl1 = *pc.cp++;		/* tl1 loop branch */
			if (tl1 == 0)
				tl1 = *pc.sp++;
			tlp = (long *)popaddr(); /* tlp = ptr to index var */
			tl = pop4();		/* tl upper bound */
			if (*tlp == tl)		/* loop is done, fall through */
				continue;
			*tlp -= 1;		/* dec index var */
			pc.cp += tl1;		/* return to top of loop */
			continue;
		case O_READE:
			pc.cp++;
			PCLONGVAL(tl);
			push2((short)(READE(curfile, base + tl)));
			continue;
		case O_READ4:
			pc.cp++;
			push4(READ4(curfile));
			continue;
		case O_READC:
			pc.cp++;
			push2((short)(READC(curfile)));
			continue;
		case O_READ8:
			pc.cp++;
			push8(READ8(curfile));
			continue;
		case O_READLN:
			pc.cp++;
			READLN(curfile);
			continue;
		case O_EOF:
			pc.cp++;
			tcp = popaddr();
			push2((short)(TEOF(tcp)));
			continue;
		case O_EOLN:
			pc.cp++;
			tcp = popaddr();
			push2((short)(TEOLN(tcp)));
			continue;
		case O_WRITEC:
			pc.cp++;
			ti = popint();
			tf = popfile();
			if (_runtst) {
				WRITEC(curfile, ti, tf);
				continue;
			}
			fputc(ti, tf);
			continue;
		case O_WRITES:
			pc.cp++;		/* Skip arg size */
			tf = popfile();
			ti = popint();
			ti2 = popint();
			tcp2 = popaddr();
			if (_runtst) {
				WRITES(curfile, tf, ti, ti2, tcp2);
				continue;
			}
			fwrite(tf, ti, ti2, tcp2);
			continue;
		case O_WRITEF:
			tf = popfile();
			tcp = popaddr();
			tcp2 = pushsp((long)0);	/* Addr of printf's args */
			if (_runtst) {
				VWRITEF(curfile, tf, tcp, tcp2);
			} else {
				vfprintf(tf, tcp, tcp2);
			}
			popsp((long)
			    (*pc.cp++) - (sizeof (FILE *)) - sizeof (char *));
			continue;
		case O_WRITLN:
			pc.cp++;
			if (_runtst) {
				WRITLN(curfile);
				continue;
			}
			fputc('\n', ACTFILE(curfile));
			continue;
		case O_PAGE:
			pc.cp++;
			if (_runtst) {
				PAGE(curfile);
				continue;
			}
			fputc('', ACTFILE(curfile));
			continue;
		case O_NAM:
			pc.cp++;
			tl = pop4();
			PCLONGVAL(tl1);
			pushaddr(NAM(tl, base + tl1));
			continue;
		case O_MAX:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			tl1 = pop4();
			if (_runtst) {
				push4(MAX(tl1, tl, (long)(*pc.usp++)));
				continue;
			}
			tl1 -= tl;
			tl = *pc.usp++;
			push4(tl1 > tl ? tl1 : tl);
			continue;
		case O_MIN:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.usp++;
			tl1 = pop4();
			push4(tl1 < tl ? tl1 : tl);
			continue;
		case O_UNIT:
			pc.cp++;
			curfile = UNIT(popaddr());
			continue;
		case O_UNITINP:
			pc.cp++;
			curfile = INPUT;
			continue;
		case O_UNITOUT:
			pc.cp++;
			curfile = OUTPUT;
			continue;
		case O_MESSAGE:
			pc.cp++;
			PFLUSH();
			curfile = ERR;
			continue;
		case O_PUT:
			pc.cp++;
			PUT(curfile);
			continue;
		case O_GET:
			pc.cp++;
			GET(curfile);
			continue;
		case O_FNIL:
			pc.cp++;
			tcp = popaddr();
			pushaddr(FNIL(tcp));
			continue;
		case O_DEFNAME:
			pc.cp++;
			tcp2 = popaddr();
			tcp = popaddr();
			tl = pop4();
			tl2 = pop4();
			DEFNAME((struct iorec *)tcp2, tcp, tl, tl2);
			continue;
		case O_RESET:
			pc.cp++;
			tcp2 = popaddr();
			tcp = popaddr();
			tl = pop4();
			tl2 = pop4();
			RESET((struct iorec *)tcp2, tcp, tl, tl2);
			continue;
		case O_REWRITE:
			pc.cp++;
			tcp2 = popaddr();
			tcp = popaddr();
			tl = pop4();
			tl2 = pop4();
			REWRITE((struct iorec *)tcp2, tcp, tl, tl2);
			continue;
		case O_FILE:
			pc.cp++;
			pushaddr(ACTFILE(curfile));
			continue;
		case O_REMOVE:
			pc.cp++;
			tcp = popaddr();
			tl = pop4();
			REMOVE(tcp, tl);
			continue;
		case O_FLUSH:
			pc.cp++;
			tcp = popaddr();
			FLUSH((struct iorec *)tcp);
			continue;
		case O_PACK:
			pc.cp++;
			tl = pop4();
			tcp = popaddr();
			tcp2 = popaddr();
			tl1 = pop4();
			tl2 = pop4();
			tl3 = pop4();
			tl4 = pop4();
			PACK(tl, tcp, tcp2, tl1, tl2, tl3, tl4);
			continue;
		case O_UNPACK:
			pc.cp++;
			tl = pop4();
			tcp = popaddr();
			tcp2 = popaddr();
			tl1 = pop4();
			tl2 = pop4();
			tl3 = pop4();
			tl4 = pop4();
			UNPACK(tl, tcp, tcp2, tl1, tl2, tl3, tl4);
			continue;
		case O_ARGC:
			pc.cp++;
			push4((long)_argc);
			continue;
		case O_ARGV:
			tl = *pc.cp++;		/* tl = size of char array */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = popaddr();	/* tcp = addr of char array */
			tl1 = pop4();		/* tl1 = argv subscript */
			ARGV(tl1, tcp, tl);
			continue;
		case O_CLCK:
			pc.cp++;
			push4(CLCK());
			continue;
		case O_WCLCK:
			pc.cp++;
			push4(time(0));
			continue;
		case O_SCLCK:
			pc.cp++;
			push4(SCLCK());
			continue;
		case O_NEW:
			tl = *pc.cp++;		/* tl = size being new'ed */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = popaddr();	/* ptr to ptr being new'ed */
			NEW(tcp, tl);
			if (_runtst) {
				blkclr(*((char **)(tcp)), tl);
			}
			continue;
		case O_DISPOSE:
			tl = *pc.cp++;		/* tl = size being disposed */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = popaddr();	/* ptr to ptr being disposed */
			DISPOSE(tcp, tl);
			*(char **)tcp = (char *)0;
			continue;
		case O_DFDISP:
			tl = *pc.cp++;		/* tl = size being disposed */
			if (tl == 0)
				tl = *pc.usp++;
			tcp = popaddr();	/* ptr to ptr being disposed */
			DFDISPOSE(tcp, tl);
			*(char **)tcp = (char *)0;
			continue;
		case O_DATE:
			pc.cp++;
			DATE(popaddr());
			continue;
		case O_TIME:
			pc.cp++;
			TIME(popaddr());
			continue;
		case O_UNDEF:
			pc.cp++;
			td = pop8();
			push2((short)(0));
			continue;
		case O_ATAN:
			pc.cp++;
			td = pop8();
			if (_runtst) {
				push8(ATAN(td));
				continue;
			}
			push8(atan(td));
			continue;
		case O_COS:
			pc.cp++;
			td = pop8();
			if (_runtst) {
				push8(COS(td));
				continue;
			}
			push8(cos(td));
			continue;
		case O_EXP:
			pc.cp++;
			td = pop8();
			if (_runtst) {
				push8(EXP(td));
				continue;
			}
			push8(exp(td));
			continue;
		case O_LN:
			pc.cp++;
			td = pop8();
			if (_runtst) {
				push8(LN(td));
				continue;
			}
			push8(log(td));
			continue;
		case O_SIN:
			pc.cp++;
			td = pop8();
			if (_runtst) {
				push8(SIN(td));
				continue;
			}
			push8(sin(td));
			continue;
		case O_SQRT:
			pc.cp++;
			td = pop8();
			if (_runtst) {
				push8(SQRT(td));
				continue;
			}
			push8(sqrt(td));
			continue;
		case O_CHR2:
		case O_CHR4:
			pc.cp++;
			tl = pop4();
			if (_runtst) {
				push2((short)(CHR(tl)));
				continue;
			}
			push2((short)tl);
			continue;
		case O_ODD2:
		case O_ODD4:
			pc.cp++;
			tl = pop4();
			push2((short)(tl & 1));
			continue;
		case O_SUCC2:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop4();
			if (_runtst) {
				push2((short)(SUCC(tl1, tl, (long)(*pc.sp++))));
				continue;
			}
			push2((short)(tl1 + 1));
			pc.sp++;
			continue;
		case O_SUCC24:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop4();
			if (_runtst) {
				push4(SUCC(tl1, tl, (long)(*pc.sp++)));
				continue;
			}
			push4(tl1 + 1);
			pc.sp++;
			continue;
		case O_SUCC4:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			tl1 = pop4();
			if (_runtst) {
				PCLONGVAL(tl2);
				push4(SUCC(tl1, tl, (long)(tl2)));
				continue;
			}
			push4(tl1 + 1);
			pc.lp++;
			continue;
		case O_PRED2:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop4();
			if (_runtst) {
				push2((short)(PRED(tl1, tl, (long)(*pc.sp++))));
				continue;
			}
			push2((short)(tl1 - 1));
			pc.sp++;
			continue;
		case O_PRED24:
			tl = *pc.cp++;
			if (tl == 0)
				tl = *pc.sp++;
			tl1 = pop4();
			if (_runtst) {
				push4(PRED(tl1, tl, (long)(*pc.sp++)));
				continue;
			}
			push4(tl1 - 1);
			pc.sp++;
			continue;
		case O_PRED4:
			tl = *pc.cp++;
			if (tl == 0)
				PCLONGVAL(tl);
			tl1 = pop4();
			if (_runtst) {
				PCLONGVAL(tl2);
				push4(PRED(tl1, tl, (long)(tl2)));
				continue;
			}
			push4(tl1 - 1);
			pc.lp++;
			continue;
		case O_SEED:
			pc.cp++;
			tl = pop4();
			push4(SEED(tl));
			continue;
		case O_RANDOM:
			pc.cp++;
			td = pop8();		/* Argument is ignored */
			push8(RANDOM());
			continue;
		case O_EXPO:
			pc.cp++;
			td = pop8();
			push4(EXPO(td));
			continue;
		case O_SQR2:
		case O_SQR4:
			pc.cp++;
			tl = pop4();
			push4(tl * tl);
			continue;
		case O_SQR8:
			pc.cp++;
			td = pop8();
			push8(td * td);
			continue;
		case O_ROUND:
			pc.cp++;
			td = pop8();
			push4(ROUND(td));
			continue;
		case O_TRUNC:
			pc.cp++;
			td = pop8();
			push4(TRUNC(td));
			continue;
		default:
			ERROR("Panic: bad op code\n");
			continue;
		}
	}
}
