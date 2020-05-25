/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
#ifndef SYS_PARAM_H_INCLUDED
#define SYS_PARAM_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <process.h>
#include <direct.h>				/* chdir */
#include <fcntl.h>				/* O_RDONLY ... */
#include <io.h> 				/* open,close,read.write ... */

#define L_SET		SEEK_SET
#define L_END		SEEK_END

#define u_char		unsigned char
#define u_short 	unsigned short
#define u_long		unsigned long

#define bcopy(s,d,l)	memcpy((d), (s), (l))
#define bzero(s,l)  	memset((s), (0), (l))
#define bcmp(a,b,l) 	memcmp((a), (b), (l))

#define ST_BLKSIZE	BUFSIZ
#define	MAXBSIZE	(BUFSIZ*2)

#define getuid()	0

struct winsize {
	int		ws_col;
	int		ws_row;
};

extern volatile int	inintr;			/* Inside interrupt handler */

int			ex_read(int fd, void *buf, unsigned);
void			ex_intr(int fd);

#define read		ex_read

#endif  /*SYS_PARAM_H_INCLUDED*/
