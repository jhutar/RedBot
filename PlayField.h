#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <string>

#define NUM_PLAYERS 			 4
#define ONE_ROUND_DELAY			 2000		//in milliseconds
#define CHECK_BOTS_INTERVAL		 100		//in milliseconds
#define FIELD_DEFAULT_FILENAME		 "/tmp/field.txt"
#define PLAY_SNAPSHOTS_FILE_SUFFIX	 ".lastplay"
#define KINGDOM_ATTACK_FILENAME		 "utok.txt"
#define KINGDOM_DEFENCE_FILENAME	 "obrana.txt"
#define KINGDOM_INFORMATION_FILENAME     "informace.txt"
#define KINGDOM_ROBBERY_ATTACK_FILENAME  "loupez_utok.txt"
#define KINGDOM_ROBBERY_DEFENCE_FILENAME "loupez_obrana.txt"


#define ARMS_LEVEL_UP                   3.0
#define FARMING_LEVEL_UP		3.0
/* how much to multiply food earned from merchanding
(FARMING_MULTIPLIER * peasants * (1+floor(farming/FARMING_LEVEL_UP))
 */
#define FARMING_MULTIPLIER		3
#define DEFENCE_MULTIPLIER		1.5
#define INFORMATION_ACT_REPEATED	2
#define ROBBERY_ACT_REPEATED		3
#define MAX_ACT_HISTORY (((INFORMATION_ACT_REPEATED) < (ROBBERY_ACT_REPEATED)) ? (ROBBERY_ACT_REPEATED) : (INFORMATION_ACT_REPEATED))

const char ACTION_SOLDIER = 'v';	/* "voják" */
const char ACTION_PEASANT = 'r';	/* "rolník" */
const char ACTION_ARMY = 'z';		/* "zbraně" */
const char ACTION_FARMING = 'f';	/* "bankovnictví" */
const char ACTION_HARVEST = 's';        /* "sklizeň" */
const char ACTION_ATTACK = 'u';		/* "utok", format is: u kingdom_id attacking_soldiers    
					    where kingdom_id is from 1 to 3 for every kingdom */
const char ACTION_SECRETSERVICES = 't'; /* "tajné služby */
const char ACTION_INFORMATION = 'i';    /* "informace o ostatních královstvích */
const char ACTION_ROBBERY = 'l';        /* "loupež", format is: l kingdom_id */

/* auxiliary values to reset history after successfull robbery / information action (e.g. to resolve case when robberying for 3 rounds causes robbery in last 2 rounds */
const char ACTION_ROBBERY_DONE = 'L';
const char ACTION_INFORMATION_DONE = 'I'; 

using namespace std;

const string run_bot_under_user[4] = { "botuser0", "botuser1", "botuser2", "botuser3" };

typedef char response_t[128];

class PlayField
{
	struct kingdom_info_t {
		unsigned land;
		unsigned soldiers;
		unsigned peasants;
		unsigned arms;
		unsigned farming;
		unsigned food;
		unsigned secret_services;
		char last_moves[MAX_ACT_HISTORY];
	};

public:
	PlayField(const string _filename);
	~PlayField();
	bool loaded_ok() { return field_loaded; };
	bool game_finished() { return _game_finished; };
	int play_one_round(response_t* moves, string* bots_dir, bool write_to_disk=true);
	unsigned get_current_round() { return current_round; };
	unsigned get_remaining_rounds() { return total_rounds - current_round; };
	string get_current_filename() { return filename; };
	void get_kingdoms_lands(int lands[NUM_PLAYERS]);
	bool is_kingdom_alive(unsigned player) { return (kingdom[player].land > 0); }
	string get_kingdom_details(unsigned player);
	void set_debug(bool _debug) { debug = _debug; };

private:
	bool field_loaded;
	bool _game_finished;
	bool debug;
	unsigned current_round, total_rounds;
	unsigned dead_kingdoms;
	string filename;
	kingdom_info_t kingdom[NUM_PLAYERS];/* improve skills and add people, earn food, prepare battles */
	void write_playfield_to_disk(const char* mode);
	void increase_current_round() {current_round++; if (current_round == total_rounds) { _game_finished = true;  }}
	void update_last_moves(int player, char move);
};

#endif  /* !PLAYFIELD_H */
