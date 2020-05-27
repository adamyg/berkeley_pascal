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
 */

#ifndef lint
static char sccsid[] = "@(#)fhdr.c	8.1 (Berkeley) 6/6/93";
#endif /* not lint */

#include "whoami.h"
#include "0.h"
#include "tree.h"
#include "opcode.h"
#include "objfmt.h"
#include "align.h"
#include "tree_ty.h"

/*
 * this array keeps the pxp counters associated with
 * functions and procedures, so that they can be output
 * when their bodies are encountered
 */
int	bodycnts[ DSPLYSZ ];

#ifdef PC
#   include "pc.h"
#endif PC

#ifdef OBJ
int	cntpatch;
int	nfppatch;
#endif OBJ

/*
 * Funchdr inserts
 * declaration of a the
 * prog/proc/func into the
 * namelist. It also handles
 * the arguments and puts out
 * a transfer which defines
 * the entry point of a procedure.
 */

struct nl *
funchdr(r)
	struct tnode *r;
{
	register struct nl *p;
	register struct tnode *rl;
	struct nl *cp, *dp, *temp;
	int o;

	if (inpflist(r->p_dec.id_ptr)) {
		opush('l');
		yyretrieve();	/* kludge */
	}
	pfcnt++;
	parts[ cbn ] |= RPRT;
	line = r->p_dec.line_no;
	if (r->p_dec.param_list == TR_NIL &&
		(p=lookup1(r->p_dec.id_ptr)) != NIL && bn == cbn) {
		/*
		 * Symbol already defined
		 * in this block. it is either
		 * a redeclared symbol (error)
		 * a forward declaration,
		 * or an external declaration.
		 * check that forwards are of the right kind:
		 *     if this fails, we are trying to redefine it
		 *     and enter() will complain.
		 */
		if (  ( ( p->nl_flags & NFORWD ) != 0 )
		   && (  ( p->class == FUNC && r->tag == T_FDEC )
		      || ( p->class == PROC && r->tag == T_PDEC ) ) ) {
			/*
			 * Grammar doesnt forbid
			 * types on a resolution
			 * of a forward function
			 * declaration.
			 */
			if (p->class == FUNC && r->p_dec.type)
				error("Function type should be given only in forward declaration");
			/*
			 * get another counter for the actual
			 */
			if ( monflg ) {
			    bodycnts[ cbn ] = getcnt();
			}
#			ifdef PC
			    enclosing[ cbn ] = p -> symbol;
#			endif PC
#			ifdef PTREE
				/*
				 *	mark this proc/func as forward
				 *	in the pTree.
				 */
			    pDEF( p -> inTree ).PorFForward = TRUE;
#			endif PTREE
			return (p);
		}
	}

	/* if a routine segment is being compiled,
	 * do level one processing.
	 */

	 if ((r->tag != T_PROG) && (!progseen))
		level1();


