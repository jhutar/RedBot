///////////////////////////////////////////////////////////////////////////////
// prize.cpp
// Red bot spring 2014 - BSOD Elite Strategy
// prize calculating executable file
// (c) Jan Horacek, team BSOD
// jan.horacek@seznam.cz
// 27.04.2014
// v1.0
// this file is open source and can be modified freely with attribution
///////////////////////////////////////////////////////////////////////////////

// This file cares about setting prizes in shops

// Main idea: look around for some shops and their prizes and assing similar prize for our shop

#include <climits>
#include <cstdlib>

#include "prize.h"
#include "main.h"

#ifdef DEBUG
    #include <iostream>
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// local variables and definitions:

#define _NEAREST_COUNT   3  // we calculate prize according to 3 nearest cities
bool** excluded;            // indexed according to cities

///////////////////////////////////////////////////////////////////////////////
// function headers:

void find_nearest_shop(Point pos, bool** excluded, int &playeri, int &shopi);
float get_pos_prize(Point pos);     // get prize at the pos

///////////////////////////////////////////////////////////////////////////////

void init_shop_prizes()
{
    // create excluded 2D array:
    excluded = new bool*[map_size];
    for(unsigned int i = 0; i < map_size; i++)
        excluded[i] = new bool[map_size];
}

void finish_shop_prizes()
{
    // free excluded 2D array:
    for(unsigned int i = 0; i < map_size; i++)
        delete[] excluded[i];
    delete[] excluded;
}

///////////////////////////////////////////////////////////////////////////////

// set prizes in all shops
void set_shop_prizes()
{
    unsigned int i;
    float prize;

    for (i = 0; i < players[0].shops.cnt; i++)
    {
        #ifdef DEBUG
        cout << "shop: " << players[0].shops.data[i].pos.x << ":" << players[0].shops.data[i].pos.y << endl;
        #endif

        // this is a little magic: random
        //  we kave 70% probability of decrease the prize and 30% of increas the prize
        //  maximum random manipulation of the prize =
        if (players[1].shops.cnt > 0)
            prize = get_pos_prize(players[0].shops.data[i].pos) + ((rand()%10 - 7)/100.0);
        else
            prize = get_pos_prize(players[0].shops.data[i].pos);

        // what to do in difficult times:
        if (prize > 5) prize = 5;
        if (prize < 1) prize = 1.5;

        players[0].shops.data[i].prize = prize;
    }
}

///////////////////////////////////////////////////////////////////////////////

// this function returns ideal prize for virtual shop at pos position
float get_pos_prize(Point pos)
{
    shop_ref nearest[_NEAREST_COUNT];
    unsigned int i, j;
    int playeri, shopi;

    #ifdef DEBUG
    cout << "Calculating prizes for: " << pos.x << ":" << pos.y << endl;
    #endif

    // just in case
    if (cities.cnt == 0)
        return 5.0;

    // reset excluded
    for (i = 0; i < map_size; i++)
        for (j = 0; j < map_size; j++)
            excluded[i][j] = false;

    // find some nearest shops and calculate prize at pos according to it:
    for (i = 0; i < _NEAREST_COUNT; i++)
    {
        find_nearest_shop(pos, excluded, playeri, shopi);

        nearest[i].playeri = playeri;
        nearest[i].shopi   = shopi;

        // when amount of shops is small...
        if (shopi < 0) continue;

        #ifdef DEBUG
        cout << "Nearest: " << players[playeri].shops.data[shopi].pos.x << ":" << players[playeri].shops.data[shopi].pos.y << endl;
        #endif

        excluded[players[playeri].shops.data[shopi].pos.y][players[playeri].shops.data[shopi].pos.x] = true;
    }

    // now, we have nearest shops -> lets calculate max distance:
    unsigned int max_dist = 0, tmp_dist;

    for (i = 0; i < _NEAREST_COUNT; i++)
    {
        if (nearest[i].shopi < 0) continue;
        if ((tmp_dist = Mannhattan(pos, players[nearest[i].playeri].shops.data[nearest[i].shopi].pos)) > max_dist)
            max_dist = tmp_dist;
    }
    max_dist++;         // this makes everything more relevant

    #ifdef DEBUG
    cout << "max_dist + 1: " << max_dist << endl;
    #endif

    //lets calculate sums:
    float prize_sum = 0;
    float dist_sum = 0;
    for (i = 0; i < _NEAREST_COUNT; i++)
    {
        if (nearest[i].shopi < 0) continue;
        prize_sum += (max_dist - Mannhattan(pos, players[nearest[i].playeri].shops.data[nearest[i].shopi].pos)) * players[nearest[i].playeri].shops.data[nearest[i].shopi].prize;
        dist_sum  += (max_dist - Mannhattan(pos, players[nearest[i].playeri].shops.data[nearest[i].shopi].pos));
    }

    #ifdef DEBUG
    cout << "prize: " << (prize_sum / dist_sum) << endl;
    #endif

    return (prize_sum / dist_sum);
}

///////////////////////////////////////////////////////////////////////////////

// returns index
// shop is defined by indexes "playeri" and "shopi" : people[playeri].shops.data[shopi]
void find_nearest_shop(Point pos, bool** excluded, int &playeri, int &shopi)
{
    unsigned int minDist = INT_MAX, tmpDist;
    unsigned int i;

    shopi = -1;

    for (int j = 0; j < 2; j++)
    {
        for (i = 0; i < players[j].shops.cnt; i++)
        {
            if (excluded[players[j].shops.data[i].pos.y][players[j].shops.data[i].pos.x]) continue;
            if ((players[j].shops.data[i].pos.y == pos.y) && (players[j].shops.data[i].pos.x == pos.x)) continue;
            if (minDist > (tmpDist = Mannhattan(pos, players[j].shops.data[i].pos)))
            {
                minDist = tmpDist;
                playeri = j;
                shopi   = i;
            }
        }
    }
}
