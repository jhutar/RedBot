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
  ',-:2,#:0,-:2,,,,,',
  ',,,,,,,\:0,',
  'C:1,,,,,,,,F:1',
  ',\:1,,,,,,,',
  'C:3;D:3,|:1;-:3,D:1,,,,E:1,,E:3;F:3'
]
[[[0, 6],         [2, 6],         [4, 6],         [6, 4],         [6, 6],         [6, 8],                 [8, 2], [8, 8]]]
[[[0, 6], [1, 6], [2, 6], [3, 6], [4, 6], [5, 6], [6, 4], [6, 5], [6, 6], [6, 7], [6, 8], [7, 3], [7, 8], [8, 2], [8, 8]]]
def test_start_init():
  strat = Strat('strat0/strat0.sh', 0, '0,5,ABC,0')
  assert strat.id == 0
  assert strat.points == 0
  assert strat.stones == 5
  assert strat.potion == ['A', 'B', 'C']
  assert strat.potion_done == False
  strat = Strat('strat1/strat1.sh', 1, '100,0,XYZ,1')
  assert strat.id == 1
  assert strat.points == 100
  assert strat.stones == 0
  assert strat.potion == ['X', 'Y', 'Z']
  assert strat.potion_done == True

def test_start_want_to_use():
  plan = Plan(plan_list)
  strat = Strat('strat0/strat0.sh', 0, '0,5,HFG,0')
  strat.set_cookbook({})
  expected = {
    'H': [[6, 8], [8, 8]],
    'F': [[8, 2]],
    'G': [[8, 8]]
  }
  returned = strat.want_to_use(plan)
  assert returned == expected

def test_plan_init():
  plan = Plan(plan_list)
  assert plan[0][0] == {'C': 3, 'D': 3}
  assert plan[1][8] == {'-': 1}
  assert plan[3][0] == {}
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
  assert plan.columns_count == 9
  assert plan.rows_count == 9
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

def test_plan_paths_for_strat():
  plan = Plan(plan_list)
  # Test "list_paths_for_strat"
  expected = [
    [7,3],
    [6,5],
    [1,6],
    [3,6],
    [5,6],
    [6,7],
    [7,8],
  ]
  expected.sort()
  returned = plan.list_paths_for_strat(0)
  returned.sort()
  assert expected == returned
  # Test "get_connected_cells"
  expected = [[0,0], [1,0], [2,0], [1,1], [0,2]]
  expected.sort()
  returned = plan.get_connected_cells([1,0], [])
  returned.sort()
  assert expected == returned
  # Test "get_continuing_cells"
  expected = [[1,1]]
  expected.sort()
  returned = plan.get_connecting_edges([0,2])
  returned.sort()
  assert expected == returned
  # Test "get_continuing_cells"
  expected = [[0,0], [1,1], [2,0]]
  expected.sort()
  returned = plan.get_continuing_cells([1,0])
  returned.sort()
  assert expected == returned
  # Test "get_paths_for_strat"
  expected = []
  expected.append([
    [0,6],
    [1,6],
    [2,6],
    [3,6],
    [4,6],
    [5,6],
    [6,4],
    [6,5],
    [6,6],
    [6,7],
    [6,8],
    [7,3],
    [7,8],
    [8,2],
    [8,8]
  ])
  expected[0].sort()
  returned = plan.get_paths_for_strat(0)
  returned[0].sort()
  assert expected == returned
  # Test "get_paths_for_strat" with stone
  expected = []
  expected.append([
    [0,4],
    [1,4],
    [2,4],
  ])
  expected.append([
    [2,4],
    [3,4],
    [4,4],
  ])
  expected[0].sort()
  returned = plan.get_paths_for_strat(2)
  returned[0].sort()
  assert expected == returned

def test_game_init():
  game = Game('plan.dat')

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
  test_plan_paths_for_strat()
  test_game_init()

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
