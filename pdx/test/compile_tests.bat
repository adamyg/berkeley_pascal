@echo off
setlocal enableDelayedExpansion

set PIOPTIONS=-krlzyxg

if "%1" == "--owc19" (
	set PI=..\..\bin.owc19\pi %PIOPTIONS%

) else if "%1" == "--vs140" (
	set PI=..\..\bin.vs140\pi %PIOPTIONS%
) else if "%1" == "--vc2015" (
	set PI=..\..\bin.vs140\pi %PIOPTIONS%

) else if "%1" == "--vs141" (
	set PI=..\..\bin.vs141\pi %PIOPTIONS%
) else if "%1" == "--vc2017" (
	set PI=..\..\bin.vs140\pi %PIOPTIONS%

) else if "%1" == "--vs142" (
	set PI=..\..\bin.vs142\pi %PIOPTIONS%
) else if "%1" == "--vc2019" (
	set PI=..\..\bin.vs142\pi %PIOPTIONS%

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

if not exist test mkdir obj
del   /Q obj\*
%PI%  -o obj\args	args.p		> obj\args.lst
%PI%  -o obj\block	block.p		> obj\block.lst
%PI%  -o obj\bug	bug.p		> obj\bug.lst
%PI%  -o obj\call	call.p		> obj\call.lst
%PI%  -o obj\case	case.p		> obj\case.lst
%PI%  -o obj\enum	enum.p		> obj\enum.lst
%PI%  -o obj\enumbug	enumbug.p	> obj\enumbug.lst
%PI%  -o obj\file	file.p		> obj\file.lst
%PI%  -o obj\fpe	fpe.p		> obj\fpe.lst
%PI%  -o obj\fproc	fproc.p		> obj\fproc.lst
%PI%  -o obj\goto	goto.p		> obj\goto.lst
%PI%  -o obj\overflow	overflow.p	> obj\overflow.lst
%PI%  -o obj\parall	parall.p	> obj\parall.lst
%PI%  -o obj\pcerror	pcerror.p	> obj\pcerror.lst
%PI%  -o obj\pointer	pointer.p	> obj\pointer.lst
%PI%  -o obj\recur	recur.p		> obj\recur.lst
%PI%  -o obj\setest	setest.p	> obj\setest.lst
%PI%  -o obj\simple	simple.p	> obj\simple.lst
%PI%  -o obj\small	small.p		> obj\small.lst
%PI%  -o obj\test	test.p		> obj\test.lst
%PI%  -o obj\varnt	varnt.p		> obj\varnt.lst
%PI%  -o obj\varparam	varparam.p	> obj\varparam.lst
set PIOPTIONS=
set PI=
:done
:end
