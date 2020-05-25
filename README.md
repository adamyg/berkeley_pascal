# Berkeley Pascal

Berkeley Pascal was developed within the Computer Science Division; Department of Electrical Engineering and Computer Science;
University of California, Berkeley by William N. Joy, Susan L. Graham and Charles B. Haley. It was originally designed for interactive instructional use on the PDP/11 and VAX/11 computers. Interpretive P-code is produced, providing fast translation at the expense of slower execution speed. 

This project is an investigation into retargeting to modern hosts, with the intention of exploring use within embedded systems/applications, plus for general nostalgia.  

The original source is from the 4th series of Berkeley Distributions, namely BSD4.3 and BSD4.4-Lite.

## Source Roadmap:

    pi -          Pascal interpreter.

    px -          Pascal byte-code compiler.
 
    pdx -         Pascal debugger.

    doc -         Reference and design documents.
 
    src -         Command source for px, pi and pdx.

    libpc -       Pascal run-time library.

    libcompat -   Compatibility library, implements omitted functionality assumed to be available.

    tstpx -       Test cases and associated expected results.

    byacc -       Berkeley yacc, is a LALR(1) parser generator.

    eyacc -       Modified yacc allowing much improved error recovery; needed by the Pascal parser.
    
    ex -          Stream editor.   

## Source Reference:

The following source modules are included for reference: 
   
    pxref -       Cross-reference generator.

    pix -         Pascal interpreter and executor.

    pc -          Pascal compiler; requires the Berkeley Portable C Compiler (pcc) infrastructure.

    pcextern -    External symbol stabs generator.

    pxheader -    libexec loader. 
 
 
## Install
 
Initial port has been retargeted to Windows (32-bit), using either Open Watcom or Microsoft Visual C/C++.
Additional targets, includes Linux are roadmapped, yet should be cleaner objective.
  
* Install the gnuwin32 tool set or similar; for example Source, http://unxutils.sourceforge.net/.

* If you dont have perl available, download and install; ActivePerl is one of the better options. 

* Run the <makelib.pl> perl script by typing:
 
       o MSVC 2008 or greater

```
          $ perl makelib.pl [--gnuwin32=<path-gnuwin32-bin>] vc[2015|2017|2019]
```

       o Open Watcom

```
          $ perl makelib.pl owc[19|20]
```

* Build the entire tree with:

```
          $ gmake
```
