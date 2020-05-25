/* -*- mode: c; tabs: 8; hard-tabs: yes; -*- */

#define B300	1
#define B1200	2
#define B2400	3
#define B115700 4

struct sgttyb {
	int	sg_flags;
	int     sg_erase;
	int     sg_kill;
};

int		tgetflag(const char *name);
int		tgetnum(const char *name);
char *		tgetstr(const char *name, char **b);

char *		tgoto(const char *s, int col, int line);
void		tputs(const char *s, int p, void (*)(int));




