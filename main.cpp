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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <iostream>

#include "OptionParser.h"
#include "PlayField.h"
 
using namespace std;

struct Options : OptionParser
{
    string field_filename;
    string strategy[NUM_PLAYERS];
    bool one_round;
    bool sudo_mode;
    bool delete_old_playing_fields;

    Options()
        : OptionParser("Usage: redbot [OPTIONS] strategy1 strategy2 .. strategyN", "Run redbot server"),
          field_filename(FIELD_DEFAULT_FILENAME),
          one_round(false),
	  sudo_mode(false),
	  delete_old_playing_fields(false)
    {
        add("field,f", field_filename, "specify filename of the description of the playing field");
        add("one_round,o", one_round, "play just one round");
	add("sudo_mode,s", sudo_mode, "sudo-mode enabled. That causes bots will run under users specified by run_bot_under_user variable.");
	add("delete_old_fields,D", delete_old_playing_fields, "before running the current game, delete playing-field snapshots from previous game");
    }

    bool checkStrategies()
    {
        if (getArguments().size() != NUM_PLAYERS) {
            cerr << "Invalid number of player strategies provided, require " << NUM_PLAYERS << ", got " << getArguments().size() << endl;
            return false;
        } else {
            for (int i = 0; i < NUM_PLAYERS; i++)
                strategy[i] = getArguments()[i];
            return true;
        }
    }
};

 
/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
    int tid;
    char response;
    string binary_to_exe;
} thread_data_t;


/* thread function */
void *thr_func(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    data->response='\0';
    pthread_testcancel();
    FILE* pipe = popen(data->binary_to_exe.c_str(), "r");
    char buffer[128];
    while(!feof(pipe)) {
        if((fgets(buffer, 128, pipe) != NULL) && (data->response == '\0'))
            data->response = buffer[0];
    }
    pclose(pipe);
    pthread_exit(NULL);
}


int main(int argc, char **argv) {

    /* parse command line options */
    Options options;
    if ( !( (options.parse(argc, argv)) && (options.checkStrategies())) )
        return EXIT_FAILURE;

    /* for sudo-mode (bots run by different users), test if we can run bots programs under the proper users' accounts */
    if (options.sudo_mode) {
	for (unsigned i=0; i<NUM_PLAYERS; i++) {
            string sudo_test = "sudo -u " + run_bot_under_user[i] + " echo > /dev/null";
            if (system(sudo_test.c_str()) != 0) {
                cerr << "User " << run_bot_under_user[i] << " can not be sudo-ed for executing bots. " 
                     << "Please either allow sudo to that user or set run_bot_under_user variable in "
                     << "PlayField.h to proper user that can be sudo-ed." << endl;
                return EXIT_FAILURE;
	    }
        }
    }

    /* load playing field */
    PlayField playfield(options.field_filename);
    if (!(playfield.loaded_ok())) {
        return EXIT_FAILURE;
    }

    /* delete playing field snapshots from previous game */
    if (options.delete_old_playing_fields) {
	string filename;
	int i=0;
	char s[10];
	do {
	    i++;
	    sprintf(s, ".%04d", i);
	    filename=options.field_filename + s;
	}
	while (remove(filename.c_str()) == 0);
    }

    /* initialize pseudo-random generator */
    srand (time(NULL));

    /* some thread related stuff */
    pthread_t thr[NUM_PLAYERS];
    thread_data_t thr_data[NUM_PLAYERS];
    int rc, tid;
    char response[NUM_PLAYERS];

    do {
	cout << "------------------ Playing " << playfield.get_current_round()+1 << "th round ------------------" << endl;
        /* create & run threads */
        for (tid = 0; tid < NUM_PLAYERS; ++tid) 
	    if (playfield.is_worm_alive(tid)) {
                thr_data[tid].tid = tid;
   	        //binary_to_exe to contain: 1) in sudo-mode something like "sudo -u botuser0 user0_bot/bot_binary playing_field.txt 0"
	        //				2) in non-sudo mode (default) then: "user0_bot/bot_binary playing_field.txt 0"
                thr_data[tid].binary_to_exe = ((options.sudo_mode)?("sudo -u " + run_bot_under_user[tid] + " "):("")) 
		   			      + options.strategy[tid] + " " + playfield.get_current_filename() + " " 
					      + (char)(tid+48);
                if ((rc = pthread_create(&thr[tid], NULL, thr_func, &thr_data[tid]))) {
	  	    cerr << "pthread_create failed, return code: " << rc << endl;
                    return EXIT_FAILURE;
                }
            }

	/* sleep phase; check every CHECK_BOTS_INTERVAL if all bots responded; if so break the "sleep X seconds" */
	unsigned time_elapsed = CHECK_BOTS_INTERVAL;
	for (; time_elapsed <= ONE_ROUND_DELAY; time_elapsed += CHECK_BOTS_INTERVAL) {
	    usleep(CHECK_BOTS_INTERVAL*1000);
	    bool all_responded = true;
	    for (int tid = 0; tid < NUM_PLAYERS; ++tid)
		if (playfield.is_worm_alive(tid))
		    all_responded &= (thr_data[tid].response != '\0');
	    if (all_responded) {
		cout << "All bots responded within " << time_elapsed << " ms" << endl;
		break;
	    }
	}
	if (time_elapsed > ONE_ROUND_DELAY) {
	    cout << "Some bot timeouted, responded after " << ONE_ROUND_DELAY << "ms." << endl;
	}

	cout << "Bots responses:";
        for (int tid = 0; tid < NUM_PLAYERS; ++tid) 
	    if (playfield.is_worm_alive(tid)) {
                pthread_cancel(thr[tid]);
   	        pthread_join(thr[tid], NULL);
		if (options.sudo_mode) {
  		    string sudo_kill = "sudo -u " + run_bot_under_user[tid] + " kill -9 -1";
               	    system(sudo_kill.c_str());
		}
	        cout << ' ' << ((thr_data[tid].response=='\0')?('T'):(thr_data[tid].response));
	        response[tid] = thr_data[tid].response;
            }
	    else {
		cout << " D";
		response[tid] = ' ';
	    }
	cout << endl;
	
	playfield.play_one_round(response);
    }
    while ((!(playfield.game_finished())) and (!(options.one_round)));

    int points[NUM_PLAYERS];
    playfield.get_worms_points(points);
    cout << "Game finished, bots have points:";
    for (unsigned i = 0; i < NUM_PLAYERS; i++)
	cout << ' ' << points[i];
    cout << endl;

    return EXIT_SUCCESS;
}
