
-= libchessinterface =-

The chess engine C/C++ library for Windows and Linux!

WHAT IS IT:

This library takes the whole work of launching chess engines,
establishing input/output to them and implementing the protocols
UCI (Universal Chess Interface) and CECP (Chess Engine Communication
Protocol - also called Winboard/Xboard protocol) for you.

It does NOT implement chess knowledge however. This means:
 * you need your own move validity checks. you still need to keep
   track of the game state on your own.
 * you need to support coordinate algebraic notation for moves
   (e.g. a2a4, d7d5 etc), and you need to support FEN (Forsyth–Edwards
   Notation) for positions, see
   http://en.wikipedia/wiki/Forsyth–Edwards_Notation
 * you SHOULD support PGN standard algebraic notation if you want
   all CECP engines to work, see
   http://en.wikipedia.org/wiki/Algebraic_notation_(chess)

HOW TO USE IT:

For installation, see INSTALL.txt
Check out example.c!

LICENSE:

The library is licensed under LGPL v3. This means you can use
it in commercial or closed-source (=freeware without source code
available) programs, but:
 * you must not link the library statically, but instead dynamically
   as .dll/.so IF your main program where you link it from doesn't
   have its source code freely available, and
 * you need to put a notice that you use libchessinterface into
   your program (e.g. in an "about" dialog), or into the documentation
   of your program.
 * also, you must publish the sources if you ship a modified .dll/.so
   so everyone has access to the modifications you applied to the
   library.
(This is just a listing to simplify things for you WITHOUT WARRANTY
OF CORRECTNESS! Read COPYING.txt for the legalese terms)


