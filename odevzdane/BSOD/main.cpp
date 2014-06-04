////////////////////////////////////////////////////////////////////////////////
// main.cpp
// Red bot spring 2014 - BSOD Elite Strategy
// main executable file
// (c) Jan Horacek, team BSOD
// jan.horacek@seznam.cz
// 27.04.2014
// v1.0
// this file is open source and can be modified freely with attribution
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "main.h"
#include "place.h"
#include "prize.h"

#ifdef DEBUG
    #include <iomanip>
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////

// real definitions of extern variables from "main.h"
city_db cities;
player players[2];
unsigned int map_size;
unsigned int game_round;
unsigned int my_player_index;

///////////////////////////////////////////////////////////////////////////////
// private function headers:

int main(int argc, char **argv);
void load_file();
void clean_up();
void maximum_city_place();
void print_prizes();

#ifdef DEBUG
    void print_cities();
#endif // DEBUG

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Enter player index as a parameter!" << endl;
        return 1;
    }

    // taken from RedBot game interpreter source file
    // generate some really random seed for srand
    unsigned random_seed;
    std::ifstream file ("/dev/urandom", std::ios::binary);
    if (file.is_open())
    {
        char * memblock;
        int size = sizeof(int);
        memblock = new char [size];
        file.read (memblock, size);
        file.close();
        random_seed = *reinterpret_cast<int*>(memblock);
        delete[] memblock;
    }
    else
    {
        cerr << "Cannot read /dev/urandom !" << endl;
        return EXIT_FAILURE;
    }
    srand (random_seed);

    ////////////////////////////////////////

    // parse player index (from argument):
    string hello(argv[1]);
    stringstream str(hello);
    str >> my_player_index;

    ////////////////////////////////////////

    // load "playfield.txt":
    load_file();

    ////////////////////////////////////////

    #ifdef DEBUG
    cout << "I have " << players[0].shops.cnt << " shops" << endl;
    cout << "Enemy has " << players[1].shops.cnt << " shops" << endl;
    #endif

    // set prizes in current shops:
    if (players[0].shops.cnt > 0)
    {
        #ifdef DEBUG
        cout << "Counting prize" << endl;
        #endif

        init_shop_prizes();
        set_shop_prizes();
        finish_shop_prizes();
        print_prizes();
    }

    // lets have fun
    // create new shop:
    if ((rand()%200 == 0) && (players[0].money > 250))
    {
        #ifdef DEBUG
        cout << "Placing shop" << endl;
        #endif

        // place one shop:
        place_init();
        place_reset();
        precalculate();
        place();
        place_finish();
    }

    // create shop in the biggest city:
    if (players[0].money >= 250)
        maximum_city_place();

    // place shops at he start of strategy
    if (game_round < 2)
    {
        // first round: place shops
        #ifdef DEBUG
        cout << "Placing shops" << endl;
        #endif

        place_init();
        // we place all possible markets:
        while (players[0].money >= 500)
        {
            place_reset();
            precalculate();
            place();
        }
        place_finish();
    }

    cout << endl;

    // clean-up
    clean_up();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

// calculate Mannhattan distancc between two points
unsigned int Mannhattan(Point first, Point second)
{
    return abs(first.x - second.x) + abs(first.y - second.y);
}

///////////////////////////////////////////////////////////////////////////////

// load "playfield.txt"
void load_file()
{
    // parse playfield.txt
    string sread;
    unsigned int i, j;

    ifstream ifile("playfield.txt");
    if (ifile.fail())
    {
        cerr << "Cannot load playfield.txt" << endl;
        return;
    }

    ifile >> sread;
    ifile >> game_round;

    ifile >> sread;
    ifile >> map_size;

    ifile >> sread;
    ifile >> cities.cnt;

    // create array
    cities.data = new city[cities.cnt];
    for (i = 0; i < cities.cnt; i++)
    {
        ifile >> cities.data[i].pos.x;
        ifile >> cities.data[i].pos.y;
        ifile >> cities.data[i].people_cnt;
    }

    int player;
    for (i = 0; i < 2; i++)
    {
        player = ((my_player_index-1) xor i);

        ifile >> sread;

        ifile >> players[player].money;
        ifile >> sread;
        ifile >> players[player].shops.cnt;
        ifile >> sread;

        players[player].shops.data = new shop[players[player].shops.cnt+_MAX_BUILD_SHOPS];

        for (j = 0; j < players[player].shops.cnt; j++)
        {
            ifile >> players[player].shops.data[j].pos.x;
            ifile >> players[player].shops.data[j].pos.y;

            #ifdef DEBUG
            cout << "Loaded shop: " << players[player].shops.data[j].pos.x << ":" << players[player].shops.data[j].pos.y << endl;
            #endif // DEBUG

            ifile >> players[player].shops.data[j].prize;
            ifile >> players[player].shops.data[j].profit;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void clean_up()
{
    delete[] cities.data;
    delete[] players[0].shops.data;
    delete[] players[1].shops.data;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void print_cities()
{
    unsigned int x, y;
    bool data[map_size][map_size];

    for (y = 0; y < map_size; y++)
        for (x = 0; x < map_size; x++)
            data[y][x] = false;

    for (x = 0; x < cities.cnt; x++)
        data[cities.data[x].pos.y][cities.data[x].pos.x] = true;

    for (y = 0; y < map_size; y++)
    {
        for (x = 0; x < map_size; x++)
            cout << setfill(' ') << setw(5) << data[y][x];
        cout << endl;
    }

}
#endif

///////////////////////////////////////////////////////////////////////////////

// build a shop in the city with maximum amount of people
void maximum_city_place()
{
    unsigned int i, maxi, maxcnt = 0;

    #ifdef DEBUG
    cout << "Placing to the maximmum city" << endl;
    #endif

    // get the biggest city:
    for (i = 0; i < cities.cnt; i++)
    {
        if (cities.data[i].people_cnt > maxcnt)
        {
            maxcnt = cities.data[i].people_cnt;
            maxi = i;
        }
    }


    bool is = false;
    for (i = 0; ((i < players[0].shops.cnt) && (!is)); i++)
        if ((players[0].shops.data[i].pos.x == cities.data[maxi].pos.x) && (players[0].shops.data[i].pos.y == cities.data[maxi].pos.y))
            is = true;

    if (!is)
    {
        // if we did not build shop in the largest city -> build it
        players[0].shops.cnt++;
        players[0].shops.data[players[0].shops.cnt-1].pos    = Point(cities.data[maxi].pos.y, cities.data[maxi].pos.x);
        players[0].shops.data[players[0].shops.cnt-1].prize  = 0;
        players[0].shops.data[players[0].shops.cnt-1].profit = 0;
        players[0].money -= 250;
        cout << "n " << cities.data[maxi].pos.x << " " << cities.data[maxi].pos.y << ";";
    } else {
        #ifdef DEBUG
        cout << "ERR: Shop in the largest city present: " << cities.data[maxi].pos.x << ":" << cities.data[maxi].pos.y << "#" << i << endl;
        #endif
    }
}

///////////////////////////////////////////////////////////////////////////////

// print output for the gaming interpreter:
void print_prizes()
{
    for (unsigned int i = 0; i < players[0].shops.cnt; i++)
        cout << players[0].shops.data[i].prize << ";";
}
