#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from strat import Strat
from plan import Plan
from game import Game

plan_list = [
  'A:3;B:3,-:1,A:1,,,,H:1,-:0,G:3;H:3',
  ',,,,,,|:0,,',
  'B:1,-:0,,-:0,,-:0,,,G:1',
  ',,,,,,|:0,,',
  ',,,,,,,,',
  ',,,,,,,\:0,',
  'C:1,,,,,,,,F:1',
  ',,,,,,,,',
  'C:3;D:3,,D:1,,,,E:1,,E:3;F:3'
]

def test_start_init():
  strat = Strat('strat0/strat0.sh', '0,0,5,ABC')
  assert strat.id == 0
  assert strat.points == 0
  assert strat.stones == 5
  assert strat.potion == ['A', 'B', 'C']
  strat = Strat('strat1/strat1.sh', '1,100,0,XYZ')
  assert strat.id == 1
  assert strat.points == 100
  assert strat.stones == 0
  assert strat.potion == ['X', 'Y', 'Z']

def test_start_want_to_use():
  plan = Plan(plan_list)
  strat = Strat('strat0/strat0.sh', '0,0,5,HFG')
  expected = {
    6: {
      8: {'H': 1},
    },
    8: {
      2: {'F': 1},
      8: {'G': 3, 'H': 3},
    }
  }
  assert strat.want_to_use(plan) == expected

def test_plan_init():
  plan = Plan(plan_list)
  assert plan[0][0] == {'C': 3, 'D': 3}
  assert plan[1][8] == {'-': 1}
  assert plan[1][0] == {}
  ##print plan

def test_plan_stones():
  plan = Plan(plan_list)
  ###print plan
  plan[0][0] = {'#': 3}
  assert plan[0][0] == {'#': 3}
  plan[0][0] = {'#': 0}   # stones can not be replaced
  assert plan[0][0] == {'#': 3}
  plan[1][8] = {'#': 3}
  assert plan[1][8] == {'#': 3}
  plan[1][0] = {'#': 3}
  assert plan[1][0] == {'#': 3}
  ###print plan

def test_plan_paths():
  plan = Plan(plan_list)
  ###print plan
  plan[0][0] = {'|': 3}
  assert plan[0][0] == {'C': 3, 'D': 3, '|': 3}
  plan[0][0] = {'/': 2}
  plan[0][0] = {'-': 1}
  plan[0][0] = {'\\': 0}
  assert plan[0][0] == {'C': 3, 'D': 3, '|': 3, '/': 2, '-': 1, '\\': 0}
  plan[2][2] = {'#': 3}
  plan[2][2] = {'|': 3}
  assert plan[2][2] == {'#': 3}
  plan[4][4] = {'|': 3}
  plan[4][4] = {'#': 3}
  assert plan[4][4] == {'#': 3}
  ###print plan

def test_plan_set_negative():
  plan = Plan(plan_list)
  for value in [{'.': 0}, {'#': 10}, {'|': -10}, {'.': 1000}]:
    try:
      plan[0][0] = value
    except Exception:
      pass
    else:
      assert 0 == 1

def test_plan_same_all_the_time():
  plan1 = Plan(plan_list)
  plan2 = Plan(plan_list)
  assert plan1 == plan2

def main():
  game = Game('plan.dat')

if __name__ == '__main__':
  ###main()
  test_start_init()
  test_start_want_to_use()
  test_plan_init()
  test_plan_stones()
  test_plan_paths()
  test_plan_set_negative()
  test_plan_same_all_the_time()

# I expect this should work like this:
#
# load all data
# execute all competing strategies with command line params: <strategy_id> <datafile_path> <potion>
# evaluate putting stones on the map
# evaluate extending routes
# count potions completed by each strategy in this round
#   for each strategy:
#     what ingredient do I have access to?
#     how many potions can I create?
#   compete for resources we have to compete for
# update (remove ingredients used for potions in this round...) and dump map
