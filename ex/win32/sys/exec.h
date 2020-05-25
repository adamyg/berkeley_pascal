#ifndef SYS_EXEC_H_INCLUDED
#define SYS_EXEC_H_INCLUDED
/* -*- mode: c; tabs: 4 -*- */

#define OMAGIC           	/* unshared */
#define NMAGIC                  /* shared text */
#define ZMAGIC                  /* VM/Unix demand paged */


struct exec {
    unsigned long       a_magic;
};

#endif  /*SYS_EXEC_H_INCLUDED*/


