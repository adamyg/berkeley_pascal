@echo off
setlocal enableDelayedExpansion

set PIOPTIONS=-krlzyxg

if "%1" == "--owc19" (
	set PI=..\..\bin.owc19\debug\pi %PIOPTIONS%

) else if "%1" == "--vs140" (
	set PI=..\..\bin.vs140\debug\pi %PIOPTIONS%
) else if "%1" == "--vc2015" (
	set PI=..\..\bin.vs140\debug\pi %PIOPTIONS%

) else if "%1" == "--vs141" (
	set PI=..\..\bin.vs141\debug\pi %PIOPTIONS%
) else if "%1" == "--vc2017" (
	set PI=..\..\bin.vs140\debug\pi %PIOPTIONS%

) else if "%1" == "--vs142" (
	set PI=..\..\bin.vs142\debug\pi %PIOPTIONS%
) else if "%1" == "--vc2019" (
	set PI=..\..\bin.vs142\debug\pi %PIOPTIONS%

) else (
	echo Error: missing target toolchain.
	echo .
	echo Usage: compile_tests [--toolchain]
	echo toolchains:
	echo  owc19             Open Watcom 1.9
	echo  vs140/vc2015      Microsoft 2015
	echo  vs141/vc2017      Microsoft 2017
	echo  vs142/vc2019      Microsoft 2019
	goto end
)

