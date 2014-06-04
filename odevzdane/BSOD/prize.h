////////////////////////////////////////////////////////////////////////////////
// prize.h
// Red bot spring 2014 - BSOD Elite Strategy
// prize calculating header file
// (c) Jan Horacek, team BSOD
// jan.horacek@seznam.cz
// 27.04.2014
// v1.0
// this file is open source and can be modified freely with attribution
////////////////////////////////////////////////////////////////////////////////

#ifndef PRIZE_H_INCLUDED
#define PRIZE_H_INCLUDED

#include "main.h"

// public functions headers:

void init_shop_prizes();            // must be called before calculating
void finish_shop_prizes();          // must be called before exeting a program
void set_shop_prizes();             // set prizes of all shops

#endif // PRIZE_H_INCLUDED
