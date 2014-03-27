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
#include <sstream>
#include <string.h>

using namespace std;

PlayField::PlayField(string _fields_dir, string _filename)
{
  unsigned i,j;
  FILE *fd;
  unsigned lines_count;
  town_t town;
  shop_t shop;
  response_t response;

  _game_finished = false;
  fields_dir = _fields_dir;
  filename = _filename;
  current_round = 0;
  debug = false;

  /* setup some players stuff */
  for (i=0; i<NUM_PLAYERS; i++)
  {
    players[i].last_response = "";
    players[i].money = 0;
    players[i].shops.clear();
  }

  fd = fopen(filename.c_str(),"r");
  if (!fd) {
    fprintf(stderr, "Nelze otevrit soubor s hracim planem: %s\n", filename.c_str());
    return;
  }

  fscanf(fd, "Kolo: %u\n", &current_round);
  fscanf(fd, "Rozmer: %u\n", &field_size);
  fscanf(fd, "Mesta: %u\n", &lines_count);
  for (i=0; i<lines_count; i++)
  {
    fscanf(fd, "%u %u %u\n", &town.x, &town.y, &town.residents);
    towns.push_back(town);
  }
  
  for (i=0; i<NUM_PLAYERS; i++)
  {
    fscanf(fd, "Hrac%u: %f euro %u obchodu\n", &j, &players[i].money, &lines_count);
    for (j=0; j<lines_count; j++)
    {
      fscanf(fd, "%u %u %f %f\n", &shop.x, &shop.y, &shop.price, &shop.profit);
      players[i].shops.push_back(shop);
    }
  }

  for (i=0; i<NUM_PLAYERS; i++)
  {
    fscanf(fd, "Hrac%u_odpoved: \"%s\"\n", &j, response);
    players[i].last_response = response;
    players[i].last_response = players[i].last_response.substr(0,players[i].last_response.length()-1); // fscanf precte i posledni \" do promenne 'response'
  }
  fclose(fd);

  print_field();
}


PlayField::~PlayField()
{
  unsigned i;
  for (i=0; i<NUM_PLAYERS; i++)
    players[i].shops.clear();
}


void PlayField::increase_current_round()
{
  unsigned i;
  unsigned players_with_money = 0;

  current_round++;
  _game_finished |= (current_round == MAX_ROUNDS);
  for (i=0; i<NUM_PLAYERS; i++)
    if (players[i].money >=0)
      players_with_money++;
  _game_finished |= (players_with_money<2);
}


