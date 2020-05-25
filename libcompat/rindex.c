//
// compat_rindex - character string operations.
//

#include <sys/types.h>
#include <string.h>
 
#if !defined(HAVE_RNDEX)
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
