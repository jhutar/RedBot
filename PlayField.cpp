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
#include <iostream>
#include <cstdio>
#include <string>
#include <list>
#include <string.h>
#include <assert.h>
#include <sstream>

using namespace std;

PlayField::PlayField(const string _filename, string _battlefield_dir) : filename(_filename), battlefield_dir(_battlefield_dir)
{
  field_loaded=false;
  field=NULL;
  FILE *fd;
  unsigned i, j;
  char c;
  char response[RESPONSE_LENGTH*3];

  fd = fopen(filename.c_str(),"r");
  if (!fd) {
    fprintf(stderr, "Nelze otevrit soubor s hracim planem: %s\n", filename.c_str());
    return;
  }

  /* nacti soucasne kolo a celkovy pocet kol */
  fscanf(fd,"%u %u\n", &current_round, &total_rounds);

  /* nacti sirku a vysku planu, alokuj hraci plan */
  fscanf(fd,"%u %u\n", &width, &height);
  field = new char [width*height];

  /* nacti informace o hracich */
  for (i = 0; i < NUM_PLAYERS; i++ ) {
    fscanf(fd,"Hrac %*u: Body:%u Zakladna:[%u,%u] Rakety", &(players[i].points), &(players[i].home_base.x), &(players[i].home_base.y));
    for (j = 0; j < RACKETS_PER_PLAYER; j++) {
      fscanf(fd,"%c[%u,%u]", &c, &(players[i].rackets[j].x), &(players[i].rackets[j].y));
      players[i].rackets_shot[j]=false;
    }
    fgetc(fd);
  }

  /* precti odpovedi hracu z minuleho kola */
  for (i = 0; i < NUM_PLAYERS; i++ ) {
    fscanf(fd,"Odpoved hrace %*u:");
    fgets(response, RESPONSE_LENGTH*3, fd);
    players[i].last_move = response;
    players[i].last_move = players[i].last_move.substr(0,
                             players[i].last_move.length() - 1); //odstran '\n'
  }

  /* nacti polohy asteroidu */
  for (i = 0; i<height; i++) {
    for (j = 0; j<width; j++) {
      field[i*width+j] = fgetc(fd);
    }
    fgetc(fd);
  }

  fclose(fd);
  field_loaded=true;
  write_playfield_to_disk(false);

  _game_finished=((current_round >= total_rounds) || (!makes_sense_to_play()));
}


PlayField::~PlayField() {
  if (field) {
    delete[] field;
  }
}

/* if there already exist an asteroid returns true */
bool PlayField::makes_sense_to_play() {
  int i;

  for (i = 0; i<RACKETS_PER_PLAYER; i++ ) {
    if (strchr(field, FIELD_ASTEROID+i+1)!=NULL)
      return true;
  }

  return false;
}

void PlayField::write_playfield_to_disk(bool rewrite_original_file)
{
  string new_filename(filename);
  size_t last_dir_delim = filename.find("/");
  if (last_dir_delim != string::npos)
    new_filename = filename.substr(last_dir_delim+1);
  char s[10], s2[10];
  FILE *fd;
  unsigned i, j;

  if (!(rewrite_original_file)) {
    sprintf(s, ".%04d", current_round-1);
    sprintf(s2, ".%04d", current_round);
    if (new_filename.rfind(s) == new_filename.length()-strlen(s)) {
      new_filename.replace(new_filename.length()-strlen(s), strlen(s), "");
    }
    if (new_filename.rfind(s2) != new_filename.length()-strlen(s2)) {
      new_filename += s2;
    }
    filename=new_filename; /* nutne kvuli zapisu na disk po odehrani dalsiho kola */
  }
  new_filename = battlefield_dir + "/" + new_filename;

  fd = fopen(new_filename.c_str(),"w+");
  if (fd == NULL) {
    cerr << "soubor " << new_filename.c_str() << " nelze vytvorit" << endl;
    return;
  }
  fprintf(fd,"%u %u\n", current_round, total_rounds);

  /* zapis sirku a vysku hraciho planu */
  fprintf(fd,"%u %u\n", width, height);

  /* zapis data o hracich */
  for (i = 0; i < NUM_PLAYERS; i++ ) {
    fprintf(fd,"Hrac %u: Body:%u Zakladna:[%u,%u] Rakety", i+1, players[i].points, players[i].home_base.x, players[i].home_base.y);
    for (j = 0; j < RACKETS_PER_PLAYER; j++) {
      fprintf(fd,":[%u,%u]", players[i].rackets[j].x, players[i].rackets[j].y);
    }
    fprintf(fd,"\n");
  }
  i=0;
  for (i=0; i<NUM_PLAYERS; i++) {
    fprintf(fd,"Odpoved hrace %u:%s\n", i+1, players[i].last_move.c_str());
  }

  /* zapis hraci plan samotny */
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++)
      fprintf(fd, "%c", field[i*width+j]);
    fputc('\n', fd);
  }

  fclose(fd);
}


