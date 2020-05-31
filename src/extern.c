#include <whoami.h>
#include <0.h>
#include <tree_ty.h> /* must be included for yy.h */
#include <yy.h>
#include <tmps.h>

#ifdef DEBUG
bool                 fulltrace = 0, errtrace = 0, testtrace = 0;
#endif
char                 opts[ 'z' - 'A' + 1 ] = {0};
short                optstk[ 'z' - 'A' + 1 ] = {0};

bool                 monflg = 0;
#ifdef PC
bool                 profflag = 0;
#endif
bool                 Recovery = 0;
bool                 Eholdnl = 0, Enocascade = 0;
bool                 eflg = 0;
int                  cgenflg = 0;
bool                 syneflg = 0;
int                  efil = 0;
#ifdef OBJ
int                  ofil = -1;
short                obuf[518] = {0};
#endif
bool                 Enoline = 0;
int                  pnumcnt = 0;
struct nl            *Fp = 0;
short                bn = 0, cbn = 0;
#ifdef PXP
short                lastbn = 0;
#endif
short                line = 0;
bool                 dfiles[DSPLYSZ] = {0};
#ifndef PXP
struct con           con = {0};
struct set           set = {0};
#endif
short                gocnt = 0;
int                  parts[DSPLYSZ] = {0};
bool                 divchk = 0;
bool                 divflg = 0;
bool                 errcnt[DSPLYSZ] = {0};
struct nl            *forechain = 0;
struct nl            *withlist = 0;
struct nl            *intset = 0;
struct nl            *program = 0;
bool                 progseen = 0;
short                level = 0;
struct nl            *gotos[DSPLYSZ] = {0};
bool                 noreach = 0;
char                 *lc = 0;
char                 **pflist = 0;
short                pflstc = 0;
short                pfcnt = 0;
char                 *filename = 0;
time_t               tvec = 0;
#ifdef PXP
time_t               ptvec; /* time profiled */
#endif
#ifdef PC
int                  ftnno = 0;
FILE                 *pcstream = 0;
#endif /*PC*/

FILE                 *ibp = 0;
char                 yyprtd = 0;
int                  yyline = 0;
int                  yyseqid = 0;
int                  yysavc = 0;
int                  yylinpt = 0;
struct yytok         Y = {0}, OY = {0};
int                  *Ps = 0;
int                  N = 0;
char                 yyResume = 0;
char                 dquote = 0;
#ifndef PC
#ifndef OBJ
char                 errout = 0;
#endif
#endif
int                  yyidhave = 0, yyidwant = 0;
int                  yyshifts = 0, yyOshifts = 0;
int                  yytokcnt = 0;
int                  yywhcnt = 0;
#ifdef PXP
struct W             yyw[MAXDEPTH + 1] = {0};
struct W             *yypw = 0;
#endif
struct comment       *cmhp = 0;
struct om            sizes[DSPLYSZ] = {0};

int                  yystate = 0;
union  semstack      yyv[MAXDEPTH] = {0};
int                  yys[MAXDEPTH] = {0};




