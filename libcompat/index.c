//
// compat_index - character string operations.
//

#include "w32config.h"

#include <sys/types.h>
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