bool PlayField::new_field_position(unsigned x, unsigned y, char direction, unsigned& newpos) {
  return new_field_position(y*width+x, direction, newpos);
}

bool PlayField::new_field_position(unsigned oldpos, char direction, unsigned& newpos) {
  switch (direction) {
    case LEFT_MOVE_CHARACTER: {
      newpos=oldpos-1;
      break;
    }
    case RIGHT_MOVE_CHARACTER: {
      newpos=oldpos+1;
      break;
    }
    case UP_MOVE_CHARACTER: {
      newpos=oldpos-width;
      break;
    }
    case DOWN_MOVE_CHARACTER: {
      newpos=oldpos+width;
      break;
    }
    default:
      return false;
  }
  return true;
}

bool is_valid_direction(char ch) {
  return ((ch==LEFT_MOVE_CHARACTER)||(ch==RIGHT_MOVE_CHARACTER)
           ||(ch==UP_MOVE_CHARACTER)||(ch==DOWN_MOVE_CHARACTER));
}

int direction_to_int(char ch) {
  switch (ch) {
    case LEFT_MOVE_CHARACTER: {
      return 0;
    }
    case RIGHT_MOVE_CHARACTER: {
      return 1;
    }
    case UP_MOVE_CHARACTER: {
      return 2;
    }
    case DOWN_MOVE_CHARACTER: {
      return 3;
    }
  }
  return 5; /*vyhodi out-of-bounds chybu; nemelo by nastat diky is_valid_direction*/
}

int PlayField::movement_to_diff_pos(char ch) {
  switch (ch) {
    case LEFT_MOVE_CHARACTER:
      return -1;
    case RIGHT_MOVE_CHARACTER:
      return +1;
    case UP_MOVE_CHARACTER:
      return -width;
    case DOWN_MOVE_CHARACTER:
      return +width;
  }
  return 0;
}

/*svazano s implementaci direction_to_int !!!*/
int PlayField::direction_to_diff_pos(unsigned dir) {
  switch (dir) {
    case 0:
      return -1;
    case 1:
      return +1;
    case 2:
      return -width;
    case 3:
      return +width;
  }
  return 0; /*nemelo by nastat*/
}

void PlayField::shot_to_racket(unsigned p, unsigned r) {
  players[p].rackets[r].x = players[p].home_base.x;
  players[p].rackets[r].y = players[p].home_base.y;
  players[p].rackets_shot[r] = true;
}

bool PlayField::ok_to_move(unsigned pos, int dir) {
  return (((dir==-1)&&(pos%width>0)) //pohyb vlevo
        ||((dir==+1)&&(pos%width<width-1)) //pohyb vpravo
        ||((dir==(int)(-width))&&(pos>=width)) //pohyb nahoru
        ||((dir==(int)(+width))&&(pos<width*height-width))); //pohyb dolu
}

