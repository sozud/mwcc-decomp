// Source-only b52: MSL's bsearch (stdlib.h). b52's 3.0 optimizer looks up
// known C library calls in IRO_StdFuncs with it (IroEval.c); the tree's MSL
// has no bsearch.c, and b69 never calls it. Native builds take the host's.
