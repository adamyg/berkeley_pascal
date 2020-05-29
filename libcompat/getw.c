//
//  compat_getw
//

#include "libcompat.h"


//   NAME
//
//     getw - get a word from a stream (LEGACY)
//
//   SYNOPSIS
//
//     #include <stdio.h>
//
//     int getw(FILE *stream);
//
//   DESCRIPTION
//
//     The getw() function reads the next word from the stream. The size of a word is the size of
//     an int and may vary from machine to machine. The getw() function presumes no special alignment
//     in the file.
//
//     The getw() function may mark the st_atime field of the file associated with stream for update.
//     The st_atime field will be marked for update by the first successful execution of fgetc(),
//     fgets(), fread(), getc(), getchar(), gets(), fscanf() or scanf() using stream that returns
//     data not supplied by a prior call to ungetc().
//
//     This interface need not be reentrant.
//
//   RETURN VALUE
//
//     Upon successful completion, getw() returns the next word from the input stream pointed to
//     by stream. If the stream is at end-of-file, the end-of-file indicator for the stream is set
//     and getw() returns EOF. If a read error occurs, the error indicator for the stream is set,
//     getw() returns EOF and sets errno to indicate the error.
//

#if !defined(HAVE_GETW)
int
getw(FILE *fp)
{
        int x;

        return (fread((void *)&x, sizeof(x), 1, fp) == 1 ? x : EOF);
}

#else
extern void __stdlibrary_has_getw(void);

void
 __stdlibrary_has_getw(void)
{
}

#endif

