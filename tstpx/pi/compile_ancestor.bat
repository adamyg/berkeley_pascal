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
%PI%  -o test\ancestor     ..\src\ancestor.p             > test\ancestor.lst
%PI%  -o test\ancestor2    .\ancestor2.p                 > test\ancestor2.lst
set PIOPTIONS=
set PI=
:done
:end
