@echo off
setlocal enableDelayedExpansion

if "%1" == "--owc19" (
	set PX=..\..\..\bin.owc19\debug\px

) else if "%1" == "--vs140" (
	set PX=..\..\..\bin.vs140\debug\px
) else if "%1" == "--vc2015" (
	set PX=..\..\..\bin.vs140\debug\px

) else if "%1" == "--vs141" (
	set PX=..\..\..\bin.vs141\debug\px
) else if "%1" == "--vc2017" (
	set PX=..\..\..\bin.vs141\debug\px

) else if "%1" == "--vs142" (
	set PX=..\..\..\bin.vs142\debug\px
) else if "%1" == "--vc2019" (
	set PX=..\..\..\bin.vs142\debug\px

) else (
	echo Error: missing target toolchain.
	echo .
	echo Usage: run_tests [--toolchain]
	echo toolchains:
	echo  owc19             Open Watcom 1.9
	echo  vs140/vc2015      Microsoft 2015
	echo  vs141/vc2017      Microsoft 2017
	echo  vs142/vc2019      Microsoft 2019
	goto end
)

cd    test
%PX%  funcs                                              > funcs.out          2>&1
%PX%  fay                                                > fay.out            2>&1
%PX%  align                                              > align.out          2>&1
%PX%  ancestor                                           > ancestor.out       2>&1
%PX%  ancestor2                                          > ancestor2.out      2>&1
%PX%  backtracerr                                        > backtracerr.out    2>&1
%PX%  basic                < ..\..\in\basic.in           > basic.out          2>&1
%PX%  bench                                              > bench.out          2>&1
%PX%  binrdwt                                            > binrdwt.out        2>&1
%PX%  case                                               > case.out           2>&1
%PX%  cfor                 < ..\..\in\cfor.in            > cfor.out           2>&1
%PX%  clock                                              > clock.out          2>&1
%PX%  cntchars             < ..\..\in\cntchars.in        > cntchars.out       2>&1
%PX%  cutter                                             > cutter.out         2>&1
%PX%  eightqueens                                        > eightqueens.out    2>&1
%PX%  expotst              < ..\..\in\expotst.in         > expotst.out        2>&1
%PX%  fay                                                > fay.out            2>&1
%PX%  ffunc                                              > ffunc.out          2>&1
%PX%  fnested                                            > fnested.out        2>&1
%PX%  form                                               > form.out           2>&1
%PX%  fproc                                              > fproc.out          2>&1
%PX%  funcs                                              > funcs.out          2>&1
%PX%  gencard                                            > gencard.out        2>&1
%PX%  gordon                                             > gordon.out         2>&1
%PX%  hamachi                                            > hamachi.out        2>&1
%PX%  hanoi                < ..\..\in\hanoi.in           > hanoi.out          2>&1
%PX%  hugereal             < ..\..\in\hugereal.in        > hugereal.out       2>&1
%PX%  index                < ..\..\in\index.in           > index.out          2>&1
%PX%  insan                < ..\..\in\insan.in           > insan.out          2>&1
%PX%  math                                               > math.out           2>&1
%PX%  nmaze                < ..\..\in\nmaze.in           > nmaze.out          2>&1
%PX%  nonlocgoto                                         > nonlocgoto.out     2>&1
%PX%  numericio                                          > numericio.out      2>&1
%PX%  palindromes                                        > palindromes.out    2>&1
REM %PX%  pcextn                                             > pcextn.out     2>&1
%PX%  permute                                            > permute.out        2>&1
%PX%  peter                < ..\..\in\peter.in           > peter.out          2>&1
%PX%  piextn                                             > piextn.out         2>&1
%PX%  powersoftwo                                        > powersoftwo.out    2>&1
%PX%  pretty               < ..\..\in\pretty.in          > pretty.out         2>&1
%PX%  primes                                             > primes.out         2>&1
%PX%  procs                < ..\..\in\procs.in           > procs.out          2>&1
%PX%  pvs1                                               > pvs1.out           2>&1
%PX%  quicksort                                          > quicksort.out      2>&1
%PX%  random                                             > random.out         2>&1
%PX%  rdpal                < ..\..\in\rdpal.in           > rdpal.out          2>&1
%PX%  rdwt                 < ..\..\in\rdwt.in            > rdwt.out           2>&1
%PX%  real                                               > real.out           2>&1
%PX%  realout                                            > realout.out        2>&1
%PX%  scaltst                                            > scaltst.out        2>&1
%PX%  sequin                                             > sequin.out         2>&1
%PX%  setest               < ..\..\in\setest.in          > setest.out         2>&1
%PX%  setret                                             > setret.out         2>&1
%PX%  shoemake                                           > shoemake.out       2>&1
%PX%  size                 < ..\..\in\size.in            > size.out           2>&1
%PX%  sort                                               > sort.out           2>&1
%PX%  strngs                                             > strngs.out         2>&1
%PX%  t01                                                > t01.out            2>&1
%PX%  t03                                                > t03.out            2>&1
%PX%  t04                                                > t04.out            2>&1
%PX%  t05                                                > t05.out            2>&1
%PX%  t06                                                > t06.out            2>&1
%PX%  t07                                                > t07.out            2>&1
%PX%  t08                                                > t08.out            2>&1
%PX%  t09                                                > t09.out            2>&1
%PX%  t10                                                > t10.out            2>&1
%PX%  t11                                                > t11.out            2>&1
%PX%  t12                                                > t12.out            2>&1
%PX%  t13                                                > t13.out            2>&1
%PX%  t14                                                > t14.out            2>&1
%PX%  t15                                                > t15.out            2>&1
%PX%  t16                  < ..\..\in\t16.in             > t16.out            2>&1
%PX%  t17                                                > t17.out            2>&1
%PX%  t18                                                > t18.out            2>&1
%PX%  t19                                                > t19.out            2>&1
%PX%  t20                                                > t20.out            2>&1
%PX%  t21                                                > t21.out            2>&1
%PX%  testio                                             > testio.out         2>&1
%PX%  transpose                                          > transpose.out      2>&1
%PX%  wnj                                                > wnj.out            2>&1

@echo Note: The following programs should fail during execution:
@echo .
@echo     pvs2, pvs3, reset, testeoln, pcerror
@echo .

%PX%  pvs2                                               > pvs2.out           2>&1
%PX%  pvs3                                               > pvs3.out           2>&1
%PX%  reset                                              > reset.out          2>&1
%PX%  pcerror              < ..\..\in\pcerror.in         > pcerror.out        2>&1
%PX%  testeoln                                           > testeoln.out       2>&1

:done
set   PX=
cd    ..

:end