void PlayField::play_one_round(response_t* moves)
{
  unsigned i,j;
  list<shop_t>::iterator shop_iter;
  list<town_t>::iterator town_iter;
  shop_weight_ptr_t *shop_weights;
  unsigned total_shops = 0;
  float weight_sum, aux_weight, rnd;
  list<string> response_elems;
  list<string>::iterator response_iter;
  list<new_shop_t> new_shops;
  list<new_shop_t>::iterator new_shops_iter;
  new_shop_t new_shop;
  unsigned shop_on_place[field_size*field_size];
  bool move_ok[NUM_PLAYERS];
  bool open_shop = false;

  for (i=0; i<NUM_PLAYERS; i++)
  {
    move_ok[i] = true;
    stringstream ss(moves[i]);
    string item;
    while (std::getline(ss, item, CMD_SEPARATOR)) {
        while (item[0]==' ')
            item.erase(0,1);
        response_elems.push_back(item);
    }
    response_iter=response_elems.begin();
    /* set prices of shops */
    for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
    {
      if ((response_iter==response_elems.end()) ||
          (sscanf(response_iter->c_str(),"%f",&(shop_iter->price))==0))
      {
        move_ok[i]=false;
        response_iter=response_elems.end();
        cerr << "Hrac " << (i+1) << ": prilis malo cen v odpovedi \""
             << moves[i] << "\" (obchodu ma " << players[i].shops.size()
             << ")" << endl;
        break;
      }
      ++response_iter;
    }
    /* destroy shops */
    while (response_iter!=response_elems.end()
            &&(sscanf(response_iter->c_str(),DEL_SHOP_CMD_FORMAT.c_str(), &new_shop.x, &new_shop.y)==2))
    {
      for (shop_iter=players[i].shops.begin();
          (shop_iter!=players[i].shops.end())
            &&((shop_iter->x!=new_shop.x)||(shop_iter->y!=new_shop.y));
          ++shop_iter)
        ;
      if (shop_iter!=players[i].shops.end()) {
        players[i].shops.erase(shop_iter);
      }
      else
      {
        move_ok[i]=false;
        response_iter=response_elems.end();
        cerr << "Hrac " << (i+1) << ": pokus o smazani neexistujiciho obchodu ["
             << new_shop.x << "," << new_shop.y << "]" << endl;
        break;
      }
      ++response_iter;
    }
    /* new shops */
    while (response_iter!=response_elems.end()
            &&(sscanf(response_iter->c_str(),NEW_SHOP_CMD_FORMAT.c_str(), &new_shop.x, &new_shop.y)==2))
    {
      players[i].money -= SHOP_BUILD_PRICE;
      new_shop.player=i;
      new_shops.push_back(new_shop);
      ++response_iter;
    }
    if ((response_iter!=response_elems.end()))
    {
      cerr << "Hrac " << (i+1) << ": neznamy prikaz " << response_iter->c_str() << endl;
      move_ok[i]=false;
    }
    if (!move_ok[i])
      for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
        shop_iter->price = (float)INVALID_PRICE;
    response_elems.clear();
  }
  /* reset profits of shops, set player's response */
  for (i=0; i<NUM_PLAYERS; i++)
  {
    total_shops += players[i].shops.size();
    for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
      open_shop |= (shop_iter->price <= (float)MAX_PRICE);
  }
  if (total_shops>0)
    shop_weights = new shop_weight_ptr_t[total_shops];
  j=0;
  for (i=0; i<NUM_PLAYERS; i++)
  {
    players[i].last_response = moves[i];
    for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
    {
      shop_iter->profit = -SHOP_MAINTENANCE;
      shop_weights[j++].shop_iter=shop_iter;
    }
  }
  /* decide where each inhabitant will go */
  for (town_iter=towns.begin(); (town_iter!=towns.end())&&(open_shop); ++town_iter)
  {
    weight_sum=0;
    if (debug)
      cout << "Town: [" << town_iter->x << "," << town_iter->y << "]:" << endl;
    for (i=0; i<total_shops; i++)
    {
      if (shop_weights[i].shop_iter->price <= (float)MAX_PRICE)
        shop_weights[i].weight = (1.0 / (1 + abs((int)(shop_weights[i].shop_iter->x - town_iter->x))+abs((int)(shop_weights[i].shop_iter->y - town_iter->y))))
                                 / (1 + shop_weights[i].shop_iter->price*shop_weights[i].shop_iter->price*shop_weights[i].shop_iter->price);
      else
        shop_weights[i].weight = 0;
      weight_sum += shop_weights[i].weight;
      if (debug)
        cout << "  shop at [" << shop_weights[i].shop_iter->x << "," << shop_weights[i].shop_iter->y << "] has weight " << shop_weights[i].weight << ", weight_limit=" << weight_sum <<endl;
    }
    /* for each town resident, throw weighted dice where he will buy */
    for (j=0; j<town_iter->residents; ++j)
    {
      rnd = weight_sum * rand() / RAND_MAX;
      if (debug)
        cout << "--- dice roll: " << rnd << endl;
      aux_weight = 0;
      i=0;
      do
      {
        aux_weight += shop_weights[i++].weight;
      }
      while (aux_weight < rnd);
      if (debug)
        cout << "shop at [" << shop_weights[i-1].shop_iter->x << ","
             << shop_weights[i-1].shop_iter->y << "] gets profit: "
             << shop_weights[i-1].shop_iter->price - GOODS_COST - 1.0*shop_weights[i-1].shop_iter->price*shop_weights[i-1].shop_iter->price/MAX_PRICE
             << " from town [" << town_iter->x << "," << town_iter->y << "]"
             << endl;
      shop_weights[i-1].shop_iter->profit += (shop_weights[i-1].shop_iter->price - GOODS_COST);
    }
  }
  /* add money from shops profit to player's money */
  for (i=0; i<NUM_PLAYERS; i++)
    for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
      players[i].money += shop_iter->profit;

  /* add new shops */
  /* count where all shops would be built if there is no conflict */
  for (i=0; i<field_size*field_size; i++)
    shop_on_place[i]=0;
  for (i=0; i<NUM_PLAYERS; i++)
    for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
      shop_on_place[shop_iter->y*field_size+shop_iter->x]++;
  for (new_shops_iter=new_shops.begin(); new_shops_iter!=new_shops.end(); ++new_shops_iter)
    shop_on_place[new_shops_iter->y*field_size+new_shops_iter->x]++;
  
  for (new_shops_iter=new_shops.begin(); new_shops_iter!=new_shops.end(); ++new_shops_iter)
    if (shop_on_place[new_shops_iter->y*field_size+new_shops_iter->x]==1)
      add_shop(new_shops_iter);
    else
      cerr << "Hrac " << new_shops_iter->player+1 << ": konflikt s umistenim obchodu ["
           << new_shops_iter->x << "," << new_shops_iter->y << "]" << endl;
  new_shops.clear();
  increase_current_round();
  print_field();
}

