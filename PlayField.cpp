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
#include <string.h>

using namespace std;

PlayField::PlayField(const string _filename) : filename(_filename) {
    field_loaded=false;
    field=NULL;
    FILE *fd;
    fd = fopen(filename.c_str(),"r");
    if (!fd) {
	fprintf(stderr, "Can not access file with playing field: %s\n", filename.c_str());
        return;
    }
    
    /* read current round number, total rounds and remaining flowers until end of the game */
    fscanf(fd,"%u %u %u\n", &current_round, &total_rounds, &remaining_flowers);

    /* read width and height of the playing field, allocate field */
    fscanf(fd,"%u %u\n", &width, &height);
    field = new char [width*height+1]; // the "+1" is due to automatic \0 append in fgets

    /* read data about worms */
    unsigned head_x, head_y, tail_x, tail_y;
    dead_worms = 0;
    for (int i = 0; i < NUM_PLAYERS; i++ ) {
        fscanf(fd,"%u %u %u %u %u %u %i\n", &head_x, &head_y, &tail_x, &tail_y, &(worms[i].rounds_frozen), &(worms[i].bonus), &(worms[i].points));
	worms[i].head_position=head_y*width+head_x;
	worms[i].tail_position=tail_y*width+tail_x;
	if (worms[i].rounds_frozen == FROZEN_WHEN_DIED)
	    dead_worms++;
    }

    /* read playfiled itself */
    for (unsigned y = 0; y < height; y++) {
	fgets(field+y*width, width+1, fd); // the "+1" is due to automatic \0 append in fgets
	fscanf(fd,"\n");
    }
    
    fclose(fd);
    field_loaded=true;
    _game_finished=((current_round >= total_rounds) || (remaining_flowers <= 0) || (dead_worms == NUM_PLAYERS));
}


PlayField::~PlayField() {
    if (field) {
	delete[] field;
    }
}


void PlayField::write_playfield_to_disk(bool rewrite_original_file) {
    string new_filename(filename);
    if (!(rewrite_original_file)) {
	char s[10], s2[10];
	sprintf(s, ".%04d", current_round-1);
	sprintf(s2, ".%04d", current_round);
	if (new_filename.rfind(s) == new_filename.length()-strlen(s)) {
	    new_filename.replace(new_filename.length()-strlen(s), strlen(s), "");
	}
	new_filename += s2;
	filename=new_filename; // necessary due to writing to disk after next round played
    }
    FILE *fd;
    fd = fopen(new_filename.c_str(),"w+");
    
    /* write current round, total rounds and remaining flowers */
    fprintf(fd,"%u %u %u\n", current_round, total_rounds, remaining_flowers);

    /* write width and height of the playing field */
    fprintf(fd,"%u %u\n", width, height);

    /* write data about worms */
    for (int i = 0; i < NUM_PLAYERS; i++ ) {
        fprintf(fd,"%u %u %u %u %u %u %i\n", worms[i].head_position%width, worms[i].head_position/width, worms[i].tail_position%width, worms[i].tail_position/width, worms[i].rounds_frozen, worms[i].bonus, worms[i].points);
    }

    /* write playfiled itself */
    for (unsigned y = 0; y < height; y++) {
	for (unsigned x = 0; x < width; x++)
            fputc(field[y*width+x], fd);
	fputc('\n', fd);
    }

    fclose(fd);
}


