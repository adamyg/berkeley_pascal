/*-
 * Copyright (c) 1980 The Regents of the University of California.
 * All rights reserved.
 */

#ifndef lint
static char sccsid[] = "@(#)ex_data.c	7.11 (Berkeley) 4/17/91";
#endif /* not lint */

#include "ex.h"
#include "ex_tty.h"

/*
 * Initialization of option values.
 * The option #defines in ex_vars.h are made
 * from this file by the script makeoptions.
 *
 * These initializations are done char by char instead of as strings
 * to confuse xstr so it will leave them alone.
 */
char	direct[ONMSZ] =
#if defined(_WIN32) || defined(WIN32) || defined(MSDOS)
	{'/', 't', 'm', 'p'};
#elif defined(vms)
	{'t', 'm', 'p', ':'}; 
#else
	{'/', 'v', 'a', 'r', '/', 't', 'm', 'p'};
#endif

char	paragraphs[ONMSZ] = {
	'I', 'P', 'L', 'P', 'P', 'P', 'Q', 'P',		/* -ms macros */
	'P', ' ', 'L', 'I',				/* -mm macros */
	'p', 'p', 'l', 'p', 'i', 'p',			/* -me macros */
	'b', 'p'					/* bare nroff */
};

char	sections[ONMSZ] = {
	'N', 'H', 'S', 'H',				/* -ms macros */
	'H', ' ', 'H', 'U',				/* -mm macros */
	'n', 'h', 's', 'h'				/* -me macros */
};

char	shell[ONMSZ] =
	{ '/', 'b', 'i', 'n', '/', 's', 'h' };

char	tags[ONMSZ] = {
	't', 'a', 'g', 's', ' ',
	'/', 'v', 'a', 'r', '/', 'd', 'b', '/', 'l', 'i', 'b', 'c', '.',
	    't', 'a', 'g', 's'
};

char	ttytype[ONMSZ] =
	{ 'd', 'u', 'm', 'b' };

short	COLUMNS = 80;
short	LINES = 24;

struct	option options[NOPTS + 1] = {
	{"autoindent",		"ai",		ONOFF,		0,	0,	0},
	{"autoprint",		"ap",		ONOFF,		1,	1,	0},
	{"autowrite",		"aw",		ONOFF,		0,	0,	0},
	{"beautify",		"bf",		ONOFF,		0,	0,	0},
	{"directory",		"dir",		STRING,		0,	0,	direct},
	{"edcompatible",	"ed",		ONOFF,		0,	0,	0},
	{"errorbells",		"eb",		ONOFF,		0,	0,	0},
	{"hardtabs",		"ht",		NUMERIC,	8,	8,	0},
	{"ignorecase",		"ic",		ONOFF,		0,	0,	0},
//#ifndef UNIX_SBRK
	{"linelimit",		"ll",		NUMERIC,	10000,	10000,	0},
//#endif
	{"lisp",		NULL,		ONOFF,		0,	0,	0},
	{"list",		NULL,		ONOFF,		0,	0,	0},
	{"magic",		NULL,		ONOFF,		1,	1,	0},
	{"mesg",		NULL,		ONOFF,		1,	1,	0},
	{"modeline",		NULL,		ONOFF,		0,	0,	0},
	{"number",		"nu",		ONOFF,		0,	0,	0},
	{"open",		NULL,		ONOFF,		1,	1,	0},
	{"optimize",		"opt",		ONOFF,		0,	0,	0},
	{"paragraphs",		"para",		STRING,		0,	0,	paragraphs},
	{"prompt",		NULL,		ONOFF,		1,	1,	0},
	{"readonly",		"ro",		ONOFF,		0,	0,	0},
	{"redraw",		NULL,		ONOFF,		0,	0,	0},
	{"remap",		NULL,		ONOFF,		1,	1,	0},
	{"report",		NULL,		NUMERIC,	5,	5,	0},
	{"scroll",		"scr",		NUMERIC,	12,	12,	0},
	{"sections",		"sect",		STRING,		0,	0,	sections},
	{"shell",		"sh",		STRING,		0,	0,	shell},
	{"shiftwidth",		"sw",		NUMERIC,	TABS,	TABS,	0},
	{"showmatch",		"sm",		ONOFF,		0,	0,	0},
	{"slowopen",		"slow",		ONOFF,		0,	0,	0},
	{"sourceany",		NULL,		ONOFF,		0,	0,	0},
	{"tabstop",		"ts",		NUMERIC,	TABS,	TABS,	0},
	{"taglength",		"tl",		NUMERIC,	0,	0,	0},
	{"tags",		"tag",		STRING,		0,	0,	tags},
	{"term",		NULL,		OTERM,		0,	0,	ttytype},
	{"terse",		NULL,		ONOFF,		0,	0,	0},
	{"timeout",		"to",		ONOFF,		1,	1,	0},
	{"ttytype",		"tty",		OTERM,		0,	0,	ttytype},
	{"warn",		NULL,		ONOFF,		1,	1,	0},
	{"window",		"wi",		NUMERIC,	23,	23,	0},
	{"wrapscan",		"ws",		ONOFF,		1,	1,	0},
	{"wrapmargin",		"wm",		NUMERIC,	0,	0,	0},
	{"writeany",		"wa",		ONOFF,		0,	0,	0},
	{0,			0,		0,		0,	0,	0},
};
