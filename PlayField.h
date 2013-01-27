#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <string>

#define NUM_PLAYERS 4
#define ONE_ROUND_DELAY 3000 //in milliseconds
#define CHECK_BOTS_INTERVAL 100 //in milliseconds
#define FIELD_DEFAULT_FILENAME "/tmp/field.txt"
const char FIELD_EMPTY = ' ';
const char FIELD_WALL = '#';
const char FIELD_FLOWER = '.';
const char FIELD_BONUS = '+';
const char FIELD_ICE = '*';
#define ROUNDS_FROZEN_PER_ICE 5
#define DYING_MALUS_DIVIDE 2
const char LEFT_MOVE_CHARACTER = 'L';
const char RIGHT_MOVE_CHARACTER = 'R';
const char STRAIGHT_MOVE_CHARACTER = ' ';
//#define DYING_PENALTY_POINTS 10
#define MAX_ROUNDS_PLAYED 10000
const unsigned FROZEN_WHEN_DIED = 999999999; //must be more than number_of_rounds_in_any_game*ROUNDS_FROZEN_PER_HIT
const char base_direction_per_worm[NUM_PLAYERS] = { 'a', 'h', 'o', 'w' }; //must be of size NUM_PLAYERS

/* directions used (offset from first letter):
 * 0: up
 * 1: right
 * 2: down
 * 3: left
 */

using namespace std;

const string run_bot_under_user[4] = { "botuser0", "botuser1", "botuser2", "botuser3" };

class PlayField
{
    struct worm_info_t {
        unsigned head_position;
	unsigned new_head_position;
	unsigned tail_position;
    	int points;
	unsigned rounds_frozen;
	unsigned bonus;
	char eaten_this_round;
    };

    public:
    	PlayField(const string _filename);
    	~PlayField();
	bool loaded_ok() { return field_loaded; };
	bool game_finished() { return _game_finished; };
	void write_playfield_to_disk(bool rewrite_original_file);
	void play_one_round(char* moves, bool write_to_disk=true, bool rewrite_original_file=false);
	unsigned get_current_round() { return current_round; };
	string get_current_filename() { return filename; };
	void get_worms_points(int points[NUM_PLAYERS]);
	bool is_worm_alive(unsigned player) { return (worms[player].rounds_frozen < FROZEN_WHEN_DIED); }
    private:
	void simulate_move_head(unsigned player, char move);
	void generate_new_stuff(unsigned _eaten, unsigned *empty_fields, const char to_fill_char);
	bool field_loaded;
	bool _game_finished;
    	unsigned current_round, total_rounds;
	unsigned remaining_flowers;
    	unsigned width, height;
	unsigned targeted_flowers;
	unsigned dead_worms;
	string filename;
    	worm_info_t worms[NUM_PLAYERS];
    	char* field;
};

#endif  /* !PLAYFIELD_H */
