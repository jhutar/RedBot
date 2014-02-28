#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <iostream>
#include <list>

#define NUM_PLAYERS			2
#define ONE_ROUND_DELAY			2000		/* in milliseconds */
#define CHECK_BOTS_INTERVAL		10		/* in milliseconds */
#define RESPONSE_LENGTH			2048

#define MAX_ROUNDS                      1000
#define SHOP_BUILD_PRICE		250
#define SHOP_MAINTENANCE		30
#define GOODS_COST			1
#define MAX_PRICE                       10   /* has to be smaller than INVALID_PRICE */
#define INVALID_PRICE			99.9 /* has to be bigger than MAX_PRICE */

using namespace std;

const char NEW_SHOP = 'n';
const char DEL_SHOP = 'z';
const char CMD_SEPARATOR = ';';
const string NEW_SHOP_CMD_FORMAT = (string)""+NEW_SHOP+" %d %d";
const string DEL_SHOP_CMD_FORMAT = (string)""+DEL_SHOP+" %d %d";

const string DEFAULT_FIELD_FILENAME = "playfield.txt";

const string run_bot_under_user[2] = { "botuser0", "botuser1" };

struct town_t {
  unsigned x;
  unsigned y;
  unsigned residents;
};

struct shop_t {
  unsigned x;
  unsigned y;
  float price;
  float profit;
};

struct player_t {
  string last_response;
  float money;
  list<shop_t> shops;
};

struct shop_weight_ptr_t {
  float weight;
  list<shop_t>::iterator shop_iter;
};

struct new_shop_t {
  unsigned x;
  unsigned y;
  unsigned player;
};

typedef char response_t[RESPONSE_LENGTH];


class PlayField
{

public:
PlayField(string _fields_dir, string _filename);
~PlayField();
bool game_finished() { return _game_finished; };
void play_one_round(response_t *moves);
unsigned get_current_round() { return current_round; };
bool is_player_alive(unsigned player) { return (players[player].money>=0); };
void set_debug(bool _debug) { debug = _debug; };
void get_points(int points[NUM_PLAYERS]);
string get_current_filename() { return (fields_dir + "/" + filename); };

private:
string fields_dir;
string filename;
player_t players[NUM_PLAYERS];
list<town_t> towns;
unsigned field_size;
bool _game_finished;
bool debug;
unsigned current_round;
void write_playfield_to_disk(const char *mode);
void print_field();
void increase_current_round();
void add_shop(list<new_shop_t>::iterator new_shop);
};

#endif  /* !PLAYFIELD_H */
