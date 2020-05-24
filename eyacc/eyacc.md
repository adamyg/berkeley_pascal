EYACC(1)                    General Commands Manual                   EYACC(1)

# NAME
       eyacc - modified yacc allowing much improved error recovery

## SYNOPSIS
       eyacc [ -v ] [ grammar ]

## DESCRIPTION

       Eyacc is an old version of yacc(1), which produces tables used by the
       Pascal system and its error recovery routines.  Eyacc fully enumerates
       test actions in its parser when an error token is in the look-ahead
       set.  This prevents the parser from making undesirable reductions when
       an error occurs before the error is detected.  The table format is
       different in eyacc than it was in the old yacc, as minor changes had
       been made for efficiency reasons.

## SEE ALSO

       yacc(1)
       ``Practical LR Error Recovery'' by Susan L. Graham, Charles B. Haley
       and W. N. Joy; SIGPLAN Conference on Compiler Construction, August
       1979.

## AUTHOR
       S. C. Johnson

       Eyacc modifications by Charles Haley and William Joy.

## BUGS
       Pc and its error recovery routines should be made into a library of
       routines for the new yacc.


4th Berkeley Distribution           4/29/85                           EYACC(1)
