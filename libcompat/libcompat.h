#ifndef LIBCOMPACT_H_INCLUDED
#define LIBCOMPACT_H_INCLUDED
//
//  libcompat
//

#include "w32config.h"

#include <sys/types.h>
#include <stdio.h>

#if !defined(HAVE_STRNLEN)
extern size_t strnlen(const char *str, size_t maxlen);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_STRNLEN
#endif
#endif

#if !defined(HAVE_STRNDUP)
extern char *strndup(const char *str, size_t maxlen);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_STRNDUP
#endif
#endif

#if !defined(HAVE_STRCATN)
extern char *strcatn(register char *s1, register char *s2, register int n);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_STRCATN
#endif
#endif

#if !defined(HAVE_PUTW)
extern int putw(int w, FILE *fp);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_PUTW
#endif
#endif

#if !defined(HAVE_GETW)
extern int getw(FILE *fp);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_GETW
#endif
#endif

#if !defined(HAVE_INDEX)
extern char *index(const char *s, int c);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_INDEX
#endif
#endif

#if !defined(HAVE_MKSTEMP)
extern int mkstemp(char *path);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_MKSTEMP
#endif
#endif

#if !defined(HAVE_MKTEMP)
extern char *mktemp(char *path);
#if !defined(LIBCOMPAT_SOURCE)
#define HAVE_MKTEMP
#endif
#endif

extern char *xmktemp(char *path, char *result, size_t length);

#endif /*LIBCOMPACT_H_INCLUDED*/



