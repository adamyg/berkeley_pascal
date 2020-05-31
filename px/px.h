#ifndef PX_H_INCLUDED
#define PX_H_INCLUDED
/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
/*
 *	Copyright (c) 2020, Adam Young.
 *	PX public interface, for use by 'pdx' and
 *	other applications that embed the interpreter.
 */

#if !defined(__P)
#if defined(__STDC__) || defined(__cplusplus) || \
	defined(_MSC_VER) || defined(__WATCOMC__)
# define __P(s)		s
#else
# define __P(s)		()
#endif
#endif

/*
 * Trap types
 */
#define PXSIGIOT	128		/* startup */
#define PXSIGERR	129		/* general error */
#define PXSIGIO		130		/* i/o error */
#define PXSIGBPT	131		/* breakpoint */
#define PXSIGEXT	132		/* exit */

typedef void (*pxtrap_t)(int, ...);

/* int.c */
extern	int		px_main __P((int argc, const char **argv));
extern	void		px_settrap __P((pxtrap_t trapf));

/* interp.c */
extern	void		px_interpreter __P((char *base));

/* util.c */
extern	void		px_stats __P((void));
extern	void		px_backtrace __P((const char *type));
extern	void		px_exit __P((int code));
extern	void		px_liberr __P((int signum));
extern	void		px_intr __P((int signum));
extern	void		px_memsize __P((int signum));
extern	void		px_syserr __P((int signum));
extern	void		px_raise __P((int signum));

#endif  /*PX_H_INCLUDED*/
