#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <iostream>

#define NUM_PLAYERS			2
#define ONE_ROUND_DELAY			2000		/* in milliseconds */
#define CHECK_BOTS_INTERVAL		10		/* in milliseconds */

#define BATTLEFIELD_SIZE		14
#define MAX_CHARGES			10
#define MAX_ROUNDS			150
#define FIREWORKS_HITS                   8

using namespace std;

const char FIELD_EMPTY     = ' ';
const char FIELD_SHIP1     = '1';
const char FIELD_SHIP2     = '2';
const char FIELD_SHIP1_HIT = '*';
const char FIELD_SHIP2_HIT = '+';
const char FIELD_EMPTY_HIT = '.';

const char FIELD_MY_SHIP = '1';
const char FIELD_MY_SHIP_HIT = '*';
const char FIELD_HIS_SHIP_HIT = '+';

const char FIRE_MISSILE   = 'm';
const char FIRE_BOMB      = 'b';
const char FIRE_TORPEDO   = 't';
const char FIRE_FIREWORKS = 'f';

const char DIRECTION_UP = 'u';
const char DIRECTION_RIGHT = 'r';
const char DIRECTION_DOWN = 'd';
const char DIRECTION_LEFT = 'l';

const string BATTLEFIELD_FILENAME = "battlefield.txt";

const string run_bot_under_user[2] = { "botuser0", "botuser1" };
const unsigned SHIPS = 7;
const unsigned ships_x_size[SHIPS] = { 1, 1, 1, 1, 1, 1, 2 };
const unsigned ships_y_size[SHIPS] = { 5, 4, 3, 3, 2, 2, 3 };

/* field mapping one-dimensional ID of fireworks fields to real fields; we
 * generate FIREWORKS_HITS randim numbers from 0..15 and then bomb fields
 * [fireworks_pos_x[i],fireworks_pos_y[i] for every rnd. "i" */
const int fireworks_pos_x[16] =
  { -2, -1,  0,  1,  2,  2,  2,  2,  2,  1,  0, -1, -2, -2, -2, -2 };
const int fireworks_pos_y[16] =
  { -2, -2, -2, -2, -2, -1,  0,  1,  2,  2,  2,  2,  2,  1,  0,  1 };

struct fleet_info_t {
unsigned charges;
unsigned ship_fields_alive;
unsigned points;
string bot_dir;
string last_round;
};

typedef char response_t[128];

/* auxiliary function to convert unsigned to string */
string unsigned_to_string(unsigned u);

class PlayField
{

public:
PlayField(string *bots_dir, string _battlefields_dir);
~PlayField();
bool game_finished() { return _game_finished; };
void play_one_round(response_t *moves);
unsigned get_current_round() { return current_round; };
bool is_fleet_alive(unsigned player);
void set_debug(bool _debug) { debug = _debug; };
bool is_playfield_generated() { return playfield_generated; };
void print_battlefield_for_player(unsigned player);
void get_points(unsigned points[NUM_PLAYERS]);

private:
fleet_info_t fleets[NUM_PLAYERS];
char **battlefield;
string battlefields_dir;
bool _game_finished;
bool playfield_generated;
bool debug;
unsigned current_round;
void write_playfield_to_disk(const char *mode);
void increase_current_round();
void generate_battlefield();
void print_battlefield(ostream & out, bool *move_ok);
bool try_to_place_one_boat(unsigned boat);
char missile_to_field(unsigned player, int x, int y);
void fire_bomb(unsigned player, int x, int y);
void fire_torpedo(unsigned player, int x, int y, int x_diff, int y_diff);
void fire_fireworks(unsigned player, int x, int y);
};

#endif  /* !PLAYFIELD_H */
