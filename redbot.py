#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import sys

from strat import Strat
from plan import Plan
from game import Game

plan_list = [
  'A:3;B:3,-:1,A:1,,,,H:1,-:0,G:3;H:3',
  ',,,,,,|:0,,',
  'B:1,-:0,,-:0,,-:0,,,G:1',
  '#:0,,,,,,|:0,,',
  ',-:2,#:0,-:2,,,,,',
  ',,,,,,,\:0,',
  'C:1,,,,,,,,F:1',
  ',\:1,,,,,,,',
  'C:3;D:3,|:1;-:3,D:1,,,,E:1,,E:3;F:3'
]
[[[0, 6],         [2, 6],         [4, 6],         [6, 4],         [6, 6],         [6, 8],                 [8, 2], [8, 8]]]
[[[0, 6], [1, 6], [2, 6], [3, 6], [4, 6], [5, 6], [6, 4], [6, 5], [6, 6], [6, 7], [6, 8], [7, 3], [7, 8], [8, 2], [8, 8]]]
def test_strat_init():
  strat = Strat('examples/prvni.sh', 0, '0,5,ABC,0')
  assert strat.id == 0
  assert strat.points == 0
  assert strat.stones == 5
  assert strat.potion == ['A', 'B', 'C']
  assert strat.potion_done == False
  assert strat.stratwd == 'examples'
  assert strat.stratbin == 'prvni.sh'
  strat = Strat('examples/druha.sh', 1, '100,0,XYZ,1')
  assert strat.id == 1
  assert strat.points == 100
  assert strat.stones == 0
  assert strat.potion == ['X', 'Y', 'Z']
  assert strat.potion_done == True
  assert strat.stratwd == 'examples'
  assert strat.stratbin == 'druha.sh'

def test_strat_want_to_use():
  plan = Plan(plan_list)
  strat = Strat('examples/prvni.sh', 0, '0,5,HFG,0')
  strat.set_cookbook({})
  expected = {
    'H': [[6, 8], [8, 8]],
    'F': [[8, 2]],
    'G': [[8, 8]]
  }
  returned = strat.want_to_use(plan)
  assert returned == expected

def test_strat_execute():
  strat = Strat('examples/prvni.sh', 0, '0,5,ABC,0')
  plan = Plan(plan_list)
  assert strat.execute(0, plan) == ['|', 1, 3]

def test_plan_init():
  plan = Plan(plan_list)
  assert plan[0][0] == {'C': 3, 'D': 3}
  assert plan[1][8] == {'-': 1}
  assert plan[3][0] == {}
  ##print plan

def test_plan_paths_and_stones():
  plan = Plan(plan_list)
  assert plan.columns_count == 9
  assert plan.rows_count == 9
  ###print plan
  # Test that you can not build path where we have stone
  plan[0][5] = {'|': 3}
  assert plan[0][5] == {'#': 0}
  # Test that you can build pathe where is another path
  plan[1][1] = {'|': 3}
  assert plan[1][1] == {'\\': 1, '|': 3}
  # Test that you can put ston and you can not build path on a vertex
  plan[0][0] = {'#': 3}
  try:
    plan[0][0] = {'|': 3}   # this should raise exception, but ignore it as we will make sure path is not added
  except AssertionError:
    pass
  try:
    plan[0][0] = {'/': 2}   # this should raise exception, but ignore it as we will make sure path is not added
  except AssertionError:
    pass
  try:
    plan[0][0] = {'-': 1}   # this should raise exception, but ignore it as we will make sure path is not added
  except AssertionError:
    pass
  try:
    plan[0][0] = {'\\': 0}   # this should raise exception, but ignore it as we will make sure path is not added
  except AssertionError:
    pass
  assert plan[0][0] == {'C': 3, 'D': 3, '#': 3}
  # Test that putting two stones on one field keeps one first there (but second attempt passes although stone is not added) and you can not build path there
  plan[2][2] = {'#': 3}
  plan[2][2] = {'#': 2}
  try:
    plan[2][2] = {'|': 3}   # this should raise exception, but ignore it as we will make sure path is not added
  except AssertionError:
    pass
  assert plan[2][2] == {'#': 3}
  # Test you cano not replace someones path of same type (but action trying to do so passes)
  plan[3][3] = {'/': 3}
  plan[3][3] = {'/': 2}
  assert plan[3][3] == {'/': 3}
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

def test_plan_put():
  plan = Plan(plan_list)
  # Test we can not put unexpected char
  failed = False
  try:
    plan.put('+', 0, [0,0])
  except AssertionError:
    failed = True
  assert failed
  assert plan[0][0] == {'C': 3, 'D': 3}
  # Test we can put stone
  plan.put('#', 0, [0,0])
  assert plan[0][0] == {'C': 3, 'D': 3, '#': 0}
  # Test we can build path on connected edge
  plan.put('-', 1, [3,8])
  assert plan[3][8] == {'-': 1}
  # Test we can not build path on not connected edge
  failed = False
  try:
    plan.put('|', 0, [0,3])
  except Exception:
    failed = True
  assert failed
  assert plan[0][3] == {}
  # Test we can not build path on foreignly connected edge
  failed = False
  try:
    plan.put('/', 0, [3,1])
  except Exception:
    failed = True
  assert failed
  assert plan[3][1] == {}
  # Test we can not build path on vertex
  failed = False
  try:
    plan.put('/', 0, [6,6])
  except Exception:
    failed = True
  assert failed
  assert plan[6][6] == {}

