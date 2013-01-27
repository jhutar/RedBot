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
#include <cmath>
#include <list>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


using namespace std;

struct attack_t {
	unsigned attacker;
	unsigned soldiers;
};

string get_relative_attack_list(list<attack_t> *attackers, unsigned me)
{
	string attack_list = "";
	char aux_ch[2];
	list<attack_t>::iterator it;
	for (it = attackers->begin(); it != attackers->end(); it++) {
		sprintf(aux_ch, "%d,", (4 + it->attacker - me) % 4);
		attack_list += aux_ch;
	}
	attack_list.erase(attack_list.size()-1);
	return attack_list;
}


PlayField::PlayField(const string _filename) : filename(_filename) {

	debug=false;
	field_loaded=false;
	int i;
	FILE *fd;

	fd = fopen(filename.c_str(),"r");
	if (!fd) {
		fprintf(stderr, "Can not access file with starting situation: %s\n", filename.c_str());
		return;
	}

	/* read current round number and total rounds */
	fscanf(fd,"%u/%u:", &current_round, &total_rounds);

	/* read data about kingdoms */
	dead_kingdoms = 0;
	for (i = 0; i < NUM_PLAYERS; i++ ) {
		fscanf(fd," %u %u %u %u %u %u %u\n", &kingdom[i].land,
			&kingdom[i].food, &kingdom[i].soldiers,
			&kingdom[i].peasants, &kingdom[i].arms,
			&kingdom[i].farming, &kingdom[i].secret_services);
		if (kingdom[i].land == 0) {
			dead_kingdoms++;
		}
	}

	fclose(fd);

	/* write snapshots of the game to file "filename + PLAY_SNAPSHOTS_FILE_SUFFIX" */
	filename += PLAY_SNAPSHOTS_FILE_SUFFIX;
	write_playfield_to_disk("w");

	field_loaded=true;
	_game_finished=((current_round >= total_rounds) ||
		(dead_kingdoms == NUM_PLAYERS - 1));
}


PlayField::~PlayField() {
}


void PlayField::write_playfield_to_disk(const char* mode)
{
	int i;
	FILE *fd;

	fd = fopen(filename.c_str(), mode);
	if (!fd) {
		fprintf(stderr, "Can not access file with starting situation: %s\n", filename.c_str());
		return;
	}

	/* write current round and total rounds */
	fprintf(fd,"%u/%u:", current_round, total_rounds);

	/* write data about kingdoms */
	for (i = 0; i < NUM_PLAYERS; i++ ) {
		fprintf(fd," %u %u %u %u %u %u %u", kingdom[i].land, kingdom[i].food,
			kingdom[i].soldiers, kingdom[i].peasants, kingdom[i].arms,
			kingdom[i].farming, kingdom[i].secret_services);
	}
	fprintf(fd,"\n");

	fclose(fd);
}

