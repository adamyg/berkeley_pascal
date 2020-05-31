/*-
 * Copyright (c) 1980 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *     This product includes software developed by the University of
 *     California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if !defined(lint) && defined(sccs)
static char          sccsid[] = "@(#)CONFIG.c      5.6 (Berkeley) 4/16/91";
#endif /* not lint */

    /*
     * make sure you are editing
     *        CONFIG.c
     * editing config.c won't work
     */

    /*
     * the version of translator
     */
const char *version        = "3.1 (6/6/93)";

    /*
     * the location of the error strings
     * and the length of the path to it
     * (in case of execution as a.something)
     */
const char *err_file       = "/usr/libdata/pascal/ERRORSTRINGS";
int    err_pathlen         = sizeof("/usr/libdata/pascal/")-1;

    /*
     * the location of the short explanation
     * and the length of the path to it
     * the null at the end is so pix can change it to pi'x' from pi.
     */
const char   how_string[]  = "/usr/libdata/pascal/HOWFILE\0";
const char   *how_file     = how_string;
const int    how_pathlen   = sizeof("/usr/libdata/pascal/")-1;
    
    /*
     * things about the interpreter.
     * these are not used by the compiler.
     */
#ifndef PC
const char   *px_header    = "/usr/libexec/pascal/px_header";
                                                /* px_header's name */
#endif

#ifndef PXP
const char   *pi_comp      = "/usr/local/bin/pi";     /* the compiler's name */
const char   *px_intrp     = "/usr/local/bin/px";     /* the interpreter's name */
const char   *px_debug     = "/usr/local/bin/pdx";    /* the debugger's name */
#endif