void PlayField::add_moves_result(string & move, unsigned shot_length, const char* result)
{
  char move_result[30];
  sprintf(move_result, " (%u,%s)", shot_length, result);
  move = move + move_result;
}

void PlayField::add_moves_result(string & move, char asteroid_weight)
{
  char move_result[30];
  sprintf(move_result, " (%c)", asteroid_weight);
  move = move + move_result;
}

void PlayField::play_one_round(response_t* _moves, bool write_to_disk, bool rewrite_original_file)
{

  bool valid_move[NUM_PLAYERS][RACKETS_PER_PLAYER];
  bool valid_push[NUM_PLAYERS][RACKETS_PER_PLAYER];
  unsigned new_pos;
  int diff_pos;
  list<unsigned> shot_rackets;
  unsigned i,j,k;
  string moves[NUM_PLAYERS][RACKETS_PER_PLAYER];
  void *ret;
  /* pole pro obranu a tahani */
  list<char> aux_defend_field[width*height];
  unsigned aux_pulls_field[width*height][4];

  /* znuluju potrebne promenne */
  for (i=0; i<NUM_PLAYERS; i++) {
    for (j=0; j<RACKETS_PER_PLAYER; j++) {
      players[i].rackets_shot[j]=false;
      moves[i][j]=ERROR_ROBOT_MOVE;
      valid_move[i][j]=true;
      valid_push[i][j]=true;
    }
  }

  /* zvys pocet odehranych kol */
  if (current_round >= total_rounds) {
    cerr << "Chyba: odehrano vice kol nez je maximum " << total_rounds << endl;
    return;
  }

  /* hraje se posledni kolo */
  if ((++current_round == total_rounds) && (debug))
    cout << "Hraje se posledni kolo." << endl;

  /* nacti kroky hracu do promenne "moves" */
  for (i = 0; i < NUM_PLAYERS; i++ ) {
    players[i].last_move=_moves[i];
    istringstream iss(_moves[i]);
    for (j=0; j<RACKETS_PER_PLAYER; j++) {
      ret = getline(iss, moves[i][j], MOVES_DELIMETER);
      if (ret == NULL) {
        /* nebyla zadana zadna akce */
        cerr << "Chybna odpoved " << i+1 << ". hrace: prilis malo akci (" << j << ")." << endl;
        for (unsigned k=j; k<RACKETS_PER_PLAYER; k++)
          valid_move[i][k]=false;
      }
    }
  }

  /* poradi vyhodnocovani:
   * 1. lety raket
   * 2. obrana
   * 3. strelba
   * 4. tahani asteroidu */

  /* zresetuj aux_*_field a nastav aux_defend_field */
  for (i = 0; i<width*height; i++) {
    for (j = 0; j<4; j++) {
      aux_pulls_field[i][j]=0;
    }
    aux_defend_field[i].clear();
  }
  /* vyhodnot zda jde o validni akci, proved lety raket a nastav obrany */
  for (i = 0; i < NUM_PLAYERS; i++) {
    for ( j = 0; j < RACKETS_PER_PLAYER; j++) {
      new_pos=players[i].rackets[j].y*width+players[i].rackets[j].x;
      switch (toupper(moves[i][j][0])) {
        case DEFEND_ACTION: { /* obrana */
            if (aux_defend_field[new_pos].empty() || aux_defend_field[new_pos].front()!=AUX_FIELD_WALL)
              aux_defend_field[new_pos].push_front(AUX_FIELD_WALL);
            break;
          }
        case MOVE_ACTION: { /* let raket */
            if ((moves[i][j].length()<2) || (!is_valid_direction(toupper(moves[i][j][2])))) {
              cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: chybi nebo neplatny smer v odpovedi \'" << moves[i][j] << "\'" << endl;
              valid_move[i][j]=false;
              break;
            }

            diff_pos=movement_to_diff_pos(toupper(moves[i][j][2]));
            if (!ok_to_move(new_pos,diff_pos)) {
              cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: pokus o odlet z hraciho planu v odpovedi \'" << moves[i][j] << "\'" << endl;
              valid_move[i][j]=false;
              break;
            }

            new_pos=new_pos+diff_pos;
            if (moves[i][j].length()>4) { // raketa chce letet o 2 pole
              if (!is_valid_direction(toupper(moves[i][j][4]))) {
                cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: neplatny druhy smer v odpovedi \'" << moves[i][j] << "\'" << endl;
                valid_move[i][j]=false;
                break;
              }

              diff_pos=movement_to_diff_pos(toupper(moves[i][j][4]));
              if (!ok_to_move(new_pos,diff_pos)) {
                cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: pokus o odlet z hraciho planu v odpovedi \'" << moves[i][j] << "\'" << endl;
                valid_move[i][j]=false;
                break;
              }
	      new_pos=new_pos+diff_pos;
            }

            players[i].rackets[j].x=new_pos%width;
            players[i].rackets[j].y=new_pos/width;
            break;
          }
        case PULL_ACTION: /* tahnuti asteroidu nyni se ignoruje */
          break;
        case SHOT_ACTION: /* strelba - nyni se ignoruje */
          break;
        default: {
          if (valid_move[i][j])
            cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: neznama akce \'" << moves[i][j] << "\'" << endl;
          valid_move[i][j]=false;
        }
      }
      // nastav ze na tomto policku stoji raketa - at muze byt zastrelena
      if (aux_defend_field[new_pos].empty() || aux_defend_field[new_pos].front()!=AUX_FIELD_WALL) {
        aux_defend_field[new_pos].push_front((char)(i*RACKETS_PER_PLAYER+j));
        // odkaz na raketu ktery by zemrel po pripadne strelbe
      }
    }
  }

  /* vyhodnot strelbu */
  for (i = 0; i < NUM_PLAYERS; i++)
    for (j = 0; j < RACKETS_PER_PLAYER; j++)
      if ((!is_racket_shot(i,j))&&(valid_move[i][j])&&(toupper(moves[i][j][0])==SHOT_ACTION)) {
        new_pos=players[i].rackets[j].y*width+players[i].rackets[j].x;
        diff_pos=movement_to_diff_pos(toupper(moves[i][j][2]));
        if (diff_pos==0) {
          cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: neznamy smer strelby v \'" << moves[i][j] << "\'" << endl;
          valid_move[i][j]=false;
        }
        k=0;
        if (valid_move[i][j]) { 
          if (ok_to_move(new_pos,diff_pos)) {
            do {
              new_pos+=diff_pos;
  	      k++;
            }
            while ((aux_defend_field[new_pos].empty())&&(ok_to_move(new_pos,diff_pos)));
            if (!aux_defend_field[new_pos].empty()) { // neco jsem zasahl
              if (aux_defend_field[new_pos].front()!=AUX_FIELD_WALL) { // zasah nebranici se rakety
                shot_rackets.push_front(new_pos);
                add_moves_result(moves[i][j], k, "zasah");
              }
              else { //zasahl jsem branici raketu
                add_moves_result(moves[i][j], k, "obrana");
              }
            }
            else { // nezasahl jsem nic
              add_moves_result(moves[i][j], k, "mimo");
            }
          }
          else { // strilim hned mimo plan
            add_moves_result(moves[i][j], 0, "mimo");
          }
        }
      }

  // az ted odstran zasazene rakety - aby se mohly strelit 2 navzajem
  while (!shot_rackets.empty()) {
    new_pos=shot_rackets.front();
    shot_rackets.pop_front();
    if (aux_defend_field[new_pos].size()) { // jeste je na policku koho zastrelit
      i=rand()%(aux_defend_field[new_pos].size());
      list<char>::iterator iter=aux_defend_field[new_pos].begin();
      while (i>0) {
        i--;
        iter++;
      }
      j=((unsigned)(*iter))%RACKETS_PER_PLAYER;
      i=((unsigned)(*iter))/RACKETS_PER_PLAYER;
      shot_to_racket(i,j);
      aux_defend_field[new_pos].erase(iter); // odstran raketu ze seznamu, je zasazena (kdyby byla dalsi strela na stejne policko)
      if (debug)
        cout << "Zastrelena " << j+1 << ".raketa " << i+1 << ". hrace na pozici [" << new_pos%width << "," << new_pos/width << "]." << endl;
    }
  }
  // promaz rakety a "zdi" kam se dalo strilet
  for (i = 0; i<width*height; i++)
    aux_defend_field[i].clear();

  /* presun asteroidy */
  // nejdriv nastav pomocne pole
  for (i = 0; i < NUM_PLAYERS; i++)
    for (j = 0; j < RACKETS_PER_PLAYER; j++)
      if ((!is_racket_shot(i,j))&&(valid_move[i][j])&&(toupper(moves[i][j][0])==PULL_ACTION)) {
        new_pos=players[i].rackets[j].y*width+players[i].rackets[j].x;
        diff_pos=movement_to_diff_pos(toupper(moves[i][j][2]));
        if (!ok_to_move(new_pos,diff_pos)) {
          cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: pokus o tahnuti pryc z hraciho planu v odpovedi \'" << moves[i][j] << "\'" << endl;
          valid_move[i][j]=false;
        }
        else { //TODO: cerr odpovedi kdyz 1. netahnu asteroid, 2. tahnu ale neutahnu
          if (!is_here_asteroid(new_pos)) {
            cerr << "Chybna odpoved " << i+1 << ". hrace " << j+1 << ". raketa: pokus o tahnuti neexistujiciho asteroidu" << endl;
            valid_move[i][j]=false;
          }
	  else {
            aux_pulls_field[new_pos][direction_to_int(toupper(moves[i][j][2]))]++;
  	    valid_push[i][j]=false; // docasne dokud nezjistim zda se posun asteroidu povedl
          }
        }
      }

  // ted vyhodnot konfliktni tahani
  // prochazim tahani dvakrat: v prvnim cyklu zjisti ktery asteroid lze odtahnout a napln pomocne pole na pozice asteroidu
  // ve druhem kole je skutecne pretahni pokud netahnu asteroid na pole s jinym asteroidem
  unsigned aux_asteroid_field[width*height];
  for (i = 0; i<width*height; i++)
    aux_asteroid_field[i]=(((field[i]>FIELD_ASTEROID)&&(field[i]<=FIELD_ASTEROID+RACKETS_PER_PLAYER))?(1):(0));

  for (unsigned aux_round=0; aux_round<2; aux_round++)
    for (i = 0; i<width*height; i++) {
      unsigned sum_weight=(unsigned)(field[i])-48; unsigned max_pull=0; unsigned max_pull_dir=0;
      for (j=0; j<4; j++) {
        sum_weight+=aux_pulls_field[i][j];
        if (max_pull<aux_pulls_field[i][j]) {
          max_pull=aux_pulls_field[i][j];
          max_pull_dir=j;
        }
      }
      if (max_pull*2>=sum_weight) { // nasimuluj ci udelej presun asteroidu
        diff_pos=direction_to_diff_pos(max_pull_dir);
        new_pos=i+diff_pos;
        if (aux_round==0) { //nasimuluj presun
          // nepresouvam asteroid na zakladnu nejakeho hrace?
          unsigned p=0;
          while ((p<NUM_PLAYERS)&&
                ((players[p].home_base.x!=new_pos%width)
                  ||(players[p].home_base.y!=new_pos/width)))
            p++;
          if (p==NUM_PLAYERS) //zadny hrac nema na nove pozici zakladnu, zkus simulovat presun
            aux_asteroid_field[new_pos]++;
          else { // pridej body hraci (a nepresouvej asteroid jen jej smaz)
            move_rockets_after_push(i, new_pos, moves, valid_move, valid_push);
            players[p].points+=((unsigned)(field[i])-48);
            field[i]=FIELD_EMPTY;
          }
        }
        else {
          if (aux_asteroid_field[new_pos]==1) {
            move_rockets_after_push(i, new_pos, moves, valid_move, valid_push);
            field[new_pos]=field[i];
            field[i]=FIELD_EMPTY;
          }
          else
            cerr << "Pokus tahnout asteroid na pozici [" << new_pos%width << "," << new_pos/width << "] (" << new_pos << ") selhal: konflikt v polohach asteroidu." << endl;
        }
      }
    }
  // napis chybne odpovedi pokud se tahnuti asteroidu nezdarilo
  for (i = 0; i < NUM_PLAYERS; i++)
    for (j = 0; j < RACKETS_PER_PLAYER; j++)
      if ((!is_racket_shot(i,j))&&(valid_move[i][j])
          &&(toupper(moves[i][j][0])==PULL_ACTION)&&(valid_push[i][j]==false)) {
        cerr << "Neprovedena odpoved " << i+1 << ". hrace " << j+1 << ". raketa: pokus o tahnuti asteroidu nevysel" << endl;
        valid_move[i][j]=false;
      }

  /* neni konec hry = probehlo posledni kolo nebo dosli asteroidy */
  _game_finished=((current_round >= total_rounds) || (!makes_sense_to_play()));


  /* zapis na disk */
  /* nejdriz zkanonizuj odpovedi do unifikovane formy */
  canonize_moves(moves, valid_move);
  if (write_to_disk)
    write_playfield_to_disk(rewrite_original_file);
}