void PlayField::add_shop(list<new_shop_t>::iterator new_shop)
{
  shop_t shop;
  list<shop_t>::iterator shop_iter = players[new_shop->player].shops.begin();
  while ((shop_iter!=players[new_shop->player].shops.end())
          &&((shop_iter->y < new_shop->y)
            ||((shop_iter->y == new_shop->y)&&(shop_iter->x < new_shop->x))))
    ++shop_iter;

  shop.x=new_shop->x;
  shop.y=new_shop->y;
  shop.price=INVALID_PRICE;
  shop.profit=0;

  if (shop_iter!=players[new_shop->player].shops.end())
    players[new_shop->player].shops.insert(shop_iter,shop);
  else
    players[new_shop->player].shops.push_back(shop);
}

void PlayField::print_field()
{
  unsigned i;
  list<town_t>::iterator town_iter;
  list<shop_t>::iterator shop_iter;
  string new_filename = filename;
  char s[20], s2[20];
  FILE *fd;
  size_t last_dir_delim = filename.find("/");

  if (last_dir_delim != string::npos)
    new_filename = filename.substr(last_dir_delim+1);

  sprintf(s, ".%04d", current_round-1);
  sprintf(s2, ".%04d", current_round);
  if (new_filename.rfind(s) == new_filename.length()-strlen(s)) {
    new_filename.replace(new_filename.length()-strlen(s), strlen(s), "");
  }
  if (new_filename.rfind(s2) != new_filename.length()-strlen(s2)) {
    new_filename += s2;
  }
  filename=new_filename; /* nutne kvuli zapisu na disk po odehrani dalsiho kola */
  new_filename = fields_dir + "/" + new_filename;

  fd = fopen(new_filename.c_str(),"w+");
  if (fd == NULL) {
    cerr << "soubor " << new_filename.c_str() << " nelze vytvorit" << endl;
    return;
  }

  fprintf(fd, "Kolo: %u\n", current_round);
  fprintf(fd, "Rozmer: %u\n", field_size);
  fprintf(fd, "Mesta: %d\n", (int)towns.size());
  for (town_iter=towns.begin(); town_iter!=towns.end(); ++town_iter)
    fprintf(fd, "%u %u %u\n", town_iter->x, town_iter->y, town_iter->residents);

  for (i=0; i<NUM_PLAYERS; i++)
  {
    fprintf(fd, "Hrac%u: %.2f euro %d obchodu\n", i+1, players[i].money, (int)players[i].shops.size());
    for (shop_iter=players[i].shops.begin(); shop_iter!=players[i].shops.end(); ++shop_iter)
      fprintf(fd, "%u %u %.2f %.2f\n", shop_iter->x, shop_iter->y, shop_iter->price, shop_iter->profit);
  }

  for (i=0; i<NUM_PLAYERS; i++)
    fprintf(fd, "Hrac%u_odpoved: \"%s\"\n", i+1, players[i].last_response.c_str());
  fclose(fd);
}

void PlayField::get_points(int points[])
{
  unsigned i;
  for (i=0; i<NUM_PLAYERS; i++)
  {
    points[i] = players[i].money;
  }
}
