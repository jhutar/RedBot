/*
 * The part utilizing OptionParser was adopted under the
 * following licence:
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "PlayField.h"
#include <cstdlib>
#include <fstream>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define FIELDCHAR_IS_EMPTY(ch) ((ch != FIELD_SHIP1) && (ch != FIELD_SHIP2))

using namespace std;

/* auxiliary function to convert unsigned to string */
string unsigned_to_string(unsigned u)
{
  string s="";
  if (u==0)
    s="0";
  while (u>0)
  {
    s = (char)(u%10+48) + s;
    u /= 10;
  }
  return s;
}

PlayField::PlayField(string *bots_dir, string _battlefields_dir)
{
  unsigned i, j;
  bool move_ok[NUM_PLAYERS]; // auxiliary here only
  _game_finished = false;
  battlefields_dir = _battlefields_dir;
  ofstream ofile;
  string filename = BATTLEFIELD_FILENAME;
  filename = battlefields_dir + "/" + filename + ".0";

  current_round = 0;
  /* setup some fleets stuff */
  for (i=0; i<NUM_PLAYERS; i++)
  {
    fleets[i].charges = MAX_CHARGES;
    fleets[i].ship_fields_alive = 0;
    fleets[i].points = 0;
    fleets[i].bot_dir = bots_dir[i];
    fleets[i].last_round = "";
    for (j=0; j<SHIPS; j++)
    {
      fleets[i].ship_fields_alive += ships_x_size[j]*ships_y_size[j];
    }
  }
  /* generate boats */
  generate_battlefield();

  /* write files - for both players and the overall battlefield */
  for (i=0; i<NUM_PLAYERS; i++)
  {
    print_battlefield_for_player(i);
    move_ok[i] = true;
  }
  ofile.open(filename.c_str());
  print_battlefield(ofile, move_ok);
  ofile.close();
}

PlayField::~PlayField()
{
  unsigned x;
  if (battlefield)
  {
    for (x=0; x<BATTLEFIELD_SIZE; x++)
    {
      delete[] battlefield[x];
    }
    delete[] battlefield;
  }
}

bool PlayField::is_fleet_alive(unsigned player)
{
  return (fleets[player].ship_fields_alive > 0);
}

void PlayField::increase_current_round()
{
  current_round++;
  if ((current_round == MAX_ROUNDS) || (!(is_fleet_alive(0))) || (!(is_fleet_alive(1))))
  {
    _game_finished = true;
  }
}

char PlayField::missile_to_field(unsigned player, int x, int y)
{
  char hit = battlefield[x][y];
  fleets[player].last_round += (string)" " + unsigned_to_string((unsigned)x) +
                               "," + unsigned_to_string((unsigned)y) + "(" +
                               hit + ")";
  switch (battlefield[x][y])
  {
    case FIELD_EMPTY:
    {
      battlefield[x][y] = FIELD_EMPTY_HIT;
      break;
    }
    case FIELD_SHIP1:
    case FIELD_SHIP2:
    {
      fleets[50-battlefield[x][y]].points++;
      fleets[battlefield[x][y]-49].ship_fields_alive--;
      battlefield[x][y] = (battlefield[x][y]==FIELD_SHIP1)?(FIELD_SHIP1_HIT):(FIELD_SHIP2_HIT);
      break;
    }
  }
  return hit;
}

void PlayField::fire_bomb(unsigned player, int x, int y)
{
  fleets[player].charges--;
  missile_to_field(player, x, y);
  if (x<BATTLEFIELD_SIZE-1)
  {
    missile_to_field(player, x+1, y);
  }
  if (y<BATTLEFIELD_SIZE-1)
  {
    missile_to_field(player, x, y+1);
  }
  if ((x<BATTLEFIELD_SIZE-1) && (y<BATTLEFIELD_SIZE-1))
  {
    missile_to_field(player, x+1, y+1);
  }
}

void PlayField::fire_torpedo(unsigned player, int x, int y, int x_diff, int y_diff)
{
  char hit = FIELD_EMPTY;
  fleets[player].charges--;
  x += x_diff;
  y += y_diff;
  while (FIELDCHAR_IS_EMPTY(hit) &&
        (x>=0) && (x<BATTLEFIELD_SIZE) && (y>=0) && (y<BATTLEFIELD_SIZE))
  {
    hit = missile_to_field(player, x, y);
    x += x_diff;
    y += y_diff;
  }
}

void PlayField::fire_fireworks(unsigned player, int x, int y)
{
  bool fireworks_hit[16];
  unsigned i, hits;
  int xx, yy;
  fleets[player].charges--;
  missile_to_field(player, x, y);

  /* generate FIREWORKS_HITS random numbers from range 0..15 */
  for (i=0; i<16; i++)
  {
    fireworks_hit[i] = false;
  }
  i=0;
  for (hits=0; hits<FIREWORKS_HITS; hits++)
  {
    do
    {
      i = (i+(rand()%16)) % 16;
    }
    while (fireworks_hit[i]);
    fireworks_hit[i]=true;
  }

  /* fire to the randomly generated fields */
  for (i=0; i<16; i++)
  {
    if (fireworks_hit[i])
    {
      xx = x + fireworks_pos_x[i];
      yy = y + fireworks_pos_y[i];
      if ((xx>=0) && (xx<BATTLEFIELD_SIZE) && (yy>=0) && (yy<BATTLEFIELD_SIZE))
      {
        missile_to_field(player, xx, yy);
      }
    }
  }
}


