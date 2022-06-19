/*-
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "libcompat.h"

#if !defined(HAVE_BASENAME)

#include <sys/cdefs.h>
#include <string.h>

#if defined(_WIN32)
#define ISSEP(_c)   (_c == '/' || _c == '\\')
#else
#define ISSEP(_c)   (_c == '/')
#endif

#undef basename
char *
basename(char *path)
{
	char *ptr;

	/*
	 * If path is a null pointer or points to an empty string,
	 * basename() shall return a pointer to the string ".".
	 */
	if (path == NULL || *path == '\0')
		return (__DECONST(char *, "."));

	/* Find end of last pathname component and null terminate it. */
	ptr = path + strlen(path);
	while (ptr > path + 1 && ISSEP(ptr[-1]))
		--ptr;
	*ptr-- = '\0';

	/* Find beginning of last pathname component. */
	while (ptr > path && ! ISSEP(ptr[-1]))
		--ptr;
	return (ptr);
}

#else
extern void __stdlibrary_has_basename(void);

void
__stdlibrary_has_basename(void)
{
}

#endif  //HAVE_BASENAME

#ifdef MAIN
#include <stdlib.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
	printf("%s\n", basename(argv[1]));
	return 0;
}

#endif