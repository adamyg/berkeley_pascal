/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
#ifndef SRC_COMMON_H_INCLUDED
#define SRC_COMMON_H_INCLUDED
/*
 * common include libpc,px,pi,pxp and pdx.
 */

#define  CONSETS
#define  CHAR		int
#define  STATIC		static

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>		/* O_BINARY */
#include <time.h>
#include <signal.h>
#include <errno.h>

#if defined(unix)
#include <unistd.h>
#else
#include <process.h>
#include <io.h>
#endif
#undef  roundup

#if defined(_WIN32) && !defined(WIN32)
#define WIN32
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4996)  /* 'xxx': The POSIX name for this item is deprecated. ... */
#endif
      
#if defined(O_RANDOM)
#undef  O_RANDOM
#endif
#if !defined(O_BINARY)
#define O_BINARY	0
#endif

#if !defined(__P)
#if defined(__STDC__) || defined(__cplusplus) || \
          defined(_MSC_VER) || defined(__WATCOMC__)
# define __P(s)		s
#else
# define __P(s)		()
#endif
#endif

typedef enum {FALSE, TRUE} bool;

#ifndef NIL
#define NIL		NULL
#endif

#endif /*SRC_COMMON_H_INCLUDED*/