	/*
	 * Declare the prog/proc/func
	 */
	switch (r->tag) {
	    case T_PROG:
		    progseen = TRUE;
		    if (opt('z'))
			    monflg = TRUE;
		    program = p = defnl(r->p_dec.id_ptr, PROG, NLNIL, 0);
		    p->value[3] = r->p_dec.line_no;
		    break;
	    case T_PDEC:
		    if (r->p_dec.type != TR_NIL)
			    error("Procedures do not have types, only functions do");
		    p = enter(defnl(r->p_dec.id_ptr, PROC, NLNIL, 0));
		    p->nl_flags |= NMOD;
#		    ifdef PC
			enclosing[ cbn ] = r->p_dec.id_ptr;
			p -> extra_flags |= NGLOBAL;
#		    endif PC
		    break;
	    case T_FDEC:
		    {
			register struct tnode *il;
		    il = r->p_dec.type;
		    if (il == TR_NIL) {
			    temp = NLNIL;
			    error("Function type must be specified");
		    } else if (il->tag != T_TYID) {
			    temp = NLNIL;
			    error("Function type can be specified only by using a type identifier");
		    } else
			    temp = gtype(il);
		    }
		    p = enter(defnl(r->p_dec.id_ptr, FUNC, temp, NIL));
		    p->nl_flags |= NMOD;
		    /*
		     * An arbitrary restriction
		     */
		    switch (o = classify(p->type)) {
			    case TFILE:
			    case TARY:
			    case TREC:
			    case TSET:
			    case TSTR:
				    warning();
				    if (opt('s')) {
					    standard();
				    }
				    error("Functions should not return %ss", clnames[o]);
		    }
#		    ifdef PC
			enclosing[ cbn ] = r->p_dec.id_ptr;
			p -> extra_flags |= NGLOBAL;
#		    endif PC
		    break;
	    default:
		    panic("funchdr");
	}
	if (r->tag != T_PROG) {
		/*
		 * Mark this proc/func as
		 * being forward declared
		 */
		p->nl_flags |= NFORWD;
		/*
		 * Enter the parameters
		 * in the next block for
		 * the time being
		 */
		if (++cbn >= DSPLYSZ) {
			error("Procedure/function nesting too deep");
			pexit(ERRS);
		}
		/*
		 * For functions, the function variable
		 */
		if (p->class == FUNC) {
#			ifdef OBJ
			    cp = defnl(r->p_dec.id_ptr, FVAR, p->type, 0);
#			endif OBJ
#			ifdef PC
				/*
				 * fvars used to be allocated and deallocated
				 * by the caller right before the arguments.
				 * the offset of the fvar was kept in
				 * value[NL_OFFS] of function (very wierd,
				 * but see asgnop).
				 * now, they are locals to the function
				 * with the offset kept in the fvar.
				 */

			    cp = defnl(r->p_dec.id_ptr, FVAR, p->type,
				(int)-roundup(roundup(
			            (int)(DPOFF1+lwidth(p->type)),
				    (long)align(p->type))), (long) A_STACK);
			    cp -> extra_flags |= NLOCAL;
#			endif PC
			cp->chain = p;
			p->ptr[NL_FVAR] = cp;
		}
		/*
		 * Enter the parameters
		 * and compute total size
		 */
	        p->value[NL_OFFS] = params(p, r->p_dec.param_list);
		/*
		 * because NL_LINENO field in the function 
		 * namelist entry has been used (as have all
		 * the other fields), the line number is
		 * stored in the NL_LINENO field of its fvar.
		 */
		if (p->class == FUNC)
		    p->ptr[NL_FVAR]->value[NL_LINENO] = r->p_dec.line_no;
		else
		    p->value[NL_LINENO] = r->p_dec.line_no;
		cbn--;
	} else { 
		/*
		 * The wonderful
		 * program statement!
		 */
#		ifdef OBJ
		    if (monflg) {
			    (void) put(1, O_PXPBUF);
			    cntpatch = put(2, O_CASE4, (long)0);
			    nfppatch = put(2, O_CASE4, (long)0);
		    }
#		endif OBJ
		cp = p;
		for (rl = r->p_dec.param_list; rl; rl = rl->list_node.next) {
			if (rl->list_node.list == TR_NIL)
				continue;
			dp = defnl((char *) rl->list_node.list, VAR, NLNIL, 0);
			cp->chain = dp;
			cp = dp;
		}
	}
	/*
	 * Define a branch at
	 * the "entry point" of
	 * the prog/proc/func.
	 */
	p->value[NL_ENTLOC] = (int) getlab();
	if (monflg) {
		bodycnts[ cbn ] = getcnt();
		p->value[ NL_CNTR ] = 0;
	}
#	ifdef OBJ
	    (void) put(2, O_TRA4, (long)p->value[NL_ENTLOC]);
#	endif OBJ
#	ifdef PTREE
	    {
		pPointer	PF = tCopy( r );

		pSeize( PorFHeader[ nesting ] );
		if ( r->tag != T_PROG ) {
			pPointer	*PFs;

			PFs = &( pDEF( PorFHeader[ nesting ] ).PorFPFs );
			*PFs = ListAppend( *PFs , PF );
		} else {
			pDEF( PorFHeader[ nesting ] ).GlobProg = PF;
		}
		pRelease( PorFHeader[ nesting ] );
	    }
#	endif PTREE
	return (p);
}

	/*
	 * deal with the parameter declaration for a routine.
	 * p is the namelist entry of the routine.
	 * formalist is the parse tree for the parameter declaration.
	 * formalist	[0]	T_LISTPP
	 *		[1]	pointer to a formal
	 *		[2]	pointer to next formal
	 * for by-value or by-reference formals, the formal is
	 * formal	[0]	T_PVAL or T_PVAR
	 *		[1]	pointer to id_list
	 *		[2]	pointer to type (error if not typeid)
	 * for function and procedure formals, the formal is
	 * formal	[0]	T_PFUNC or T_PPROC
	 *		[1]	pointer to id_list (error if more than one)
	 *		[2]	pointer to type (error if not typeid, or proc)
	 *		[3]	pointer to formalist for this routine.
	 */
