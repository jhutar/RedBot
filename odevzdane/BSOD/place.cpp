////////////////////////////////////////////////////////////////////////////////
// place.cpp
// Red bot spring 2014 - BSOD Elite Strategy
// placing executable file
// (c) Jan Horacek, team BSOD
// jan.horacek@seznam.cz
// 27.04.2014
// v1.0
// this file is open source and can be modified freely with attribution
////////////////////////////////////////////////////////////////////////////////

// This file cares about placing new shops
// Main idea = heuristic function
//  We assign a number to every position in map, find maximmum number and place a shop there

// This theory has a few problems:
//  - it is difficult to calculate heuristic function for placing more shops than one ->
//      -> we can place more shops by executing algorithm more times, but the positions are not ideal

// From the task:
//  strenght_of_shop_in_city = 1 / ( (1+distance_to_shop) * (1+(prize_in_shop^3)) )

#include <iostream>

#include "place.h"
#include "main.h"

#ifdef DEBUG
    #include <iomanip>
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// private variables:

float **heuristic;          // 2D array for heuristic, we save there number of people coming to "virtual" shop at coordinates [Y][X]
float* place_cities;        // here we precalculate strenght of current cities, we do it just one time and then execute whole heuristic N times
                            // warning:  it is necessary to make new precalculation after placing a shop

///////////////////////////////////////////////////////////////////////////////
// private function headers:

void find_max_heuristic(unsigned int &maxy, unsigned int &maxx);        // find field for shop from results of the heuristics
void place_shop(unsigned int y, unsigned int x);                        // calculate heuristics for one point
bool is_free(int y, int x);                                             // chacks for exesting markets at the coordinates

///////////////////////////////////////////////////////////////////////////////

void place_init()
{
    unsigned int i;

    // create a heuristic sarray:
    heuristic = new float*[map_size];
    for(i = 0; i < map_size; i++)
        heuristic[i] = new float[map_size];

    // create a place_cities array
    place_cities = new float[cities.cnt];
}

void place_reset()
{
    // reset heuristics array:
    unsigned int y, x;
    for (y = 0; y < map_size; y++)
        for (x = 0; x < map_size; x++)
            heuristic[y][x] = 0;
}

void place_finish()
{
    // clean-up arrays:
    for(unsigned int i = 0; i < map_size; i++)
        delete[] heuristic[i];
    delete[] heuristic;

    delete[] place_cities;
}

///////////////////////////////////////////////////////////////////////////////

// calculate temp strenght for all cities
void precalculate()
{
    unsigned int i, j;
    for (i = 0; i < cities.cnt; i++)
    {
        place_cities[i] = 0;

        for (j = 0; j < players[0].shops.cnt; j++)
            place_cities[i] += 1.0 / (1 + Mannhattan(cities.data[i].pos, players[0].shops.data[j].pos));

        for (j = 0; j < players[1].shops.cnt; j++)
            place_cities[i] += 1.0 / (1 + Mannhattan(cities.data[i].pos, players[1].shops.data[j].pos));
    }
}

///////////////////////////////////////////////////////////////////////////////

// place shop
void place()
{
    unsigned int y,x;

    // calculate heuristics for every pos on the map
    for (y = 0; y < map_size; y++)
        for (x = 0; x < map_size; x++)
            place_shop(y, x);

    // find maximum
    find_max_heuristic(y, x);

    // and put the shop there
    while (!is_free(y, x))
    {
        heuristic[y][x] = 0;
        find_max_heuristic(y, x);
    }

    cout << "n " << x << " " << y << ";";

    players[0].shops.cnt++;
    players[0].shops.data[players[0].shops.cnt-1].pos    = Point(y, x);
    players[0].shops.data[players[0].shops.cnt-1].prize  = 0;
    players[0].shops.data[players[0].shops.cnt-1].profit = 0;
    players[0].money -= 250;
}

void place_shop(unsigned int y, unsigned int x)
{
    unsigned int i;
    float my_strenght;

    for (i = 0; i < cities.cnt; i++)
    {
        my_strenght = 1.0 / (1+Mannhattan(cities.data[i].pos, Point(y, x)));
        heuristic[y][x] += (cities.data[i].people_cnt * (my_strenght / (my_strenght+place_cities[i])));
    }
}

void find_max_heuristic(unsigned int &maxy, unsigned int &maxx)
{
    unsigned int y, x;
    float amax = 0;

    for (y = 0; y < map_size; y++)
    {
        for (x = 0; x < map_size; x++)
        {
            if (heuristic[y][x] > amax)
            {
                amax = heuristic[y][x];
                maxy = y;
                maxx = x;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void print_heuristic()
{
    unsigned int y, x;

    for (y = 0; y < map_size; y++)
    {
        for (x = 0; x < map_size; x++)
            cout << setfill(' ') << setw(5) << setprecision(3) << heuristic[y][x];
        cout << endl;
    }
}


void print_place_cities()
{
    unsigned int x, y;
    float data[map_size][map_size];

    for (y = 0; y < map_size; y++)
        for (x = 0; x < map_size; x++)
            data[y][x] = 0;

    for (x = 0; x < cities.cnt; x++)
        data[cities.data[x].pos.y][cities.data[x].pos.x] = place_cities[x];

    for (y = 0; y < map_size; y++)
    {
        for (x = 0; x < map_size; x++)
            cout << setfill(' ') << setw(5) << (int)(data[y][x]*1000);
        cout << endl;
    }
}

#endif

///////////////////////////////////////////////////////////////////////////////

// return true if free, false if not free
bool is_free(int y, int x)
{
    unsigned int i, j;
    for (i = 0; i < 2; i++)
        for (j = 0; j < players[i].shops.cnt; j++)
            if ((x == players[i].shops.data[j].pos.x) && (y == players[i].shops.data[j].pos.y))
                return false;
    return true;
}