def test_game_match_plans():
  game = Game('plan.dat', ['examples/prvni.sh', 'examples/prvni.sh', 'examples/prvni.sh', 'examples/druha.sh'])
  plan = Plan(plan_list)
  assert plan == game.playfield

def test_game_cookbook():
  game = Game('plan.dat', ['examples/prvni.sh', 'examples/prvni.sh', 'examples/prvni.sh', 'examples/druha.sh'])
  cookbook = {"ACEH":2, "BDFH":2}
  assert cookbook == game._get_cookbook()

def test_game_init():
  game = Game('plan.dat', ['examples/prvni.sh', 'examples/prvni.sh', 'examples/prvni.sh', 'examples/druha.sh'])
  assert game.round == 123
  assert game.executables[0] == 'examples/prvni.sh'
  assert game.executables[1] == 'examples/prvni.sh'
  assert game.executables[2] == 'examples/prvni.sh'
  assert game.executables[3] == 'examples/druha.sh'
  assert game.playfield[2][8] == {'A': 1}
  assert game.strats[0].potion == ['A','E','H']
  assert game.strats[1].potion == ['C','G','B']
  assert game.strats[2].potion == ['E','A','D']
  assert game.strats[3].potion == ['G','C','F']
  assert game._get_want_to_use() == [{'H': [[6, 8], [8, 8]]}, {'C': [[0, 0], [0, 2]], 'B': [[0, 8]]}, {}, {'C': [[0, 0], [0, 2]]}]

def test_game_get_can_use():
  game = Game('plan.dat', ['examples/prvni.sh', 'examples/prvni.sh', 'examples/prvni.sh', 'examples/druha.sh'])
  want_to_use = [
    {'A': [[2,2]], 'B': [[4,4]]},
    {'B': [[4,4],[6,6]], 'C':[[8,8]], 'D':[[10,10]], 'E': [[0,0]]},
    {'D':[[12,12]]},
    {'C':[[8,8]]},
  ]
  returned = game._get_can_use(want_to_use)
  assert len(returned[0]['A']) == 1
  assert [2,2] in returned[0]['A']
  assert ( 'B' in returned[0] and [4,4] in returned[0]['B'] ) or ( 'B' in returned[1] and [4,4] in returned[1]['B'] )
  assert [6,6] in returned[1]['B']
  assert ( 'C' in returned[1] and [8,8] in returned[1]['C'] ) or ( 'C' in returned[3] and [8,8] in returned[3]['C'] )
  assert [10,10] in returned[1]['D']
  assert [12,12] in returned[2]['D']
  strat = Strat('examples/prvni.sh', 1, '0,5,BDE,0')
  plan_list = [
    ',,,,,,,,,,,,D:1',
    ',,,,,,,,,,,,',
    ',,,,,,,,,-:1,D:1,,',
    ',,,,,,,/:1,,,,,',
    ',,,,,,,,C:1,,,,',
    ',,,,,,|:1,,,,,,',
    ',,,-:1,,-:1,B:1,,,,,,',
    ',/:1,,,,,,,,,,,',
    ',,,,B:1,,,,,,,,',
    '|:1,,,,,,,,,,,,',
    ',,A:1,,,,,,,,,,',
    '|:1,,,,,,,,,,,,',
    'E:2,,,,,,,,,,,,',
  ]
  plan = Plan(plan_list)
  assert plan[0][0] == {'E': 2}
  assert plan[2][2] == {'A': 1}
  assert plan[6][6] == {'B': 1}
  assert strat.potion_done == False
  strat.brew(plan, returned[1])
  assert plan[0][0] == {'E': 1}   # this is correctly decreased
  assert plan[2][2] == {'A': 1}   # this is not touched
  assert plan[6][6] == {}   # this is decreased to noothing on the cell
  assert strat.potion_done == True   # we were brewing our main potion

def test_game_round():
  game = Game('plan.dat', ['examples/prvni.sh', 'examples/prvni.sh', 'examples/prvni.sh', 'examples/druha.sh'])
  game.round()

def main():
  game = Game('plan.dat', sys.argv[:1])

if __name__ == '__main__':
  ###main()
  #test_strat_init()
  #test_strat_want_to_use()
  test_strat_execute()
  #test_plan_init()
  #test_plan_paths_and_stones()
  #test_plan_set_negative()
  #test_plan_same_all_the_time()
  #test_plan_paths_for_strat()
  #test_plan_put()
  #test_game_match_plans()
  #test_game_cookbook()
  #test_game_init()
  #test_game_get_can_use()
  ###test_game_round()

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
