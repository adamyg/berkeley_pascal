//
//  compat_strcasecmp
//

#include "libcompat.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#if !defined(HAVE_STRCASECMP)
int
strcasecmp(const char *s1, const char *s2)
{
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
	return _stricmp(s1, s2);
#else
	return stricmp(s1, s2);
#endif
}


#else
extern void __stdlibrary_has_strcasecmp(void);

void
__stdlibrary_has_strcasecmp(void)
{
}

#endif