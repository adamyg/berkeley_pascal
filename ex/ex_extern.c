/*-
 * Copyright (c) 1980 The Regents of the University of California.
 * All rights reserved.
 */

#ifndef lint
static char sccsid[] = "@(#)ex_extern.c	7.5 (Berkeley) 4/17/91";
#endif /* not lint */

/*
 * Provide defs of the global variables.
 * This crock is brought to you by the turkeys
 * who broke Unix on the Bell Labs 3B machine,
 * all in the name of "but that's what the C
 * book says!"
 */

# define var 	/* nothing */
# include "ex.h"
# include "ex_argv.h"
# include "ex_re.h"
# include "ex_temp.h"
# include "ex_tty.h"
# include "ex_tune.h"
# include "ex_vars.h"
# include "ex_vis.h"
