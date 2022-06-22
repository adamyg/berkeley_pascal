//
//  compat_strncasecmp
//

#include "libcompat.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#if !defined(HAVE_STRNCASECMP)
int
strncasecmp(const char *s1, const char *s2, size_t len)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	return _strnicmp(s1, s2, len);
#else
	return strnicmp(s1, s2, len);
#endif
}


#else
extern void __stdlibrary_has_strncasecmp(void);

void
__stdlibrary_has_strncasecmp(void)
{
}

#endif