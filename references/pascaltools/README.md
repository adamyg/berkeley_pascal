## Software Tools in Pascal programs

	Source: https://www.cs.princeton.edu/~bwk/btl.mirror/

### Software Tools in Pascal

	Copyright (C) 1981 by Bell Laboratories, Inc., and Whitesmiths Ltd.

	This tape or disk contains all of the programs from
	Software Tools in Pascal, plus the documentation.
	There are 361 files (8500 lines; 210000 characters).
	The format of the tape is 800 bpi 9 track ASCII in 512 byte blocks.
	Each source line is terminated by an ASCII newline character;
	each file is introduced by a line of the form

	-h- directory/filename number-of-bytes

	as in the archive program of Chapter 3.
	The "number-of-bytes" field includes the copyright notice
	and the terminating newline.
	The "directory" is intended to help you assign the
	files to the proper programs.  Directories are:

	UCBPRIMS	primitives for UCB Pascal
	WSPRIMS		primitives for Whitesmiths Pascal
	UCSDPRIMS	primitives for UCSD Pascal
	UTIL		utility routines common to all programs

	INTRO		programs from Chapter 1
	FILTERS		all programs from Chapter 2 except translit
	TRANSLIT	translit program from Chapter 2
	FILEIO		early programs in Chapter 3
	PRINT		print programs from Chapter 3
	ARCHIVE		archive program from Chapter 3
	SORT		all programs from Chapter 4; mostly sorting
	EDIT		all programs from Chapters 5 and 6: find, change, edit
	FORMAT		format program from Chapter 7
	MACRO		define and macro processors from Chapter 8

	MAN		manual pages for programs
	PMAN		manual pages for primitives

	Within each group, files are presented in alphabetical order.
	Each file begins with a header like this one:

	-h- UCBPRIMS/getc.p 341

	which indicates that getc.p is part of the UCB primitives
	and is 341 bytes long.

	The list of file names and sizes from the tape follows.

		UCBPRIMS/close.p 315
		UCBPRIMS/create.p 890
		UCBPRIMS/getarg.p 642
		UCBPRIMS/getc.p 341
		UCBPRIMS/getcf.p 484
		UCBPRIMS/getline.p 453
		UCBPRIMS/initio.p 427
		UCBPRIMS/nargs.p 219
		UCBPRIMS/open.p 911
		UCBPRIMS/prims.p 379
		UCBPRIMS/putc.p 223
		UCBPRIMS/putcf.p 319
		UCBPRIMS/putstr.p 271
		UCBPRIMS/remove.p 360
		WSPRIMS/Base.p 2558
		WSPRIMS/addstr.p 353
		WSPRIMS/ctoi.p 502
		WSPRIMS/equal.p 303
		WSPRIMS/esc.p 497
		WSPRIMS/fcopy.p 372
		WSPRIMS/getc.p 466
		WSPRIMS/getline.p 597
		WSPRIMS/index.p 317
		WSPRIMS/istuff.p 867
		WSPRIMS/itoc.p 454
		WSPRIMS/length.p 251
		WSPRIMS/maxmin.p 353
		WSPRIMS/pcreate.p 379
		WSPRIMS/popen.p 367
		WSPRIMS/pputstr.p 368
		WSPRIMS/prims.p 2558
		WSPRIMS/putc.p 349
		WSPRIMS/putdec.p 432
		WSPRIMS/scopy.p 320
		WSPRIMS/seek.p 325
		WSPRIMS/tools.p 1726
		UCSDPRIMS/Call.p 108
		UCSDPRIMS/chars.p 1292
		UCSDPRIMS/close.p 393
		UCSDPRIMS/create.p 550
		UCSDPRIMS/endcmd.p 210
		UCSDPRIMS/fcopy.p 237
		UCSDPRIMS/fdalloc.p 553
		UCSDPRIMS/fgetcf.p 350
		UCSDPRIMS/fputcf.p 236
		UCSDPRIMS/ftalloc.p 360
		UCSDPRIMS/getarg.p 343
		UCSDPRIMS/getc.p 212
		UCSDPRIMS/getcf.p 378
		UCSDPRIMS/getkbd.p 1083
		UCSDPRIMS/getline.p 660
		UCSDPRIMS/initcmd.p 1389
		UCSDPRIMS/mustcreate.p 347
		UCSDPRIMS/mustopen.p 335
		UCSDPRIMS/nargs.p 174
		UCSDPRIMS/open.p 557
		UCSDPRIMS/prims.p 1899
		UCSDPRIMS/putc.p 189
		UCSDPRIMS/putcf.p 343
		UCSDPRIMS/putdec.p 304
		UCSDPRIMS/putstr.p 277
		UCSDPRIMS/remove.p 445
		UCSDPRIMS/strname.p 333
		UTIL/addstr.p 347
		UTIL/ctoi.p 502
		UTIL/equal.p 303
		UTIL/esc.p 462
		UTIL/fcopy.p 237
		UTIL/globdefs.p 2030
		UTIL/index.p 336
		UTIL/isalphanum.p 266
		UTIL/isdigit.p 201
		UTIL/isletter.p 245
		UTIL/islower.p 211
		UTIL/isupper.p 211
		UTIL/itoc.p 438
		UTIL/itoctest.p 312
		UTIL/length.p 251
		UTIL/max.p 212
		UTIL/min.p 212
		UTIL/mustcreate.p 347
		UTIL/mustopen.p 335
		UTIL/putdec.p 303
		UTIL/scopy.p 320
		UTIL/utility.p 507
		INTRO/charcount.p 279
		INTRO/copy.p 193
		INTRO/detab.p 648
		INTRO/linecount.p 299
		INTRO/settabs.p 288
		INTRO/tabpos.p 273
		INTRO/wholecopy.p 839
		INTRO/wordcount.p 442
		FILTERS/compress.p 597
		FILTERS/echo.p 381
		FILTERS/entab.p 802
		FILTERS/expand.p 558
		FILTERS/overstrike.p 788
		FILTERS/putrep.p 425
		FILTERS/settabs.p 288
		FILTERS/tabpos.p 273
		TRANSLIT/dodash.p 891
		TRANSLIT/makeset.p 373
		TRANSLIT/translit.p 1292
		TRANSLIT/xindex.p 410
		FILEIO/compare.p 872
		FILEIO/compare0.p 651
		FILEIO/concat.p 347
		FILEIO/dcompare.p 424
		FILEIO/diffmsg.p 289
		FILEIO/finclude.p 594
		FILEIO/getword.p 478
		FILEIO/include.p 483
		FILEIO/makecopy.p 432
		PRINT/fprint.p 806
		PRINT/head.p 486
		PRINT/print.p 517
		PRINT/print0.p 364
		PRINT/skip.p 200
		ARCHIVE/acopy.p 338
		ARCHIVE/addfile.p 489
		ARCHIVE/archive.p 1011
		ARCHIVE/archproc.p 442
		ARCHIVE/delete.p 549
		ARCHIVE/extract.p 799
		ARCHIVE/filearg.p 480
		ARCHIVE/fmove.p 304
		ARCHIVE/fsize.p 333
		ARCHIVE/fskip.p 302
		ARCHIVE/getfns.p 595
		ARCHIVE/gethdr.p 504
		ARCHIVE/getword.p 478
		ARCHIVE/help.p 195
		ARCHIVE/initarch.p 509
		ARCHIVE/makehdr.p 437
		ARCHIVE/notfound.p 318
		ARCHIVE/replace.p 487
		ARCHIVE/table.p 406
		ARCHIVE/tprint.p 392
		ARCHIVE/update.p 679
		SORT/bubble.p 371
		SORT/cmp.p 551
		SORT/cscopy.p 318
		SORT/exchange.p 245
		SORT/gname.p 408
		SORT/gopen.p 320
		SORT/gremove.p 323
		SORT/gtext.p 736
		SORT/inmemquick.p 684
		SORT/inmemsort.p 675
		SORT/kwic.p 257
		SORT/makefile.p 246
		SORT/merge.p 993
		SORT/ptext.p 397
		SORT/putrot.p 439
		SORT/quick.p 234
		SORT/reheap.p 594
		SORT/rotate.p 354
		SORT/rquick.p 754
		SORT/sccopy.p 318
		SORT/shell.p 621
		SORT/shell0.p 572
		SORT/sort.p 1284
		SORT/sortproc.p 304
		SORT/sortquick.p 690
		SORT/sorttest.p 424
		SORT/unique.p 380
		SORT/unrotate.p 783
		EDIT/altpatsize.p 472
		EDIT/amatch.p 1265
		EDIT/amatch0.p 367
		EDIT/amatch1.p 392
		EDIT/append.p 599
		EDIT/blkmove.p 366
		EDIT/catsub.p 510
		EDIT/change.p 630
		EDIT/chngcons.p 194
		EDIT/chngproc.p 190
		EDIT/ckglob.p 827
		EDIT/ckp.p 411
		EDIT/clrbuf1.p 170
		EDIT/clrbuf2.p 203
		EDIT/default.p 363
		EDIT/docmd.p 2981
		EDIT/dodash.p 891
		EDIT/doglob.p 664
		EDIT/doprint.p 369
		EDIT/doread.p 645
		EDIT/dowrite.p 473
		EDIT/edit.p 994
		EDIT/editcons.p 695
		EDIT/editproc.p 676
		EDIT/edittype.p 93
		EDIT/editvar.p 92
		EDIT/edprim.p 93
		EDIT/edprim1.p 240
		EDIT/edprim2.p 258
		EDIT/edtype1.p 307
		EDIT/edtype2.p 260
		EDIT/edvar1.p 485
		EDIT/edvar2.p 722
		EDIT/find.p 454
		EDIT/findcons.p 378
		EDIT/getccl.p 636
		EDIT/getfn.p 668
		EDIT/getlist.p 793
		EDIT/getmark.p 187
		EDIT/getnum.p 755
		EDIT/getone.p 891
		EDIT/getpat.p 245
		EDIT/getrhs.p 544
		EDIT/getsub.p 248
		EDIT/gettxt1.p 213
		EDIT/gettxt2.p 345
		EDIT/getword.p 478
		EDIT/lndelete.p 371
		EDIT/locate.p 502
		EDIT/makepat.p 1385
		EDIT/makesub.p 657
		EDIT/match.p 358
		EDIT/move.p 401
		EDIT/nextln.p 217
		EDIT/omatch.p 977
		EDIT/optpat.p 579
		EDIT/patscan.p 487
		EDIT/patsize.p 483
		EDIT/prevln.p 217
		EDIT/putmark.p 184
		EDIT/putsub.p 393
		EDIT/puttxt1.p 398
		EDIT/puttxt2.p 440
		EDIT/reverse.p 305
		EDIT/seek.p 520
		EDIT/setbuf1.p 272
		EDIT/setbuf2.p 521
		EDIT/skipbl.p 236
		EDIT/stclose.p 427
		EDIT/subline.p 622
		EDIT/subst.p 1358
		FORMAT/break.p 275
		FORMAT/center.p 214
		FORMAT/command.p 1173
		FORMAT/fmtcons.p 196
		FORMAT/fmtproc.p 571
		FORMAT/format.p 1820
		FORMAT/format0.p 1820
		FORMAT/getcmd.p 889
		FORMAT/gettl.p 423
		FORMAT/getval.p 462
		FORMAT/getword.p 478
		FORMAT/initfmt.p 574
		FORMAT/leadbl.p 402
		FORMAT/page.p 247
		FORMAT/put.p 447
		FORMAT/putfoot.p 225
		FORMAT/puthead.p 301
		FORMAT/puttl.p 317
		FORMAT/putword.p 809
		FORMAT/putword0.p 633
		FORMAT/setparam.p 518
		FORMAT/skip.p 202
		FORMAT/skipbl.p 236
		FORMAT/space.p 343
		FORMAT/spread.p 816
		FORMAT/text.p 762
		FORMAT/text0.p 183
		FORMAT/text1.p 567
		FORMAT/underln.p 553
		FORMAT/width.p 377
		MACRO/cscopy.p 318
		MACRO/defcons.p 339
		MACRO/define.p 836
		MACRO/defproc.p 379
		MACRO/deftype.p 417
		MACRO/defvar.p 346
		MACRO/dochq.p 473
		MACRO/dodef.p 350
		MACRO/doexpr.p 296
		MACRO/doif.p 507
		MACRO/dolen.p 305
		MACRO/dosub.p 734
		MACRO/eval.p 1083
		MACRO/expr.p 462
		MACRO/factor.p 413
		MACRO/getdef.p 1122
		MACRO/getpbc.p 323
		MACRO/gettok.p 591
		MACRO/gnbchar.p 266
		MACRO/hash.p 287
		MACRO/hashfind.p 447
		MACRO/initdef.p 412
		MACRO/inithash.p 261
		MACRO/initmacro.p 1446
		MACRO/install.p 727
		MACRO/lookup.p 369
		MACRO/maccons.p 494
		MACRO/macproc.p 581
		MACRO/macro.p 2396
		MACRO/mactype.p 468
		MACRO/macvar.p 1107
		MACRO/pbnum.p 249
		MACRO/pbstr.p 224
		MACRO/push.p 319
		MACRO/putback.p 263
		MACRO/putchr.p 332
		MACRO/puttok.p 266
		MACRO/sccopy.p 318
		MACRO/term.p 514
		MAN/archive.m 1987
		MAN/change.m 840
		MAN/charcount.m 471
		MAN/close.m 339
		MAN/compare.m 568
		MAN/compress.m 839
		MAN/concat.m 436
		MAN/copy.m 565
		MAN/create.m 650
		MAN/define.m 879
		MAN/detab.m 638
		MAN/echo.m 385
		MAN/edit.m 4040
		MAN/entab.m 802
		MAN/error.m 362
		MAN/expand.m 737
		MAN/find.m 1802
		MAN/format.m 2268
		MAN/getarg.m 572
		MAN/getc.m 618
		MAN/getline.m 704
		MAN/include.m 587
		MAN/kwic.m 704
		MAN/linecount.m 290
		MAN/macro.m 2869
		MAN/makecopy.m 515
		MAN/open.m 484
		MAN/overstrike.m 897
		MAN/print.m 729
		MAN/putc.m 601
		MAN/putstr.m 470
		MAN/remove.m 298
		MAN/seek.m 450
		MAN/sort.m 683
		MAN/translit.m 1669
		MAN/unique.m 484
		MAN/unrotate.m 1035
		MAN/wordcount.m 423
		PMAN/close.m 280
		PMAN/create.m 943
		PMAN/error.m 364
		PMAN/getarg.m 557
		PMAN/getc.m 722
		PMAN/getcf.m 776
		PMAN/getline.m 701
		PMAN/message.m 311
		PMAN/nargs.m 411
		PMAN/open.m 972
		PMAN/putc.m 350
		PMAN/putcf.m 360
		PMAN/putstr.m 431
		PMAN/remove.m 493
		PMAN/seek.m 651

