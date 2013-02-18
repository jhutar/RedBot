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

#include <cstdio>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "OptionParser.h"
#include "PlayField.h"

using namespace std;

struct Options : OptionParser
{
  string strategy[NUM_PLAYERS];
  bool one_round;
  bool sudo_mode;
  bool debug;
  string battlefields_dir;

  Options()
    : OptionParser("Pouziti: ./redbot [OPTIONS] strategie1 strategie2 .. strategieN", "Spust redbot server"),
    one_round(false),
    sudo_mode(false),
    debug(false),
    battlefields_dir("game")
    {
      add("one_round,o", one_round, "odehrej jen jedno kolo");
      add("sudo_mode,s", sudo_mode, "sudo-mod - klienti pobezi pod separatnimi uzivateli");
      add("debug,d", debug, "debug mod");
      add("bfields,b", battlefields_dir, "adresar kam si server bude ukladat hraci plany");
    }

    bool checkStrategies()
    {
      int i;
      if (getArguments().size() != NUM_PLAYERS)
      {
        cerr << "Spatny pocet klientu, potrebuji "
             << NUM_PLAYERS << ", dostal jsem " << getArguments().size() << endl;
        return false;
      }
      else
      {
        for (i = 0; i < NUM_PLAYERS; i++)
          strategy[i] = getArguments()[i];
        return true;
      }
    }
};


/* create thread argument struct for thr_func() */
typedef struct _thread_data_t
{
  int tid;
  response_t response;
  string binary_to_exe;
} thread_data_t;

