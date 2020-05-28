//
// compat_index - character string operations.
//

#include "libcompat.h"

#include <string.h>

#if !defined(HAVE_INDEX)
char *
index(const char *s, int c)
{
	return strchr(s, c);
}

#else
extern void __stdlibrary_has_index(void);

void
 __stdlibrary_has_index(void)
{
}

#endif

