@echo off
set   PX=..\..\..\bin.owc19\px
cd    test
%PX%  funcs                                              > funcs.out
%PX%  fay                                                > fay.out
%PX%  align                                              > align.out
%PX%  ancestor                                           > ancestor.out
%PX%  ancestor2                                          > ancestor2.out
%PX%  backtracerr                                        > backtracerr.out
%PX%  basic                < ..\..\in\basic.in           > basic.out
%PX%  bench                                              > bench.out
%PX%  binrdwt                                            > binrdwt.out
%PX%  case                                               > case.out
%PX%  cfor                 < ..\..\in\cfor.in            > cfor.out
%PX%  clock                                              > clock.out
%PX%  cntchars             < ..\..\in\cntchars.in        > cntchars.out
%PX%  cutter                                             > cutter.out
%PX%  eightqueens                                        > eightqueens.out
%PX%  expotst              < ..\..\in\expotst.in         > expotst.out
%PX%  fay                                                > fay.out
%PX%  ffunc                                              > ffunc.out
%PX%  fnested                                            > fnested.out
%PX%  form                                               > form.out
%PX%  fproc                                              > fproc.out
%PX%  funcs                                              > funcs.out
%PX%  gencard                                            > gencard.out
%PX%  gordon                                             > gordon.out
%PX%  hamachi                                            > hamachi.out
%PX%  hanoi                < ..\..\in\hanoi.in           > hanoi.out
%PX%  hugereal             < ..\..\in\hugereal.in        > hugereal.out
%PX%  index                < ..\..\in\index.in           > index.out
%PX%  insan                < ..\..\in\insan.in           > insan.out
%PX%  math                                               > math.out
%PX%  nmaze                < ..\..\in\nmaze.in           > nmaze.out
%PX%  nonlocgoto                                         > nonlocgoto.out
%PX%  numericio                                          > numericio.out
%PX%  palindromes                                        > palindromes.out
REM %PX%  pcextn                                             > pcextn.out
%PX%  permute                                            > permute.out
%PX%  peter                < ..\..\in\peter.in           > peter.out
%PX%  powersoftwo                                        > powersoftwo.out
%PX%  pretty               < ..\..\in\pretty.in          > pretty.out
%PX%  primes                                             > primes.out
%PX%  procs                < ..\..\in\procs.in           > procs.out
%PX%  pvs1                                               > pvs1.out
%PX%  quicksort                                          > quicksort.out
%PX%  random                                             > random.out
%PX%  rdpal                < ..\..\in\rdpal.in           > rdpal.out
%PX%  rdwt                 < ..\..\in\rdwt.in            > rdwt.out
%PX%  real                                               > real.out
%PX%  realout                                            > realout.out
%PX%  scaltst                                            > scaltst.out
%PX%  sequin                                             > sequin.out
%PX%  setest               < ..\..\in\setest.in          > setest.out
%PX%  setret                                             > setret.out
%PX%  shoemake                                           > shoemake.out
%PX%  size                 < ..\..\in\size.in            > size.out
%PX%  sort                                               > sort.out
%PX%  strngs                                             > strngs.out
%PX%  t01                                                > t01.out
%PX%  t03                                                > t03.out
%PX%  t04                                                > t04.out
%PX%  t05                                                > t05.out
%PX%  t06                                                > t06.out
%PX%  t07                                                > t07.out
%PX%  t08                                                > t08.out
%PX%  t09                                                > t09.out
%PX%  t10                                                > t10.out
%PX%  t11                                                > t11.out
%PX%  t12                                                > t12.out
%PX%  t13                                                > t13.out
%PX%  t14                                                > t14.out
%PX%  t15                                                > t15.out
%PX%  t16                  < ..\..\in\t16.in             > t16.out
%PX%  t17                                                > t17.out
%PX%  t18                                                > t18.out
%PX%  t19                                                > t19.out
%PX%  t20                                                > t20.out
%PX%  t21                                                > t21.out
%PX%  testio                                             > testio.out
%PX%  transpos                                           > transpos.out
%PX%  wnj                                                > wnj.out

@echo Note: The following programs should fail during execution:
@echo .
@echo     pvs2, pvs3, reset, testeoln, pcerror
@echo .

%PX%  pvs2                                               > pvs2.out
%PX%  pvs3                                               > pvs3.out
%PX%  reset                                              > reset.out
%PX%  pcerror              < ..\..\in\pcerror.in         > pcerror.out
%PX%  testeoln                                           > testeoln.out

:done
set   PX=
cd    ..
