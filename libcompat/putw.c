//
//  compat_putw
//

#include "libcompat.h"
		    
//   NAME
//
//      putw - put a word on a stream (LEGACY)
//
//   SYNOPSIS
//
//      #include <stdio.h>
//
//      int putw(int w, FILE *stream);
//
//   DESCRIPTION
//
//      The putw() function writes the word (that is, type int) w to the output stream (at the position
//      at which the file offset, if defined, is pointing). The size of a word is the size of a type
//      int and varies from machine to machine. The putw() function neither assumes nor causes special
//      alignment in the file.
//
//      The st_ctime and st_mtime fields of the file will be marked for update between the successful
//      execution of putw() and the next successful completion of a call to fflush() or fclose() on
//      the same stream or a call to exit() or abort().
//
//      This interface need not be reentrant.
//
//   RETURN VALUE
//
//      Upon successful completion, putw() returns 0. Otherwise, a non-zero value is returned,
//      the error indicators for the stream are set, and errno is set to indicate the error.
//

#if !defined(HAVE_PUTW)
int
putw(int w, FILE *fp)
{
	return (fwrite((void *)&w, sizeof(w), 1, fp) == 1 ? 0 : EOF);
}

#else
extern void __stdlibrary_has_putw(void);

void
 __stdlibrary_has_putw(void)
{
}

#endif

