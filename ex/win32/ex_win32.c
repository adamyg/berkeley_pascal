/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */

#define  WINDOWS_MEAN_AND_LEAN
#include <windows.h>

#undef	 DELETE
#undef	 IN

#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"

volatile int inintr = 0;			/* Inside interrupt handler */

/* 
 * Replacement i/o interface, which handles interrupts within the context
 * of the main thread. Unlike unix it's unsafe to longjmp from signal 
 * handles.
 */
int
ex_read( int fd, void *buf, unsigned cnt )
{
	long	handle;
	int	ret = 0;

	if ( fd == fileno(stdin) ) {
		handle = _get_osfhandle( fd );
		while (!inintr) {
			if (WaitForSingleObject((HANDLE)handle,1000) != WAIT_TIMEOUT) {
				break;						       
			}
		}
	}

	if (inintr) {
		inintr = 0;
		reset();
		/*NOTREACHED*/
	}
	return _read( fd, buf, cnt );
}


void
ex_intr( int fd )
{
	(void) fd;

	inintr++;
}


/*
 * Preserve the current editor status.
 */
int
preserve()
{
	error("Preserve support unavailable");
	return (0);
}


/*
 * Set up to do a recover, getting io to be a pipe from
 * the recover process.
 */
void
recover()
{
	error("Recovery support unavailable");
}


/*
 * The end of a recover operation.  If the process
 * exits non-zero, force not edited; otherwise force
 * a write.
 */
void
revocer()
{
}