int PlayField::play_one_round(response_t* moves, string* bots_dir,
	bool write_to_disk)
{
	int left;
	int i;

	/* attack/defence/information/.. file descriptors per client */
	FILE *fd_attack[NUM_PLAYERS];
	FILE *fd_defence[NUM_PLAYERS];
	FILE *fd_information[NUM_PLAYERS];
	FILE *fd_robbery_attack[NUM_PLAYERS];
	FILE *fd_robbery_defence[NUM_PLAYERS];

	/* set of attackers to each kingdom - to avoid defining 
	  comparator of attack_t, set type replaced by list */
	list<attack_t> kingdom_attackers[NUM_PLAYERS];
	list<int> kingdom_robbers[NUM_PLAYERS];
	list<int> kingdom_act_information;

	unsigned defender;
	attack_t attack;
	int done;

	/* increment current round */
	if (current_round >= total_rounds) {
		fprintf(stderr,"Error: trying to play more rounds than maximum defined %d.", total_rounds);
		return 1;
	}

	/* open files for wirting attacks and defences of kingdoms */
	for (i = 0; i < NUM_PLAYERS; i++ ) {
		string attack_filename = bots_dir[i] + KINGDOM_ATTACK_FILENAME;
		string defence_filename = bots_dir[i] + KINGDOM_DEFENCE_FILENAME;
		string information_filename = bots_dir[i] + KINGDOM_INFORMATION_FILENAME;
		string robbery_attack_filename = bots_dir[i] + KINGDOM_ROBBERY_ATTACK_FILENAME;
		string robbery_defence_filename = bots_dir[i] + KINGDOM_ROBBERY_DEFENCE_FILENAME;

		fd_attack[i] = fopen(attack_filename.c_str(), "w");
		if (fd_attack[i] == NULL)
			return 1;

		fd_defence[i] = fopen(defence_filename.c_str(), "w");
		if (fd_defence[i] == NULL)
			return 1;

		fd_information[i] = fopen(information_filename.c_str(), "w");
                if (fd_information[i] == NULL)
                        return 1;
		
		fd_robbery_attack[i] = fopen(robbery_attack_filename.c_str(), "w");
                if (fd_robbery_attack[i] == NULL)
                        return 1;
		fd_robbery_defence[i] = fopen(robbery_defence_filename.c_str(), "w");
                if (fd_robbery_defence[i] == NULL)
                        return 1;
	}
	if (debug)
		cout << "	kroky    : ";

	for (i = 0; i < NUM_PLAYERS; i++) {
		/* update last_moves - history of moves */
		update_last_moves(i, moves[i][0]);
		/* improve skills and add people, earn food, prepare battles */
		done = 0;
		switch (moves[i][0]) {
			case ACTION_SOLDIER: {
				kingdom[i].soldiers++;
				if (debug)
					cout << i << ":vojak       ";
				done = 1;
				break;
			}
			case ACTION_PEASANT: {
				kingdom[i].peasants++;
				if (debug)
					cout << i << ":rolnik      ";
				done = 1;
				break;
			}
			case ACTION_ARMY: {
				kingdom[i].arms++;
				if (debug)
					cout << i << ":zbrojeni    ";
				done = 1;
				break;
			}
			case ACTION_FARMING: {
				kingdom[i].farming++;
				if (debug)
					cout << i << ":farmareni   ";
				done = 1;
				break;
			}
			case ACTION_HARVEST: {
				kingdom[i].food += (FARMING_MULTIPLIER
					* kingdom[i].peasants
					* (1+floor(kingdom[i].farming/FARMING_LEVEL_UP)));
				if (debug)
					cout << i << ":sklizen     ";
				done = 1;
				break;
			}
			case ACTION_SECRETSERVICES: {
				kingdom[i].secret_services++;
				if (debug)
					cout << i << ":tajneSluzby ";
				done = 1;
				break;
			}
                        case ACTION_ROBBERY: {
				for (left=0;
					((left<ROBBERY_ACT_REPEATED) && (kingdom[i].last_moves[left] == ACTION_ROBBERY));
					left++);
				if (left == ROBBERY_ACT_REPEATED) {
					if (EOF == sscanf(moves[i], "l %u", &defender)) {
						cout << "CHYBA: Kolo " << current_round
							<< ": chybna syntaxe loupeze " << i
							<< "teho hrace: \"" << moves[i] << "\"" << endl;
					} else {
						// shift defender as attacker (=i) thinks it has ID 0
						defender = (defender + i) % 4;
						kingdom_robbers[defender].push_back(i);
						kingdom[i].last_moves[0] = ACTION_ROBBERY_DONE; // to reset past robberies
					}
				}
				else {
					sscanf(moves[i], "l %u", &defender); // to read defender, though irrelevant
					defender = (defender + i) % 4;
				}
				if (debug)
					cout << i << ":loupez u " << defender << "  ";
				done = 1;
				break;
			}
			case ACTION_INFORMATION: {
				for (left=0;
					((left<INFORMATION_ACT_REPEATED) && (kingdom[i].last_moves[left] == ACTION_INFORMATION)); 
					left++);
				if (left == INFORMATION_ACT_REPEATED) {
					kingdom_act_information.push_back(i);
					kingdom[i].last_moves[0] = ACTION_INFORMATION_DONE; // to reset past information/spy actions
				}
				if (debug)
                            		cout << i << ":informace  ";
                                done = 1;
                                break;
                        }
			case '\0': { // no response provided => client timeouted
				if (debug)
                                        cout << i << ((is_kingdom_alive(i))?(":TIMEOUT    "):(":----       "));
				done = 1;
				break;
			}
		}
		if (moves[i][0] == ACTION_ATTACK) {
			done = 1;

			attack.attacker = i;
			// decode defender and # of attacking soldiers
			if (EOF == sscanf(moves[i], "u %u %u", &defender, &attack.soldiers)) {
				cout << "CHYBA: Kolo " << current_round
					<< ": chybna syntaxe utoku " << i
					<< "teho hrace: \"" << moves[i] << "\"" << endl;
			} else {
				// shift defender as attacker (=i) thinks it has ID 0
				defender = (defender + i) % 4;

				// check if the attacker has enough soldiers
				if (attack.soldiers > kingdom[i].soldiers)
					cout << "CHYBA:" << i << "ty hrac chce utocit s "
						<< attack.soldiers << "vojaky ale ma jich jen "
						<< kingdom[i].soldiers << "." << endl;
				else {
					/* decrement defending soldiers of attacker 
					and add the attack for future evaluation */
					kingdom[i].soldiers -= attack.soldiers;
					kingdom_attackers[defender].push_back(attack);
				}
			}
			if (debug)
				cout << i << ": u na " << defender << " " << attack.soldiers << " vojaky    ";
		}
		if (done == 0) {
			if (kingdom[i].land == 0) {
				if (debug)
					cout << i << ": ---         ";
			}
			else
				if (debug)
					cout << i << ": CHYBA neznama akce (" << moves[i] << ")   ";
		}
	}
	if (debug)
		cout << "\n";

	/* evaluate fights */
	int land_got[NUM_PLAYERS]; // track land won or lost by a player in battles; negative = loose, positive = win
	unsigned defence;
	unsigned sum_attack;
	list<attack_t>::iterator it;
	unsigned attacker_lost;
	unsigned dlost_land, dlost_peasants, dlost_army;
	float ratio;
	string attack_list;
	unsigned land_to_move, lost_land;

	for (i = 0; i < NUM_PLAYERS; i++) {
		land_got[i] = 0;
	}

	for (i = 0; i < NUM_PLAYERS; i++)
		if (!kingdom_attackers[i].empty()) {
			if (debug)
				cout << "	utok na  " << i << ": (utocnici:" ;
			//somebody attacks to this kingdom
			defence = (int) (DEFENCE_MULTIPLIER * kingdom[i].soldiers * 
					(int)(kingdom[i].arms / ARMS_LEVEL_UP));

			// compute strength of all attackerer
			sum_attack = 0;
			for (it = kingdom_attackers[i].begin();
				it != kingdom_attackers[i].end();
				it++) {
					sum_attack += it->soldiers * (int)(kingdom[(it->attacker)].arms
						/ ARMS_LEVEL_UP);
					if (debug)
						cout << it->attacker << " "; 
			}
			if (debug)
				cout << ") (obranne cislo: "<< defence << ", utocne cislo: " << sum_attack << "), ";

			if (sum_attack > defence) {
				// attack i/s successfull
				// how much land and potentially peasants the defender is loosing
				dlost_land = MIN(sum_attack-defence, kingdom[i].land);
				dlost_army = kingdom[i].soldiers;
				dlost_peasants = MIN(sum_attack-defence, kingdom[i].peasants);


				/* each attacker gets its relative portion 
				of lost_land_f (relative to its attack strength) 
				- rounded down for the sake of simplicity
				also the attacker looses some soldiers */
				lost_land = 0;
				for (it = kingdom_attackers[i].begin();
					it != kingdom_attackers[i].end(); it++) {

					/* count ratio of attack power */
					ratio = (float) (it->soldiers * (int)(kingdom[(it->attacker)].arms
							/ ARMS_LEVEL_UP))/sum_attack;

					/* move proportion of land */
					land_to_move = (int) (dlost_land  * ratio);
					land_got[i] -= land_to_move;
					land_got[it->attacker] += land_to_move;
					lost_land += land_to_move;

					/* move proportion of attack army */
					attacker_lost = (int) (defence *ratio /
						(int) (kingdom[(it->attacker)].arms / ARMS_LEVEL_UP));
					it->soldiers -= attacker_lost;
					attack_list = get_relative_attack_list(&(kingdom_attackers[i]), it->attacker);

					if (debug)
						cout << " " << it->attacker << ": zisk(zeme:" << (int) (dlost_land  * ratio) 
							<< ", armada:-" << attacker_lost << ") ";

					/* edit attackers log file */
					fprintf(fd_attack[it->attacker],
						"cil=%d\nutocnici=%s\n"\
						"ztraty_ja_vojaci=%d\nztraty_cile_vojaci=%d\n"\
						"ztraty_cile_rolnici=%d\nzisk_ja_uzemi=%d\n",
						(4 + i - it->attacker) % 4, attack_list.c_str(),
						attacker_lost, kingdom[i].soldiers,
						dlost_peasants, land_to_move);
				}
				if (debug) {
					cout << "obrance: ztraty(zeme:" << dlost_land
						<< ", armada:" << dlost_army
						<< ", rolnici:" << dlost_peasants << ")"<< endl;
				}
				kingdom[i].peasants -= dlost_peasants;

				// kill all defending soldiers and also peasants, write to defender's output file
				string attack_list = get_relative_attack_list(&(kingdom_attackers[i]), i);
				fprintf(fd_defence[i],
					"utocnici=%s\nztraty_utoku_vojaci=%d\n"\
					"ztraty_ja_vojaci=%d\nztraty_ja_rolnici=%d\n"\
					"ztraty_ja_uzemi=%d\n",
					attack_list.c_str(), attacker_lost,
					kingdom[i].soldiers, dlost_peasants, lost_land);
				kingdom[i].soldiers = 0;
			} else { // attack is not successfull

				if (defence == 0) {
					dlost_army = 0;
				} else {
					dlost_army = (sum_attack * kingdom[i].soldiers)/ defence;
				}
				attacker_lost = 0;
				for (it = kingdom_attackers[i].begin();
					it != kingdom_attackers[i].end(); it++) {

					attacker_lost += it->soldiers;
					attack_list = get_relative_attack_list(&(kingdom_attackers[i]), it->attacker);
					if (debug)
						cout << " " << it->attacker << ": zisk(zeme:0 , armada:-" << it->soldiers << " ) ";

					fprintf(fd_attack[it->attacker],
						"cil=%d\nutocnici=%s\n"\
						"ztraty_ja_vojaci=%d\nztraty_cile_vojaci=%d\n"\
						"ztraty_cile_rolnici=%d\nzisk_ja_uzemi=%d\n",
						(4 + i - it->attacker) % 4, attack_list.c_str(),
						it->soldiers, dlost_army, 0, 0);
					it->soldiers = 0;
				}

				// kill relative portion of defending soldiers
				attack_list = get_relative_attack_list(&(kingdom_attackers[i]), i);
				kingdom[i].soldiers -= dlost_army;

				if (debug)
					cout << "obrance: ztraty(zeme: 0, armada:"
						<< dlost_army << ", rolnici:0 )"<< endl;

				fprintf(fd_defence[i],
					"utocnici=%s\nztraty_utoku_vojaci=%d\n"\
					"ztraty_ja_vojaci=%d\nztraty_ja_rolnici=%d\n"\
					"ztraty_ja_uzemi=%d\n",
					attack_list.c_str(), attacker_lost,
					dlost_army, 0, 0);
		}
	}

	/* evaluate robberies */
	list<int>::iterator iter;
        unsigned arms_robbery_gain[NUM_PLAYERS];
        unsigned arms_robbery_loose[NUM_PLAYERS];
	unsigned arms_robbery;
        unsigned farming_robbery_gain[NUM_PLAYERS];
        unsigned farming_robbery_loose[NUM_PLAYERS];
	unsigned farming_robbery;
	string robbery_attackers[NUM_PLAYERS];


	for (i = 0; i < NUM_PLAYERS; i++) {
		arms_robbery_gain[i]=0;
		arms_robbery_loose[i]=0;
		farming_robbery_gain[i]=0;
		farming_robbery_loose[i]=0;
		robbery_attackers[i]="";
	}
	for (i = 0; i < NUM_PLAYERS; i++) {
		for (iter = kingdom_robbers[i].begin();
			iter != kingdom_robbers[i].end();
			iter++)
		{
			fprintf(fd_robbery_attack[*iter], "cil=%d\n", i);
			robbery_attackers[i] = robbery_attackers[i] + (char)((*iter)+48) + ",";
			if (debug)
				cout << "        loupez u " << i << ": (utocnici:" << *iter <<" ) ";
			/* evaluate robbery of arms */
			if (kingdom[i].arms > kingdom[*iter].arms)
			{
				arms_robbery = MIN(kingdom[i].arms,2);
				arms_robbery_loose[i] += arms_robbery;
				arms_robbery_gain[*iter] += arms_robbery;
				fprintf(fd_robbery_attack[*iter], "zisk_ja_zbrojeni=%d\nztraty_cile_zbrojeni=%d\n", arms_robbery, arms_robbery);
				if (debug)
					cout << "utocnik: zisk(vs:" << arms_robbery;
			}
			else
			{
				arms_robbery = MIN(kingdom[i].arms,1);
				arms_robbery_loose[i] += arms_robbery;
				fprintf(fd_robbery_attack[*iter], "zisk_ja_zbrojeni=0\nztraty_cile_zbrojeni=%d\n", arms_robbery);
				if (debug)
					cout << "utocnik: zisk(vs:0";
			}

			/* evaluate robbery of farming tech */
			if (kingdom[i].farming > kingdom[*iter].farming) {
				farming_robbery = MIN(kingdom[i].farming,2);
	                        farming_robbery_loose[i] += farming_robbery;
                                farming_robbery_gain[*iter] += farming_robbery;
				fprintf(fd_robbery_attack[*iter], "zisk_ja_farmareni=%d\nztraty_cile_farmareni=%d\n", farming_robbery, farming_robbery);
				if (debug)
					cout << ", fs:" << farming_robbery;
                        } else {
				farming_robbery = MIN(kingdom[i].farming,1);
                                farming_robbery_loose[i] += farming_robbery;
				fprintf(fd_robbery_attack[*iter], "zisk_ja_farmareni_ukradeno=0\nztraty_cile_farmareni=%d\n", farming_robbery);
				if (debug)
					cout << ", fs:0";
                        }
			/* rest print out */
			if (debug) {
				cout << ") obrance: ztrata(vs:" << arms_robbery << ", fs:" << farming_robbery << ")" << endl;
			}
		}
	}

	/* update skills and write to loupez_obrana.txt files */
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		if (robbery_attackers[i].length() > 0)
		{
			robbery_attackers[i].erase(robbery_attackers[i].length()-1);
			fprintf(fd_robbery_defence[i], 
				"utocnici=%s\nztraty_ja_zbrojeni=%d\nztraty_ja_farmareni=%d\n", 
				robbery_attackers[i].c_str(), MIN(kingdom[i].arms,arms_robbery_loose[i]), 
				MIN(kingdom[i].farming,farming_robbery_loose[i]));
		}
		kingdom[i].arms = kingdom[i].arms + arms_robbery_gain[i] - MIN(kingdom[i].arms,arms_robbery_loose[i]);
		kingdom[i].farming = kingdom[i].farming + farming_robbery_gain[i] - MIN(kingdom[i].farming,farming_robbery_loose[i]);
	}

	/* evaluate information / spying */
	for (iter=kingdom_act_information.begin();
		iter != kingdom_act_information.end();
		iter++)
	{
		for (i = 1; i < NUM_PLAYERS; i++)
			if (kingdom[((*iter)+i)%NUM_PLAYERS].secret_services < kingdom[*iter].secret_services)
			{
				string kingdom_details = get_kingdom_details(((*iter)+i)%NUM_PLAYERS);
				fprintf(fd_information[*iter],"%d: %s\n", i, kingdom_details.c_str());
				if (debug)
					cout << "       kralovstvi " << *iter
						<< " ziskalo informace o stavu kralovstvi "
						<< ((*iter)+i)%NUM_PLAYERS << endl;
			}
	}

	/* update kingdoms' land, re-calculate dead_kingdoms */
	dead_kingdoms = 0;
	for (i = 0; i < NUM_PLAYERS; i++) {
	kingdom[i].land += land_got[i];
		if (kingdom[i].land == 0) {
			dead_kingdoms++;
			if ((kingdom[i].arms+kingdom[i].farming+kingdom[i].secret_services) != 0) {
				if (debug) {
					cout << "		KRALOVSTVI " << i << " ZANIKLO" << endl;
				}
				kingdom[i].soldiers = 0;
				kingdom[i].arms = 0;
				kingdom[i].peasants = 0;
				kingdom[i].farming = 0;
				kingdom[i].food = 0;
				kingdom[i].secret_services = 0;
			}
		}
	}

	/* move soldiers back from battles */
	for (i = 0; i < NUM_PLAYERS; i++) {
		for (it = kingdom_attackers[i].begin(); it != kingdom_attackers[i].end(); it++)
			kingdom[it->attacker].soldiers += it->soldiers;
	}

	/* feed soldiers and peasants */
	for (i = 0; i < NUM_PLAYERS; i++) {
		// first feed peasants
		if (kingdom[i].food >= kingdom[i].peasants)
			kingdom[i].food -= kingdom[i].peasants;
		else {
			left = kingdom[i].peasants - kingdom[i].food;
			kingdom[i].peasants -= left;
			kingdom[i].food = 0;
			if (debug) {
				cout << "	ztraty:   kralovstvi " << i << "nema dost potravy, "
					<< left << " rolniku umrelo" << endl;
			}
		}
		// then feed soldiers
		if (kingdom[i].food >= kingdom[i].soldiers) {
			kingdom[i].food -= kingdom[i].soldiers;
		} else {
			left = kingdom[i].soldiers - kingdom[i].food;
			kingdom[i].soldiers -= left;
			kingdom[i].food = 0;
			if (debug) {
				cout << "	ztraty:   kralovstvi " << i << "nema dost potravy, "
					<< left << " vojaku umrelo" << endl;
			}
		}
	}

	for (i = 0; i < NUM_PLAYERS; i++ ) {
		fclose(fd_attack[i]);
		fclose(fd_defence[i]);
        	fclose(fd_information[i]);
        	fclose(fd_robbery_attack[i]);
        	fclose(fd_robbery_defence[i]);
	}

	if (debug) {
		for (i = 0; i < NUM_PLAYERS; i++ ) {
			if (kingdom[i].land != 0){
				/* live */
				cout << "	po akci  : " << i << ": uzemi:" << kingdom[i].land
					<< ", zasoby:" << kingdom[i].food
					<< ", vojaci:" << kingdom[i].soldiers << ", rolnici:" 
					<< kingdom[i].peasants << ", zbrojeni:" << kingdom[i].arms 
					<< ", farmareni:" << kingdom[i].farming << ", tajne sluzby:"
					<< kingdom[i].secret_services << endl;
			} else {
				/* dead */
				cout << "	po akci  : ----" << endl;
			}
		}
	}

    /* update whether the game hasn't just finished */
    increase_current_round();
    _game_finished=((current_round >= total_rounds) || (dead_kingdoms == NUM_PLAYERS - 1));

    /* write to disk the new playfield */
    if (write_to_disk)
	write_playfield_to_disk("a");
    return 0;
}

void PlayField::get_kingdoms_lands(int lands[])
{
	unsigned i;

	for (i = 0; i < NUM_PLAYERS; i++) {
		lands[i] = kingdom[i].land;
	}
}

string PlayField::get_kingdom_details(unsigned player)
{
	char buf[100];

	sprintf (buf,"%u %u %u %u %u %u %u", kingdom[player].land,
		kingdom[player].soldiers, kingdom[player].peasants,
		kingdom[player].arms, kingdom[player].farming,
		kingdom[player].food, kingdom[player].secret_services);
	return (string)buf;
}

void PlayField::update_last_moves(int player, char move)
{
	for (unsigned position=MAX_ACT_HISTORY-1; position > 0; position--)
		kingdom[player].last_moves[position]=kingdom[player].last_moves[position-1];
	kingdom[player].last_moves[0]=move;
}