void PlayField::move_rockets_after_push(unsigned from, unsigned to,
    string moves[NUM_PLAYERS][RACKETS_PER_PLAYER],
    bool valid_move[NUM_PLAYERS][RACKETS_PER_PLAYER],
    bool (&valid_push)[NUM_PLAYERS][RACKETS_PER_PLAYER]) {
  unsigned i,j,pos,new_pos;
  for (i = 0; i < NUM_PLAYERS; i++)
    for (j = 0; j < RACKETS_PER_PLAYER; j++)
      if ((!is_racket_shot(i,j))&&(valid_move[i][j])&&(toupper(moves[i][j][0])==PULL_ACTION)) {
        pos=players[i].rackets[j].y*width+players[i].rackets[j].x;
	new_pos=pos+movement_to_diff_pos(toupper(moves[i][j][2]));
	if ((from==pos)&&(to==new_pos)) {
          players[i].rackets[j].x=new_pos%width;
          players[i].rackets[j].y=new_pos/width;
	  valid_push[i][j]=true;
          add_moves_result(moves[i][j],field[from]);
        }
      }
}

void PlayField::canonize_moves (string moves[NUM_PLAYERS][RACKETS_PER_PLAYER], bool valid_move[NUM_PLAYERS][RACKETS_PER_PLAYER]) {
  for (unsigned i=0; i<NUM_PLAYERS; i++) {
    players[i].last_move="(" + players[i].last_move + ")";
    for (unsigned j=0; j<NUM_PLAYERS; j++) {
      string robot_move = moves[i][j];
      if (!valid_move[i][j])
        robot_move = ERROR_ROBOT_MOVE;
      for (unsigned k=0; (k<robot_move.length())&&(robot_move[k]!='('); k++)
        if (k%2==0)
        robot_move[k] = (k%2==0) ? (toupper(robot_move[k])) : (' ');
      players[i].last_move = players[i].last_move + MOVES_DELIMETER + robot_move;
    }
//    players[i].last_move =  players[i].last_move + MOVES_DELIMETER + "(" + orig_answer + ")";
  }
}

void PlayField::get_player_points(unsigned points[]) {
  for (unsigned i = 0; i < NUM_PLAYERS; i++)
    points[i] = players[i].points;
}
