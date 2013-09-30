#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <string>
#include <climits>

#define NUM_PLAYERS		2
#define RACKETS_PER_PLAYER	2
#define ONE_ROUND_DELAY		3000	/* maximalni delka jednoho kola v milisekundach*/
#define CHECK_BOTS_INTERVAL	100	/* v milisekundach */
#define FIELD_DEFAULT_FILENAME	"playfield.txt"
#define RESPONSE_LENGTH		256

const char FIELD_EMPTY = ' ';
const char FIELD_ASTEROID = '0';	/* plus jeho vaha (1 az RACKETS_PER_PLAYER)*/

const char AUX_FIELD_WALL = '#';	/* pomocne pro aux_defend_field */

const char LEFT_MOVE_CHARACTER = 'L';	/* vLevo */
const char RIGHT_MOVE_CHARACTER = 'P';	/* vPravo */
const char UP_MOVE_CHARACTER = 'N';	/* Nahoru */
const char DOWN_MOVE_CHARACTER = 'D';	/* Dolu */

const char MOVE_ACTION = 'L';		/* "L D" (let dolu) */
const char PULL_ACTION = 'T';		/* "T L" (tahni asteroid vlevo) */
const char SHOT_ACTION = 'S';		/* "S L" (strilej vlevo) */
const char DEFEND_ACTION = 'B';		/* "B" (bran se) */
const char MOVES_DELIMETER = ':';
const char ERROR_ROBOT_MOVE = 'E';

using namespace std;

typedef char response_t[RESPONSE_LENGTH];

const string run_bot_under_user[NUM_PLAYERS] = {  "botuser0", "botuser1"};

class PlayField
{
  struct position_t {
    unsigned x;
    unsigned y;
  };

  struct player_info_t {
    position_t home_base;
    position_t rackets[RACKETS_PER_PLAYER];
    bool rackets_shot[RACKETS_PER_PLAYER];
    unsigned points;
    string last_move;
  };

  public:
    PlayField(const string _filename, string _battlefield_dir);
    ~PlayField();
    bool loaded_ok() { return field_loaded; };
    bool game_finished() { return _game_finished; };
    void write_playfield_to_disk(bool rewrite_original_file);
    void play_one_round(response_t *_moves, bool write_to_disk = true,
                        bool rewrite_original_file = false);
    unsigned get_current_round() { return current_round; };
    string get_current_filename() { return (battlefield_dir + "/" + filename); };
    void get_player_points(unsigned points[NUM_PLAYERS]);
    void set_debug(bool _debug) { debug = _debug; };

  private:
    bool field_loaded;
    bool _game_finished;
    unsigned current_round, total_rounds;
    unsigned width, height;
    unsigned remaining_asteroids;
    string filename;
    string battlefield_dir;
    player_info_t players[NUM_PLAYERS];
    char *field;
    bool makes_sense_to_play();
    void shot_to_racket(unsigned p, unsigned r);
    inline bool is_racket_shot(unsigned p, unsigned r)
    {
      return players[p].rackets_shot[p];
    }
    bool new_field_position(unsigned x, unsigned y, char direction,
                            unsigned & newpos);
    bool new_field_position(unsigned oldpos, char direction, unsigned & newpos);
    bool ok_to_move(unsigned pos, int dir);
    int movement_to_diff_pos(char ch);
    int direction_to_diff_pos(unsigned dir);
    void add_moves_result(string & move, unsigned shot_length, const char* result);
    void add_moves_result(string & move, char asteroid_weight);
    inline bool is_here_asteroid(unsigned pos)
    {
      return ((field[pos] > FIELD_ASTEROID) && 
              (field[pos] <= FIELD_ASTEROID+RACKETS_PER_PLAYER));
    }
    void move_rockets_after_push(unsigned from, unsigned to,
        string moves[NUM_PLAYERS][RACKETS_PER_PLAYER],
        bool valid_move[NUM_PLAYERS][RACKETS_PER_PLAYER],
        bool (&valid_push)[NUM_PLAYERS][RACKETS_PER_PLAYER]);
    void canonize_moves(string moves[NUM_PLAYERS][RACKETS_PER_PLAYER], bool valid_move[NUM_PLAYERS][RACKETS_PER_PLAYER]);
    bool debug;
};

#endif  /* !PLAYFIELD_H */
