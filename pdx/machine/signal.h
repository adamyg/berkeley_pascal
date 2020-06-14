/*
 */
#ifndef  MACHINE_SIGNAL_H_INCLUDED
#define  MACHINE_SIGNAL_H_INCLUDED

#include <signal.h>

#if defined(WIN32) || defined(_WIN32)
#if defined(__WATCOMC__)
#define  SIGTRAP        (_SIGMAX+1)
#else
#define  SIGTRAP        (NSIG+1)
#endif

#elif defined(MSDOS)
#if defined(__WATCOMC__)
#define  SIGTRAP        (_SIGMAX+1)
#else
#define  SIGTRAP        (NSIG+1)
#endif

#endif

#endif   /*MACHINE_SIGNAL_H_INCLUDED*/


