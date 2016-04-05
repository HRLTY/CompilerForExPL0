#pragma once
#include "tables.h"
extern tabEntry tab[TMAX]; //symbol table
extern int t; //table index
extern btabEntry btab[BMAX];
extern int b;//procedure table index
extern atabEntry atab[AMAX];
extern int a;//atab index
extern float st[SMAX];
extern int dtab[DMAX];
extern order code[CMAX]; //P-code base
extern int lc; //current code location
extern int display[DMAX]; //for stack
extern std::string trans[];
extern std::string stab[STMAX]; //string table
extern int s;//stab index