fparams(p, formal)
	register struct nl *p;
	struct tnode *formal;		/* T_PFUNC or T_PPROC */
{
	(void) params(p, formal->pfunc_node.param_list);
	p -> value[ NL_LINENO ] = formal->pfunc_node.line_no;
	p -> ptr[ NL_FCHAIN ] = p -> chain;
	p -> chain = NIL;
}

params(p, formalist)
	register struct nl *p;
	struct tnode *formalist;	/* T_LISTPP */
{
	struct nl *chainp, *savedp;
	struct nl *dp;
	register struct tnode *formalp;	/* an element of the formal list */
	register struct tnode *formal;	/* a formal */
	struct tnode *r, *s, *t, *typ, *idlist;
	int w, o;

	/*
	 * Enter the parameters
	 * and compute total size
	 */
	chainp = savedp = p;

#	ifdef OBJ
	    o = 0;
#	endif OBJ
#	ifdef PC
		/*
		 * parameters used to be allocated backwards,
		 * then fixed.  for pc, they are allocated correctly.
		 * also, they are aligned.
		 */
	    o = DPOFF2;
#	endif PC
	for (formalp = formalist; formalp != TR_NIL;
			formalp = formalp->list_node.next) {
		formal = formalp->list_node.list;
		if (formal == TR_NIL)
			continue;
		/*
		 * Parametric procedures
		 * don't have types !?!
		 */
		typ = formal->pfunc_node.type;
		p = NLNIL;
		if ( typ == TR_NIL ) {
		    if ( formal->tag != T_PPROC ) {
			error("Types must be specified for arguments");
		    }
		} else {
		    if ( formal->tag == T_PPROC ) {
			error("Procedures cannot have types");
		    } else {
			p = gtype(typ);
		    }
		}
		for (idlist = formal->param.id_list; idlist != TR_NIL;
				idlist = idlist->list_node.next) {
			switch (formal->tag) {
			    default:
				    panic("funchdr2");
			    case T_PVAL:
				    if (p != NLNIL) {
					    if (p->class == FILET)
						    error("Files cannot be passed by value");
					    else if (p->nl_flags & NFILES)
						    error("Files cannot be a component of %ss passed by value",
							    nameof(p));
				    }
#				    ifdef OBJ
					w = lwidth(p);
					o -= roundup(w, (long) A_STACK);
#					ifdef DEC11
					    dp = defnl((char *) idlist->list_node.list,
								VAR, p, o);
#					else
					    dp = defnl((char *) idlist->list_node.list,
						    VAR,p, (w < 2) ? o + 1 : o);
#					endif DEC11
#				    endif OBJ
#				    ifdef PC
					o = roundup(o, (long) A_STACK);
					w = lwidth(p);
#					ifndef DEC11
					    if (w <= sizeof(int)) {
						o += sizeof(int) - w;
					    }
#					endif not DEC11
					dp = defnl((char *) idlist->list_node.list,VAR,
							p, o);
					o += w;
#				    endif PC
				    dp->nl_flags |= NMOD;
				    break;
			    case T_PVAR:
#				    ifdef OBJ
					dp = defnl((char *) idlist->list_node.list, REF,
						    p, o -= sizeof ( int * ) );
#				    endif OBJ
#				    ifdef PC
					dp = defnl( (char *) idlist->list_node.list, REF,
						    p , 
					    o = roundup( o , (long)A_STACK ) );
					o += sizeof(char *);
#				    endif PC
				    break;
			    case T_PFUNC:
				    if (idlist->list_node.next != TR_NIL) {
					error("Each function argument must be declared separately");
					idlist->list_node.next = TR_NIL;
				    }
#				    ifdef OBJ
					dp = defnl((char *) idlist->list_node.list,FFUNC,
						p, o -= sizeof ( int * ) );
#				    endif OBJ
#				    ifdef PC
					dp = defnl( (char *) idlist->list_node.list , 
						FFUNC , p ,
						o = roundup( o , (long)A_STACK ) );
					o += sizeof(char *);
#				    endif PC
				    dp -> nl_flags |= NMOD;
				    fparams(dp, formal);
				    break;
			    case T_PPROC:
				    if (idlist->list_node.next != TR_NIL) {
					error("Each procedure argument must be declared separately");
					idlist->list_node.next = TR_NIL;
				    }
#				    ifdef OBJ
					dp = defnl((char *) idlist->list_node.list,
					    FPROC, p, o -= sizeof ( int * ) );
#				    endif OBJ
#				    ifdef PC
					dp = defnl( (char *) idlist->list_node.list ,
						FPROC , p,
						o = roundup( o , (long)A_STACK ) );
					o += sizeof(char *);
#				    endif PC
				    dp -> nl_flags |= NMOD;
				    fparams(dp, formal);
				    break;
			    }
			if (dp != NLNIL) {
#				ifdef PC
				    dp -> extra_flags |= NPARAM;
#				endif PC
				chainp->chain = dp;
				chainp = dp;
			}
		}
		if (typ != TR_NIL && typ->tag == T_TYCARY) {
#		    ifdef OBJ
			w = -roundup(lwidth(p->chain), (long) A_STACK);
#			ifndef DEC11
			    w = (w > -2)? w + 1 : w;
#			endif
#		    endif OBJ
#		    ifdef PC
			w = lwidth(p->chain);
			o = roundup(o, (long)A_STACK);
#		    endif PC
		    /*
		     * Allocate space for upper and
		     * lower bounds and width.
		     */
		    for (s=typ; s->tag == T_TYCARY; s = s->ary_ty.type) {
			for (r=s->ary_ty.type_list; r != TR_NIL;
						r = r->list_node.next) {
			    t = r->list_node.list;
			    p = p->chain;
#			    ifdef OBJ
				o += w;
#			    endif OBJ
			    chainp->chain = defnl(t->crang_ty.lwb_var,
								VAR, p, o);
			    chainp = chainp->chain;
			    chainp->nl_flags |= (NMOD | NUSED);
			    p->nptr[0] = chainp;
			    o += w;
			    chainp->chain = defnl(t->crang_ty.upb_var,
								VAR, p, o);
			    chainp = chainp->chain;
			    chainp->nl_flags |= (NMOD | NUSED);
			    p->nptr[1] = chainp;
			    o += w;
			    chainp->chain  = defnl(0, VAR, p, o);
			    chainp = chainp->chain;
			    chainp->nl_flags |= (NMOD | NUSED);
			    p->nptr[2] = chainp;
#			    ifdef PC
				o += w;
#			    endif PC
			}
		    }
		}
	}
	p = savedp;
#	ifdef OBJ
		/*
		 * Correct the naivete (naivety)
		 * of our above code to
		 * calculate offsets
		 */
	    for (dp = p->chain; dp != NLNIL; dp = dp->chain)
		    dp->value[NL_OFFS] += -o + DPOFF2;
	    return (-o + DPOFF2);
#	endif OBJ
#	ifdef PC
	    return roundup( o , (long)A_STACK );
#	endif PC
}