void PlayField::play_one_round(char* moves, bool write_to_disk, bool rewrite_original_file) {
    /* increment current round */
    if (current_round >= total_rounds) {
	fprintf(stderr,"Error: trying to play more rounds than maximum defined %d.", total_rounds);
	return;
    }
    if (++current_round == total_rounds) { // then we are playing the very last round
        // _game_finished is updated at the end of this method
	cout << "The latest round to be played." << endl;
    }

    /* canonise moves just to 'L', 'R' or ' '(straight on) */
    for (unsigned i = 0 ; i < NUM_PLAYERS; i++ ) {
        moves[i]=toupper(moves[i]);
        if ((moves[i] != LEFT_MOVE_CHARACTER) && (moves[i] != RIGHT_MOVE_CHARACTER))
	    moves[i]=STRAIGHT_MOVE_CHARACTER;
    }

    /* clear round-related data in worms[] */
    for (unsigned i = 0 ; i < NUM_PLAYERS; i++ ) {
	worms[i].new_head_position = worms[i].head_position;
	worms[i].eaten_this_round = FIELD_EMPTY;
    }

    /* move heads and resolve potential conflicts, decide who will consume flowers or attack others */
    //set auxiliary field where aux_field[i] is:
    // = 0 if field i is empty,
    // = 1 if field will contain one head of a worm after all moves,
    // > 1 if the field contains already a wall, a worm or more than 1 head after the moves of worms
    short aux_field[width*height]; 
    for (unsigned i = 0; i < width*height; i++) {
        aux_field[i] = ((field[i]==FIELD_EMPTY) || (field[i]==FIELD_FLOWER)|| (field[i]==FIELD_BONUS) || (field[i]==FIELD_ICE)) ? 0 : 2;
    }
    // compute new heads of worms based on their current position and their movement
    for (unsigned i = 0; i < NUM_PLAYERS; i++) {
        //move heads only if worm is alive and not frozen
	if (worms[i].rounds_frozen == 0) {
	    simulate_move_head(i, moves[i]);
	    //increment the aux.field where the new head is to be
	    aux_field[worms[i].new_head_position]++;
	    worms[i].eaten_this_round = field[worms[i].new_head_position];
	}
    }
    // check if some worm did not move to wrong field (wall, another worm incl. its moved head)
    for (unsigned i = 0; i < NUM_PLAYERS; i++) {
	if ((worms[i].rounds_frozen == 0) && (aux_field[worms[i].new_head_position] > 1)) {
	    // the worm crashed
	    worms[i].rounds_frozen = FROZEN_WHEN_DIED;
	    worms[i].points /= DYING_MALUS_DIVIDE;
	    worms[i].eaten_this_round = FIELD_EMPTY;
	    dead_worms++;
	    printf("Worm %d died in turn %d.\n", i, current_round);
	}
    }
    // finally move heads: set field[new_head_position] properly and update head_position
    // if the worm did not move (i.e. is frozen or dead), then the switch below does not cause updating field - correct
    for (unsigned i = 0; i < NUM_PLAYERS; i++) {
	//move heads of not-frozen worms only
	if (worms[i].rounds_frozen == 0) {
    	    //as switch can't handle variables in case branches, we use if-elif-else format
 	    int diff = (int)worms[i].new_head_position-worms[i].head_position;
	    if (diff == -(int)width) // worm moving up
	        field[worms[i].head_position] = field[worms[i].new_head_position] = base_direction_per_worm[i] + 0;
	    else if (diff == 1) //worm moving right
	        field[worms[i].head_position] = field[worms[i].new_head_position] = base_direction_per_worm[i] + 1;
	    else if (diff == (int)width) //worm moving down
	        field[worms[i].head_position] = field[worms[i].new_head_position] = base_direction_per_worm[i] + 2;
	    else if (diff == -1) //wrom moving left
	        field[worms[i].head_position] = field[worms[i].new_head_position] = base_direction_per_worm[i] + 3;
	    //update head_position to the new one - valid regardless the worm moved or not (due to frozeness)
	    worms[i].head_position = worms[i].new_head_position;
	}
    }

    /* move tails - only if the worm has not eaten a flower and is not frozen */
    for (unsigned i = 0 ; i < NUM_PLAYERS; i++ ) {
	if ((worms[i].rounds_frozen == 0) && (worms[i].eaten_this_round != FIELD_FLOWER)) {
	    switch (field[worms[i].tail_position]-base_direction_per_worm[i]) {
		case 0: { //move tail up
		    field[worms[i].tail_position] = FIELD_EMPTY;
		    worms[i].tail_position -= width;
		    break;
		}
		case 1: { //move tail right
		    field[worms[i].tail_position] = FIELD_EMPTY;
		    worms[i].tail_position += 1;
		    break;
		}
		case 2: { //move tail down
		    field[worms[i].tail_position] = FIELD_EMPTY;
		    worms[i].tail_position += width;
		    break;
		}
		case 3: { //move tail left
		    field[worms[i].tail_position] = FIELD_EMPTY;
		    worms[i].tail_position -= 1;
		    break;
		}
	    }
	}
    }

    /* update points and frozeness of worms */
    //first decrease frozeness of already frozen worms - can not be in the for cycle below!
    for (unsigned i = 0 ; i < NUM_PLAYERS; i++ )
	if ((worms[i].rounds_frozen > 0) && (worms[i].rounds_frozen < FROZEN_WHEN_DIED))
	    worms[i].rounds_frozen--;
    //now update points and frozeness based on eaten stuff
    unsigned flowers_eaten=0, bonuses_eaten=0, ices_eaten=0;
    for (unsigned i = 0 ; i < NUM_PLAYERS; i++ ) {
	switch (worms[i].eaten_this_round) {
	    case (FIELD_FLOWER): {
		worms[i].points+=(1<<worms[i].bonus);
		worms[i].bonus=0;
		flowers_eaten++;
		break;
	    }
	    case (FIELD_BONUS): {
                worms[i].bonus++;
		bonuses_eaten++;
                break;
            }
	    case (FIELD_ICE): { //increase frozeness of other worms by 10 rounds
		for (unsigned j = 0; j < NUM_PLAYERS; j++)
		    if ((i!=j) && (worms[j].rounds_frozen < FROZEN_WHEN_DIED))
			worms[j].rounds_frozen += ROUNDS_FROZEN_PER_ICE*(worms[i].bonus+1);
		worms[i].bonus=0;
		ices_eaten++;
		break;
	    }
	} // end of switch
    }
    // if a worm ate an ice, reset bonuses of all worms
    if (ices_eaten > 0)
      for (unsigned i = 0 ; i < NUM_PLAYERS; i++ )
	worms[i].bonus=0;
    //update remaining_flowers properly
    if (remaining_flowers <= flowers_eaten) //(more than) all remaining flowers eaten
	remaining_flowers = 0;
    else
	remaining_flowers -= flowers_eaten;

    /* generate new flowers and ices */
    // count empty fields to prevent end-less loop
    unsigned empty_fields=0;
    for (unsigned i = 0; i < width*height; i++) {
	if (field[i] == FIELD_EMPTY)
	    empty_fields++;
    }
    // generate new flowers (and then ices) if necessary (and until empty fields - that is why we need empty_fields variable)
    generate_new_stuff(flowers_eaten, &empty_fields, FIELD_FLOWER);
    generate_new_stuff(ices_eaten, &empty_fields, FIELD_ICE);
    generate_new_stuff(bonuses_eaten, &empty_fields, FIELD_BONUS);

    /* update whether the game hasn't just finished */
    _game_finished=((current_round >= total_rounds) || (remaining_flowers <= 0) || (dead_worms == NUM_PLAYERS));

    /* write to disk the new playfield */
    if (write_to_disk)
	write_playfield_to_disk(rewrite_original_file);
}