if not exist test mkdir test
del   /Q test\*
%PI%  -o test\align        ..\src\align.p                > test\align.lst
%PI%  -o test\ancestor     ..\src\ancestor.p             > test\ancestor.lst
%PI%  -o test\ancestor2    ..\src\ancestor2.p            > test\ancestor2.lst
%PI%  -o test\backtracerr  ..\src\backtracerr.p          > test\backtracerr.lst
%PI%  -o test\basic        ..\src\basic.p                > test\basic.lst
%PI%  -o test\bench        ..\src\bench.p                > test\bench.lst
%PI%  -o test\binrdwt      ..\src\binrdwt.p              > test\binrdwt.lst
%PI%  -o test\case         ..\src\case.p                 > test\case.lst
%PI%  -o test\cfor         ..\src\cfor.p                 > test\cfor.lst
%PI%  -o test\clock        ..\src\clock.p                > test\clock.lst
%PI%  -o test\cntchars     ..\src\cntchars.p             > test\cntchars.lst
%PI%  -o test\cutter       ..\src\cutter.p               > test\cutter.lst
%PI%  -o test\eightqueens  ..\src\eightqueens.p          > test\eightqueens.lst
%PI%  -o test\expotst      ..\src\expotst.p              > test\expotst.lst
%PI%  -o test\fay          ..\src\fay.p                  > test\fay.lst
%PI%  -o test\ffunc        ..\src\ffunc.p                > test\ffunc.lst
%PI%  -o test\fnested      ..\src\fnested.p              > test\fnested.lst
%PI%  -o test\form         ..\src\form.p                 > test\form.lst
%PI%  -o test\fproc        ..\src\fproc.p                > test\fproc.lst
%PI%  -o test\funcs        ..\src\funcs.p                > test\funcs.lst
%PI%  -o test\gencard      ..\src\gencard.p              > test\gencard.lst
%PI%  -o test\gordon       ..\src\gordon.p               > test\gordon.lst
%PI%  -o test\hamachi      ..\src\hamachi.p              > test\hamachi.lst
%PI%  -o test\hanoi        ..\src\hanoi.p                > test\hanoi.lst
%PI%  -o test\hugereal     ..\src\hugereal.p             > test\hugereal.lst
%PI%  -o test\index        ..\src\index.p                > test\index.lst
%PI%  -o test\insan        ..\src\insan.p                > test\insan.lst
%PI%  -o test\math         ..\src\math.p                 > test\math.lst
%PI%  -o test\nmaze        ..\src\nmaze.p                > test\nmaze.lst
%PI%  -o test\nonlocgoto   ..\src\nonlocgoto.p           > test\nonlocgoto.lst
%PI%  -o test\numericio    ..\src\numericio.p            > test\numericio.lst
%PI%  -o test\palindromes  ..\src\palindromes.p          > test\palindromes.lst
%PI%  -o test\pcerror      ..\src\pcerror.p              > test\pcerror.lst
REM %PI%  -o test\pcextn       ..\src\pcextn.p               > test\pcextn.lst
%PI%  -o test\permute      ..\src\permute.p              > test\permute.lst
%PI%  -o test\peter        ..\src\peter.p                > test\peter.lst
%PI%  -o test\piextn       ..\src\piextn.p               > test\piextn.lst
%PI%  -o test\powersoftwo  ..\src\powersoftwo.p          > test\powersoftwo.lst
%PI%  -o test\pretty       ..\src\pretty.p               > test\pretty.lst
%PI%  -o test\primes       ..\src\primes.p               > test\primes.lst
%PI%  -o test\procs        ..\src\procs.p                > test\procs.lst
%PI%  -o test\pvs1         ..\src\pvs1.p                 > test\pvs1.lst
%PI%  -o test\pvs2         ..\src\pvs2.p                 > test\pvs2.lst
%PI%  -o test\pvs3         ..\src\pvs3.p                 > test\pvs3.lst
%PI%  -o test\quicksort    ..\src\quicksort.p            > test\quicksort.lst
%PI%  -o test\random       ..\src\random.p               > test\random.lst
%PI%  -o test\rdpal        ..\src\rdpal.p                > test\rdpal.lst
%PI%  -o test\rdwt         ..\src\rdwt.p                 > test\rdwt.lst
%PI%  -o test\real         ..\src\real.p                 > test\real.lst
%PI%  -o test\realout      ..\src\realout.p              > test\realout.lst
%PI%  -o test\reset        ..\src\reset.p                > test\reset.lst
%PI%  -o test\scaltst      ..\src\scaltst.p              > test\scaltst.lst
%PI%  -o test\sequin       ..\src\sequin.p               > test\sequin.lst
%PI%  -o test\setest       ..\src\setest.p               > test\setest.lst
%PI%  -o test\setret       ..\src\setret.p               > test\setret.lst
%PI%  -o test\shoemake     ..\src\shoemake.p             > test\shoemake.lst
%PI%  -o test\size         ..\src\size.p                 > test\size.lst
%PI%  -o test\sort         ..\src\sort.p                 > test\sort.lst
%PI%  -o test\strngs       ..\src\strngs.p               > test\strngs.lst
%PI%  -o test\t01          ..\src\t01.p                  > test\t01.lst
%PI%  -o test\t03          ..\src\t03.p                  > test\t03.lst
%PI%  -o test\t04          ..\src\t04.p                  > test\t04.lst
%PI%  -o test\t05          ..\src\t05.p                  > test\t05.lst
%PI%  -o test\t06          ..\src\t06.p                  > test\t06.lst
%PI%  -o test\t07          ..\src\t07.p                  > test\t07.lst
%PI%  -o test\t08          ..\src\t08.p                  > test\t08.lst
%PI%  -o test\t09          ..\src\t09.p                  > test\t09.lst
%PI%  -o test\t10          ..\src\t10.p                  > test\t10.lst
%PI%  -o test\t11          ..\src\t11.p                  > test\t11.lst
%PI%  -o test\t12          ..\src\t12.p                  > test\t12.lst
%PI%  -o test\t13          ..\src\t13.p                  > test\t13.lst
%PI%  -o test\t14          ..\src\t14.p                  > test\t14.lst
%PI%  -o test\t15          ..\src\t15.p                  > test\t15.lst
%PI%  -o test\t16          ..\src\t16.p                  > test\t16.lst
%PI%  -o test\t17          ..\src\t17.p                  > test\t17.lst
%PI%  -o test\t18          ..\src\t18.p                  > test\t18.lst
%PI%  -o test\t19          ..\src\t19.p                  > test\t19.lst
%PI%  -o test\t20          ..\src\t20.p                  > test\t20.lst
%PI%  -o test\t21          ..\src\t21.p                  > test\t21.lst
%PI%  -o test\testeoln     ..\src\testeoln.p             > test\testeoln.lst
%PI%  -o test\testio       ..\src\testio.p               > test\testio.lst
%PI%  -o test\transpose    ..\src\transpose.p            > test\transpose.lst
%PI%  -o test\wnj          ..\src\wnj.p                  > test\wnj.lst
set PIOPTIONS=
set PI=
:done
:end