/* thread function */
void *thr_func(void *arg)
{
  thread_data_t *data;
  FILE* pipe;
  response_t buffer;

  /* request delivery of any pending cancellation request */
  pthread_testcancel();

  data = (thread_data_t *)arg;
  data->response[0] = '\0';
  /* open pipe to thread binary */
  pipe = popen(data->binary_to_exe.c_str(), "r");
  if (pipe == NULL)
  {
    fprintf(stderr, "pipe: error in popen %s\n", data->binary_to_exe.c_str());
  }
  while(!feof(pipe))
  {
    if ((fgets(buffer, 128, pipe) != NULL) &&
        (data->response[0] == '\0'))
    {
      strncpy(data->response,buffer, 128);
      data->response[127] = '\0';
    }
  }
  pclose(pipe);
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  /* parse command line options */
  Options options;
  string sudo_test;

  /* some thread related stuff */
  pthread_t thr[NUM_PLAYERS];
  thread_data_t thr_data[NUM_PLAYERS];
  int rc, tid;
  response_t response[NUM_PLAYERS];

  /* bot_dirs - what is working directory of each bot to where store files
   * with localized playingfields etc.; if bot's binary is ./binary, then
   * bot_dirs="./binary."
   */
  string bots_dir[NUM_PLAYERS];
  string bots_bin[NUM_PLAYERS];
  size_t last_slash_pos;

  unsigned time_elapsed;
  bool all_responded;
  string sudo_kill;
  unsigned points[NUM_PLAYERS];

  int i;
  char *c;
  FILE *fd;

  struct stat sb;

  /* parse command line options */
  if (!(options.parse(argc, argv)) || !(options.checkStrategies()))
    return EXIT_FAILURE;

  if (options.debug)
      cout << "Debug mod." << endl;

  /* for sudo-mode (bots run by different users), test if we can run bots
   * programs under proper users' accounts
   */
  if (options.sudo_mode) {
    for (i = 0; i < NUM_PLAYERS; i++) {
      sudo_test = "sudo -u " + run_bot_under_user[i]
        + " echo > /dev/null";
      if (system(sudo_test.c_str()) != 0) {
        cerr << "Uzivatel " << run_bot_under_user[i]
          << " nemuze byt spusten pod sudo. "
          << "Prosim bud umoznete sudo na daneho uzivatele "
          << "nebo nepouzivejte sudo mod."
          << endl;
        return EXIT_FAILURE;
      }
    }
  }

  for (tid = 0; tid < NUM_PLAYERS; ++tid)
  {

    fd = fopen(options.strategy[tid].c_str(), "r");
    if (!fd)
    {
      cerr << "Neexistuje strategie " << options.strategy[tid] << endl;
      return EXIT_FAILURE;
    }
    fclose(fd);

    last_slash_pos = options.strategy[tid].find_last_of('/');
    if ((last_slash_pos > 2) ||
      ((last_slash_pos == 1) && options.strategy[tid][0] != '.'))
    {
      // options.strategy[tid] is in form directory/binary
      bots_dir[tid]=options.strategy[tid].substr(0, last_slash_pos + 1);
      bots_bin[tid]=options.strategy[tid].substr(last_slash_pos + 1,
        options.strategy[tid].size() - 1);
    }
    else
    {
      // options.strategy[tid] is in form ./binary or only binary
      bots_dir[tid] = ".";
      bots_bin[tid] = options.strategy[tid];
    }

    for (i = 0; i < tid; ++i)
    {
      if (bots_dir[i] == bots_dir[tid])
      {
        cerr << "PROBLEM: Dve strategie "
          << options.strategy[i] << " a "
          << options.strategy[tid]
          << " by si prepsisovaly soubory s informacemi, "
          << "presunte je do separatnich adresaru."
          << endl;
        return EXIT_FAILURE;
      }
    }
  }

  /* test if directory for storing battlefields exists */
  if ((stat(options.battlefields_dir.c_str(), &sb) != 0) || (! S_ISDIR(sb.st_mode)))
  {
    cout << "Neexistuje adresar \"" << options.battlefields_dir
         << "\" kam ukladat hraci plany, vytvarim jej." << endl;
    if (mkdir(options.battlefields_dir.c_str(), 0777) != 0)
    {
      cerr << "Nepodarilo se vytvorit adresar \"" << options.battlefields_dir
           << "\" kam ukladat hraci plany!" << endl;
      return EXIT_FAILURE;
    }
  }

  /* load playing field */
  srand (time(NULL));
  PlayField playfield(bots_dir, options.battlefields_dir);
  if (!(playfield.is_playfield_generated()))
  {
    cerr << "Nepodarilo se mi umistit lode!!!" << endl;
    return EXIT_FAILURE;
  }
  playfield.set_debug(options.debug);

  do
  {
    cout << playfield.get_current_round() + 1 << ". kolo:";

    /* create & run threads */
    for (tid = 0; tid < NUM_PLAYERS; ++tid)
    {
      if (playfield.is_fleet_alive(tid))
      {
        thr_data[tid].tid = tid;
        /* binary_to_exe to contain:
          1) in sudo-mode something like
               "sudo -u botuser0 user0_bot/bot_binary <args>"
          2) in non-sudo mode (default) then: "user0_bot/bot_binary <args>"
         */
        thr_data[tid].binary_to_exe = "cd " + bots_dir[tid] + "; ./"
                                    + bots_bin[tid] + " ";
        if (options.sudo_mode)
        {
          thr_data[tid].binary_to_exe =
            "sudo -u " + run_bot_under_user[tid] + " "
            + thr_data[tid].binary_to_exe;
        }
        rc = pthread_create(&thr[tid], NULL, thr_func, &thr_data[tid]);
        if (rc) {
          cerr << "pthread_create se neprovedl, navratova hodnota: " << rc
               << endl;
          return EXIT_FAILURE;
        }
      }
    }

    /* sleep phase;
        check every CHECK_BOTS_INTERVAL if all bots responded;
        if so break the "sleep X seconds"
    */
    time_elapsed = CHECK_BOTS_INTERVAL;
    for (; time_elapsed <= ONE_ROUND_DELAY; time_elapsed += CHECK_BOTS_INTERVAL)
    {
      usleep(CHECK_BOTS_INTERVAL*1000);
      all_responded = true;

      for (tid = 0; tid < NUM_PLAYERS; ++tid)
      {
        if (playfield.is_fleet_alive(tid))
        {
          all_responded &= (thr_data[tid].response[0] != '\0');
        }
      }

      if (all_responded)
      {
        break;
      }
    }

    /* put bot responses to relevnant variable response[tid] */
    for (tid = 0; tid < NUM_PLAYERS; ++tid)
    {
      if (playfield.is_fleet_alive(tid))
      {
        pthread_cancel(thr[tid]);
        pthread_join(thr[tid], NULL);
        if (options.sudo_mode)
        {
          sudo_kill = "sudo -u " + run_bot_under_user[tid] + " kill -9 -1";
          system(sudo_kill.c_str());
        }

        /* save bot response to relevant structure */
        strncpy (response[tid],thr_data[tid].response, 128);
        response[tid][127] = '\0';

        /* only the first line of trategy entry is relevant */
        c = strchr(response[tid],'\n');
        if (c != NULL)
        {
          c[0] = '\0';
        }
      }
      else
      {
        response[tid][0] = '\0';
      }
      cout << "\t\"" << response[tid] << "\"";
    }
    cout << endl;

    if ((time_elapsed > ONE_ROUND_DELAY) && (options.debug))
    {
      cout << "       WARN: Nejaka strategie timeoutovala, "
           << "zadna odpoved behem "
           << ONE_ROUND_DELAY << "ms." << endl;
    }

    /* evaluate the round */
    playfield.play_one_round(response);
  }
  while ((!(playfield.game_finished())) && (!(options.one_round)));

  cout << "Hra skoncila, flotily maji bodu:";
  playfield.get_points(points);
  for (i = 0; i < NUM_PLAYERS; i++)
  {
    cout << ' ' << points[i];
  }
  cout << endl;

  return EXIT_SUCCESS;
}