void PlayField::play_one_round(response_t* moves)
{
  int i, x, y, x_diff, y_diff;
  char order, direction;
  ofstream ofile;
  bool move_ok[NUM_PLAYERS];
  string filename = BATTLEFIELD_FILENAME;

  for (i=0; i<NUM_PLAYERS; i++)
  {
    fleets[i].last_round = (string)"\"" + moves[i];
    fleets[i].last_round += "\", hit:";
    move_ok[i] = (EOF != sscanf(moves[i], "%c %u %u", &order, &x, &y));
    move_ok[i] &= ((x >= 0) && (x < BATTLEFIELD_SIZE) 
               && (y >= 0) && (y < BATTLEFIELD_SIZE));
    if (move_ok[i])
    {
      switch (order)
      {
        case FIRE_MISSILE:
        {
          missile_to_field(i,x,y);
          break;
        }
        case FIRE_BOMB:
        {
          move_ok[i] = (fleets[i].charges > 0);
          if (move_ok[i])
          {
            fire_bomb(i, x, y);
          }
          break;
        }
        case FIRE_TORPEDO:
        {
          move_ok[i] = ((fleets[i].charges > 0) && (battlefield[x][y] == (i+49))
                     && (EOF != sscanf(moves[i], "%c %u %u %c", &order, &x, &y,
                                                                &direction)));
          if (move_ok[i])
          {
            x_diff=0; y_diff=0;
            switch (direction)
            {
              case DIRECTION_UP:
              {
                y_diff = -1;
                break;
              }
              case DIRECTION_RIGHT:
              {
                x_diff = 1;
                break;
              }
              case DIRECTION_DOWN:
              {
                y_diff = 1;
                break;
              }
              case DIRECTION_LEFT:
              {
                x_diff = -1;
                break;
              }
              default:
              {
                move_ok[i] = false;
                break;
              }
            } /* end of "switch (direction)" */
          }
          if (move_ok[i])
          {
            fire_torpedo(i, x, y, x_diff, y_diff);
          }
          break;
        }
        case FIRE_FIREWORKS:
        {
          move_ok[i] = ((fleets[i].charges > 0) && (battlefield[x][y] == (i+49)));
          if (move_ok[i])
          {
            fire_fireworks(i, x, y);
          }
          break;
        }
        default:
        {
          move_ok[i] = false;
          break;
        }
      } /* end of switch */
    }
    if (!move_ok[i])
    {
      cout << "Chybna odpoved " << (i+1) << "-teho hrace: \"" << moves[i] << "\"" << endl;
    }
  } /* end of for cycle */

  increase_current_round();
  filename = battlefields_dir + "/" + filename + "." + unsigned_to_string(current_round);
  ofile.open(filename.c_str());
  print_battlefield(ofile, move_ok);
  ofile.close();
  for (i=0; i<NUM_PLAYERS; i++)
  {
    print_battlefield_for_player(i);
  }
}

/* classic backtracking algorithm used: try to randomly put one boat after
 * another until there is a conflict (then remove latest boat) or all boats
 * are put
 */
bool PlayField::try_to_place_one_boat(unsigned boat)
{
  bool boat_put;
  int start_x, start_y, x_size, y_size, x, y;
  for (unsigned attempt=0; attempt<100; attempt++)
  {
    if (rand()%2==0)
    {
      x_size=ships_x_size[boat%SHIPS];
      y_size=ships_y_size[boat%SHIPS];
    }
    else
    {
      x_size=ships_y_size[boat%SHIPS];
      y_size=ships_x_size[boat%SHIPS];
    }
    start_x = rand()%(BATTLEFIELD_SIZE-x_size+1);
    start_y = rand()%(BATTLEFIELD_SIZE-y_size+1);
    boat_put=true; //until we find a conflict
    //check the boat itself
    for (x=0; x<x_size; x++)
    {
      for (y=0; y<y_size; y++)
      {
        boat_put&=(battlefield[start_x+x][start_y+y] == ' ');
      }
    }
    //check above the boat
    if (start_y>0)
    {
      for (x=MAX(start_x-1,0); x<MIN(start_x+x_size+1,BATTLEFIELD_SIZE); x++)
      {
        boat_put&=(battlefield[x][start_y-1] == ' ');
      }
    }
    //check right from the boat
    if (start_x+x_size<BATTLEFIELD_SIZE)
    {
      for (y=MAX(start_y-1,0); y<MIN(start_y+y_size+1,BATTLEFIELD_SIZE); y++)
      {
        boat_put&=(battlefield[start_x+x_size][y] == ' ');
      }
    }
    //check below the boat
    if (start_y+y_size<BATTLEFIELD_SIZE)
    {
      for (x=MAX(start_x-1,0); x<MIN(start_x+x_size+1,BATTLEFIELD_SIZE); x++)
      {
        boat_put&=(battlefield[x][start_y+y_size] == ' ');
      }
    }
    //check left from the boat
    if (start_x>0)
    {
      for (y=MAX(start_y-1,0); y<MIN(start_y+y_size+1,BATTLEFIELD_SIZE); y++)
      {
        boat_put&=(battlefield[start_x-1][y] == ' ');
      }
    }

    if (boat_put)
    {
      //really place the boat
      for (x=0; x<x_size; x++)
      {
        for (y=0; y<y_size; y++)
        {
          battlefield[start_x+x][start_y+y] = (boat/SHIPS) + 49;
        }
      }
      if (boat == SHIPS*NUM_PLAYERS - 1)
      {
        return true;
      }
      else
      {
        if (try_to_place_one_boat(boat+1))
        {
          return true;
        }
        else /* remove the boat, can't be placed */
        {
          for (x=0; x<x_size; x++)
          {
            for (y=0; y<y_size; y++)
            {
              battlefield[start_x+x][start_y+y] = ' ';
            }
          }
        }
      }
    }
  }
  return false;
}

