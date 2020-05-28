/* libmain - flex run-time support library "main" function */

/* $Header: /cvs/berkeley_pascal/contrib/flex/flex-2.5.2/libmain.c,v 1.1 2020/05/28 18:03:33 cvsuser Exp $ */

extern int yylex();

int main( argc, argv )
int argc;
char *argv[];
	{
	return yylex();
	}
