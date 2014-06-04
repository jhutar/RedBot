////////////////////////////////////////////////////////////////////////////////
// place.h
// Red bot spring 2014 - BSOD Elite Strategy
// placing header file
// (c) Jan Horacek, team BSOD
// jan.horacek@seznam.cz
// 27.04.2014
// v1.0
// this file is open source and can be modified freely with attribution
////////////////////////////////////////////////////////////////////////////////

#ifndef PLACE_H_INCLUDED
#define PLACE_H_INCLUDED

// public functions headers:

void place_init();          // must be called once at the start of a program
void place_reset();         // ! must be executed before every placing
void place_finish();        // must be called once, when ending the program

void precalculate();        // must be executed before placing, when shops were mofified after last precalculate() calling
void place();               // place shop

// debug "printing functions":
#ifdef DEBUG
    void print_heuristic();     // print heuristics function results for whole map
    void print_place_cities();  // print result of precalculation
#endif // DEBUG

#endif // PLACE_H_INCLUDED
