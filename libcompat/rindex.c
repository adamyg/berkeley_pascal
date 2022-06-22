//
// compat_rindex - character string operations.
//

#include "libcompat.h"

#include <string.h>
 
#if !defined(HAVE_RINDEX)
char *
rindex(const char *s, int c)
{
	return strrchr(s, c);
}

#else
extern void __stdlibrary_has_rindex(void);

void
 __stdlibrary_has_rindex(void)
{
}

#endif

