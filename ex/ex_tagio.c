/*-
 * Copyright (c) 1980 The Regents of the University of California.
 * All rights reserved.
 */

#ifndef lint
static char sccsid[] = "@(#)ex_tagio.c	7.5 (Berkeley) 4/17/91";
#endif /* not lint */

/*
 * These routines are for faster tag lookup.  They support the binary
 * search used in tagfind() instead of the linear search.  The speedup
 * is quite noticable looking for a tag at the end of a long tags
 * file.  Define FASTTAG in the Makefile to use these routines.
 */
#ifdef FASTTAG

#ifndef vms
#include <sys/file.h>
#else
#include <file.h>
#endif
#include "ex.h"

static long offset = -1;
static long block = -1;
static int bcnt = 0;
static int b_size = MAXBSIZE;
static char *ibuf;

topen(file, buf)
char *file, *buf;
{
	int fd;
	struct stat statb;

	offset = -1;
	block = -1;
	if ((fd = open(file, O_RDONLY, 0)) < 0)
		return(-1);
	if (fstat(fd, &statb) < 0) {
		(void)close(fd);
		return(-1);
	}
	ibuf = buf;
#if defined(ST_BLKSIZE)
	b_size = ST_BLKSIZE;
#else
	b_size = statb.st_blksize;
#endif
	return(fd);
}

tseek(fd, off)
int fd;
long off;
{
	int nblock;

	nblock = off / b_size * b_size;
	offset = off % b_size;
	if (nblock == block)
		return(0);
	block = nblock;
	if (lseek(fd, nblock, L_SET) < 0)
		return(-1);
	if ((bcnt = read(fd, ibuf, b_size)) < 0)
		return(-1);
	return(0);
}

tgets(buf, cnt, fd)
register char *buf;
int cnt;
int fd;
{
	register char *cp;
	register int cc;

	cc = offset;
	if (cc == -1) {
		if ((bcnt = read(fd, ibuf, b_size)) <= 0)
			return (0);
		cc = 0;
		block = 0;
	}
	if (bcnt == 0)		/* EOF */
		return(0);
	cp = ibuf + cc;
	while (--cnt > 0) {
		if (++cc > bcnt) {
			block += b_size;
			if ((bcnt = read(fd, ibuf, b_size)) <= 0) {
				offset = cc;
				return (0);
			}
			cp = ibuf;
			cc = 1;
		}
		if ((*buf++ = *cp++) == '\n')
			break;
	}
	*--buf = 0;
	offset = cc;
	return(1);
}

void
tclose(fd)
int fd;
{
	(void)close(fd);
	offset = -1;
	block = -1;
	bcnt = 0;
}
#endif
