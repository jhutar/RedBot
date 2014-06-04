////////////////////////////////////////////////////////////////////////////////
// main.j
// Red bot spring 2014 - BSOD Elite Strategy
// main header file
// (c) Jan Horacek, team BSOD
// jan.horacek@seznam.cz
// 27.04.2014
// v1.0
// this file is open source and can be modified freely with attribution
////////////////////////////////////////////////////////////////////////////////

// Main ideas of "BSOD Elite Strategy":
//  1) build some shops in the first rounds
//  2) dynamically set prizes in existing shops in every next round

// This strategy for example:
//  - does not remove any shop anytime
//  - do some things randomly
//  - tries to have lower prizes than its enemy (hope that will bring larger earnings)
//  - was tester against "BSOD Light Strategy", "BSOD Down Strategy", "BSOD Action Strategy",
//      "BSOD First-biggest Strategy", "BSOD Second-biggest Strategy"

// warning: software is designed for building maximally 32 shops per round, be careful!

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#undef DEBUG

///////////////////////////////////////////////////////////////////////////////
// Basic structures definitions

#define _MAX_BUILD_SHOPS    32

///////////////////////////////////////////////////////////////////////////////
// CITIES:

typedef struct Pnt {
    int x;
    int y;

    Pnt(int ay, int ax) { y = ay; x = ax; }
    Pnt() { y = 0; x = 0; };
} Point;

typedef struct {
    Point pos;
    unsigned int people_cnt;
} city;

typedef struct {
    city* data;
    unsigned int cnt;
} city_db;

extern city_db cities;

///////////////////////////////////////////////////////////////////////////////
// SHOPS:

typedef struct {
    Point pos;
    float prize;
    float profit;
} shop;

typedef struct {
    shop* data;
    unsigned int cnt;
} shops_db;

typedef struct {
    float money;
    shops_db shops;
    //string response;      not necessary
} player;

extern player players[2];   // me = 0, enemy = 1

///////////////////////////////////////////////////////////////////////////////

typedef struct {
    int playeri;
    int shopi;
} shop_ref;

///////////////////////////////////////////////////////////////////////////////

extern unsigned int map_size;
extern unsigned int game_round;
extern unsigned int my_player_index;

unsigned int Mannhattan(Point first, Point second);

#endif // MAIN_H_INCLUDED
