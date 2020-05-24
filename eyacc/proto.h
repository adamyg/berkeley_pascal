/* -*- mode: c; tabs: 4; -*- */
#ifndef PROTO_H_INCLUDED
#define PROTO_H_INCLUDED

#if defined(__STDC__) || defined(__cplusplus) || \
          defined(_MSC_VER) || defined(__WATCOMC__)
# define __P(s) s
#else
# define __P(s) ()
#endif

#if defined(__WATCOMC__)      
#pragma disable_message(107)  // Missing return value for function 'xxx'
#pragma disable_message(131)  // No prototype found for function 'xxx'
#pragma disable_message(202)  // Symbol 'x' has been defined, but not referenced
#pragma disable_message(304)  // Return type 'int' assumed for function 'xxx'
#endif

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable:4013) // 'xxx' undefined; assuming extern returning int
#pragma warning(disable:4033) // 'xxx' must return a value
#pragma warning(disable:4101) // 'xxx': unreferenced local variable
#pragma warning(disable:4716) // 'xxx': must return a value
#pragma warning(disable:4996) // 'xxx': The POSIX name for this item is deprecated.
#endif

/* ey0.c */

/* ey1.c */
int     main __P((int argc, char *argv[]));
int     settty __P((void));
int     settab __P((void));
char *  chcopy __P((char *p, char *q));
char *  writem __P((struct item *pp));
char *  symnam __P((int i));
int     summary __P((void));
int     error __P((char *s, ...));
int     arrset __P((char s[]));
int     arrval __P((int n));
int     arrdone __P((void));
int     copy __P((char *v));
int     compare __P((char *v));
int *   yalloc __P((int n));
int     aryfil __P((int *v, int n, int c));
int     aryfil2 __P((char *v, int n, int c));
int     UNION __P((int *a, int *b, int *c));
int     prlook __P((struct looksets *p));

/* ey2.c */
int     setup __P((int argc, char *argv[]));
int     finact __P((void));
int     defin __P((int t));
int     defout __P((void));
int     chstash __P((int c));
int     gettok __P((void));
int     chfind __P((int t));
int     cpycode __P((void));
int     cpyact __P((void));

/* ey3.c */
int     cpres __P((void));
int     cpfir __P((void));
int     state __P((int c));
int     putitem __P((int *ptr, struct looksets *lptr));
int     cempty __P((void));
int     stagen __P((void));
int     closure __P((int i));
struct looksets *flset __P((struct looksets *p));

/* ey4.c */
int     output __P((void));
int     prred __P((void));
int     go2 __P((int i, int c));
int     apack __P((int *p, int n));
int     go2out __P((void));
int     go2gen __P((int c));
int     precftn __P((int r, int t));
int     wract __P((int i));
int     wrstate __P((int i));

/* ey5.c */
FILE *  copen __P((char *s, int c));
void    cflush __P((FILE *x));
void    cclose __P((FILE *i));
void    cexit __P((int i));

#undef __P

#endif	//PROTO_H_INCLUDED