void PlayField::generate_battlefield()
{
  unsigned x,y;
  battlefield = new char*[BATTLEFIELD_SIZE];
  for (x=0; x<BATTLEFIELD_SIZE; x++)
  {
    battlefield[x] = new char[BATTLEFIELD_SIZE];
    for (y=0; y<BATTLEFIELD_SIZE; y++)
      battlefield[x][y] = ' ';
  }
  /* start backtracking */
  playfield_generated = try_to_place_one_boat(0);
}

void PlayField::print_battlefield_for_player(unsigned player)
{
  unsigned x,y;
  FILE *fd;
  string filename = fleets[player].bot_dir + BATTLEFIELD_FILENAME;
  fd = fopen(filename.c_str(),"w");
  fprintf(fd, "%d %d %d\n", player+1, MAX_ROUNDS-current_round, fleets[player].charges);
  for (y=0; y<BATTLEFIELD_SIZE; y++)
  {
    for (x=0; x<BATTLEFIELD_SIZE; x++)
    {
      switch (battlefield[x][y])
      {
        case FIELD_EMPTY:
        case FIELD_EMPTY_HIT:
        {
          fprintf(fd, "%c", battlefield[x][y]);
          break;
        }
        case FIELD_SHIP1_HIT:
        {
          fprintf(fd, "%c", (player==0)?(FIELD_MY_SHIP_HIT):(FIELD_HIS_SHIP_HIT));
          break;
        }
        case FIELD_SHIP2_HIT:
        {
          fprintf(fd, "%c", (player==1)?(FIELD_MY_SHIP_HIT):(FIELD_HIS_SHIP_HIT));
          break;
        }
        default: /* not hit boat (i.e. FIELD_SHIP1 or FIELD_SHIP2), here we assumes FIELD_SHIP1 = '1' etc. */
        {
          fprintf(fd, "%c", (battlefield[x][y]==((char)(49 + player)))?(FIELD_MY_SHIP):(FIELD_EMPTY));
          break;
        }
      }
    }
    fprintf(fd, "\n");
  }
  fclose(fd);
}

void print_top_line(ostream &out)
{
  unsigned x;
  out << '+';
  for (x=1; x<BATTLEFIELD_SIZE+1; x++)
  {
    out << "-";
  }
  out << '+';
  out << endl;
}

void PlayField::print_battlefield(ostream &out, bool *move_ok)
{
  unsigned x,y;
  out << "remaining rounds: " << MAX_ROUNDS-current_round << endl;
  out << "points:";
  for (x=0; x<NUM_PLAYERS; x++)
  {
    out << ' ' << fleets[x].points;
  }
  out << endl << "charges:";
  for (x=0; x<NUM_PLAYERS; x++)
  {
    out << ' ' << fleets[x].charges;
  }
  out << endl;
  print_top_line(out);
  for (y=0; y<BATTLEFIELD_SIZE; y++)
  {
    out << "|";
    for (x=0; x<BATTLEFIELD_SIZE; x++)
      out << battlefield[x][y];
    out << "|" << endl;
  }
  print_top_line(out);
  for (x=0; x<NUM_PLAYERS; x++)
  {
    out << "Player " << x+1 << "("
        << fleets[x].bot_dir.substr(0, fleets[x].bot_dir.size()-1)
        << "): last command: " << fleets[x].last_round
        << ((move_ok[x])?(""):(" (INVALID)")) << endl;
  }
}

void PlayField::get_points(unsigned points[])
{
  unsigned i;
  for (i=0; i<NUM_PLAYERS; i++)
  {
    points[i] = fleets[i].points;
  }
}
