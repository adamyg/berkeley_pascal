/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */
/*
   Min requirements for visual interface mode

   CA      Cursor addressible.
   UP      Upline.
   OS      Overstrike works.
   EO      Can erase overstrikes with ' '.
   CL      Clear screen.
   NS      No scroll - linefeed at bottom won't scroll.
   SF      Scroll forwards.
 */

#define  WINDOWS_MEAN_AND_LEAN
#include <windows.h>

#undef	 DELETE
#undef	 IN

#include "ex.h"
#include "ex_tty.h"
#include "ex_vis.h"

#define E_CURSOR_UP	'A'			/* DEC/vt110 */
#define E_CURSOR_DOWN	'D'
#define E_CURSOR_FWD	'C'
#define E_CURSOR_BKW	'D'
#define E_CURSOR_ADDR	'H'

#define E_CLR_SCREEN	'J'
#define E_CLR_LINE	'K'

#define E_UPDATE	'U'

static int		Compare(const char *name, const char *sym);
static int		ExecuteESC( const char *s, int p1 );
static void		ConsoleSize( HANDLE hConsole, int *cols, int *lines );
static void		ConsoleCursor( HANDLE hConsole, WORD col, WORD line );
static void		ConsoleClear( HANDLE hConsole );

short			ospeed = B115700;	/* Output speed (115k) */

static WORD		w_col, w_line;		/* last tgoto() value */

char			PC = ' ';		/* Pad character */
char *			BC = 0;                 /* Back cursor */
char *			UP = "";		/* Upline */


/* 
 * tgetflag ---	
 *	tgetflag retrieves a capability value that is a boolean value. If 
 *	the capability name is present in the terminal description, tgetflag 
 *	returns 1; otherwise, it returns 0. 
 */
int
tgetflag(const char *name)
{
	int val = 0;

	if (Compare(name, "ca"))		/* cursor addressable */
		val = 1;                        
	else if (Compare(name, "os"))		/* overstrike works */
		val = 1;                        
	else if (Compare(name, "eo"))		/* Can erase overstrikes with ' ' */
		val = 1;                        

	return (val);
}


/* 
 * tgetnum ---	
 *	tgetnum retreives a capability value that is numeric. The argument 
 *	name is the two-letter code name of the capability.  if the 
 *	capability is present, tgetnum returns the numeric value 
 *	(which is nonnegative). If the capability is not mentioned in 
 *	int the terminal description, tgetnum returns -1.
 */
int
tgetnum(const char *name)
{
	HANDLE	hConsole;
	int	val = -1;

	hConsole = GetStdHandle( STD_INPUT_HANDLE );

	if (Compare(name, "co"))		/* columns */
		ConsoleSize( hConsole, &val, NULL );

	else if (Compare(name, "li"))		/* lines */
		ConsoleSize( hConsole, NULL, &val );

	return (val);
}


/*
 * tgetstr ---
 *	Use tgetstr to get a string value. It returns a pointer to a string 
 *	which is the capability value, or a null pointer if the capability 
 *	is not present in the terminal description. 
 */
char *
tgetstr(const char *name, char **b)
{	
	char		buffer[4] = { ESCAPE, '[', '\0', '\0' };
	char *		p = NULL;
	char *		s;

	if (Compare(name, "up")) { 		/* update */
		buffer[2] = E_UPDATE;
                p = buffer;

	}  else if (Compare(name, "cl")) { 	/* clear screen */
		buffer[2] = E_CLR_SCREEN;
		p = buffer;
	}

	if (p && *b)
	{
		s = *b; 			/* start of buffer */
		while (*p) {			/* copy to buffer */
			**b = *p++; (*b)++;
		}
		**b = '\0'; (*b)++;		/* terminate */
		p = s;
	}
	return ((char *)p);
}


char *
tgoto(const char *s, int col, int line)
{
	static char	buf[] = { ESCAPE, '[', E_CURSOR_ADDR, '\0' };

	w_col = (WORD)col;
	w_line = (WORD)line;
	return (buf);
}


void
tputs( const char *s, int p1, void (*putf)(int) )
{
	if (s == NULL)
		return;
	while (*s)
		if (s[0] == ESCAPE && s[1] == '[')
			s += 2 + ExecuteESC( s+2, p1 );
		else
		{
			(putf)(*s++);
		}
}


static int
Compare(const char *name, const char *sym)
{
	return !strnicmp(name, sym, 2);
}


static int
ExecuteESC( const char *s, int p1 )
{
	HANDLE	hConsole;

	hConsole = GetStdHandle( STD_INPUT_HANDLE );
	switch (*s)
	{
	case E_CURSOR_ADDR:
		ConsoleCursor( hConsole, w_col, w_line );
		return (1);

	case E_CLR_SCREEN:
		ConsoleClear( hConsole );
		return (1);
	}
	return (0);
}


static void
ConsoleSize( HANDLE hConsole, int *cols, int *lines )
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if ( !GetConsoleScreenBufferInfo( hConsole, &csbi )) {
		csbi.dwSize.X = 80;
		csbi.dwSize.Y = 24;
	}
	if (cols)
		*cols = csbi.dwSize.X;
	if (lines)
		*lines = csbi.dwSize.Y;
}


static void
ConsoleCursor( HANDLE hConsole, WORD col, WORD line )
{
	COORD coordScreen;

	coordScreen.X = col;
	coordScreen.Y = line;
	SetConsoleCursorPosition( hConsole, coordScreen );
}


static void
ConsoleClear( HANDLE hConsole )
{
	COORD coordScreen = { 0, 0 };           // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if ( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
		return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if ( !FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
			dwConSize, coordScreen, &cCharsWritten ))
		return;

	// Get the current text attribute.
	if ( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
                return;

	// Set the buffer's attributes accordingly.
	if ( !FillConsoleOutputAttribute( hConsole, csbi.wAttributes,
			dwConSize, coordScreen, &cCharsWritten ))
		return;

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition( hConsole, coordScreen );
}