void PlayField::simulate_move_head(unsigned player, char move) {
    /* big switch adding absolute direction of worm and its move (left, right, straight) 
     * the switch utilises the fact that move letters 'L', 'R' and ' ' are "far enough" from each other
     * such that adding to them absolute direction (0 to 3) makes a unique number */
    switch (0+field[worms[player].head_position]-base_direction_per_worm[player] + move) {
        /* finally move to up: */
	case 3+RIGHT_MOVE_CHARACTER: // directing left, turn right
	case 0+STRAIGHT_MOVE_CHARACTER: // directing up, no change
	case 1+LEFT_MOVE_CHARACTER: // directing right, turn left
	    {
		field[worms[player].head_position] = base_direction_per_worm[player]+0;
		worms[player].new_head_position = worms[player].head_position-width;
		break;
	    }
	/* finally move to right: */
	case 0+RIGHT_MOVE_CHARACTER: // directing up, turn right
	case 1+STRAIGHT_MOVE_CHARACTER: // directing right, no change
	case 2+LEFT_MOVE_CHARACTER: // directing down, turn left
	    {
		field[worms[player].head_position] = base_direction_per_worm[player]+1;
		worms[player].new_head_position = worms[player].head_position+1;
		break;
	    }
	/* finally move to down: */
	case 1+RIGHT_MOVE_CHARACTER: // directing right, turn right
	case 2+STRAIGHT_MOVE_CHARACTER: // directing down, no change
	case 3+LEFT_MOVE_CHARACTER: // directing left, turn left
	    {
		field[worms[player].head_position] = base_direction_per_worm[player]+2;
		worms[player].new_head_position = worms[player].head_position+width;
		break;
	    }
	/* finally move left: */
	case 2+RIGHT_MOVE_CHARACTER: // directing down, turn right
	case 3+STRAIGHT_MOVE_CHARACTER: // directing left, no change
	case 0+LEFT_MOVE_CHARACTER: // directing up, turn left
	    {
		field[worms[player].head_position] = base_direction_per_worm[player]+3;
		worms[player].new_head_position = worms[player].head_position-1;
		break;
	    }
	default: fprintf(stderr, "Invalid movement evaluation of worm %d with head on [%d,%d] and move \'%c\'\n", 
			player, worms[player].head_position%width, worms[player].head_position/width, move);
    }
}

void PlayField::generate_new_stuff(unsigned _eaten, unsigned *empty_fields, const char to_fill_char) {
    for (; ((_eaten > 0) && ((*empty_fields) > 0)); _eaten--, (*empty_fields)-- ) {
	unsigned i;
	do {
	    i=random()%(width*height);
	} while (field[i] != FIELD_EMPTY);
	field[i] = to_fill_char;
    }
}

void PlayField::get_worms_points(int points[]) {
    for (unsigned i = 0; i < NUM_PLAYERS; i++)
	points[i] = worms[i].points;
}
