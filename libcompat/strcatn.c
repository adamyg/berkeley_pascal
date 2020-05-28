//
//  compat_strcatn
//

#include "libcompat.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
 
/*
 * Concatenate s2 on the end of s1.  S1's space must be large enough.
 * At most n characters are moved.
 * Return s1.
 */

#if !defined(HAVE_STRCATN)
char *
strcatn(register char *s1, register char *s2, register int n)
{
	register char *os1;

	os1 = s1;
	while (*s1++)
		;
	--s1;
	while (*s1++ = *s2++)
		if (--n < 0) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}

#else
extern void __stdlibrary_has_strcatn(void);

void
 __stdlibrary_has_strcatn(void)
{
}

#endif

